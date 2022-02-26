/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright 2008 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 * Copyright 2011 Joyent, Inc.  All rights reserved.
 * Copyright 2011 Nexenta Systems, Inc.  All rights reserved.
 */

/*
 *	Power Button Driver
 *
 *	This driver handles interrupt generated by the power button on
 *	platforms with "power" device node which has "button" property.
 *	Currently, these platforms are:
 *
 *		ACPI-enabled x86/x64 platforms
 *		Ultra-5_10, Ultra-80, Sun-Blade-100, Sun-Blade-150,
 *		Sun-Blade-1500, Sun-Blade-2500,
 *		Sun-Fire-V210, Sun-Fire-V240, Netra-240
 *
 *	Only one instance is allowed to attach.  In order to know when
 *	an application that has opened the device is going away, a new
 *	minor clone is created for each open(9E) request.  There are
 *	allocations for creating minor clones between 1 and 255.  The ioctl
 *	interface is defined by pbio(4I) and approved as part of
 *	PSARC/1999/393 case.
 */

#include <sys/types.h>
#include <sys/conf.h>
#include <sys/ddi.h>
#include <sys/sunddi.h>
#include <sys/ddi_impldefs.h>
#include <sys/cmn_err.h>
#include <sys/errno.h>
#include <sys/modctl.h>
#include <sys/open.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/pbio.h>
#include <sys/sysevent/eventdefs.h>
#include <sys/sysevent/pwrctl.h>

#if defined(__sparc)
#include <sys/machsystm.h>
#endif

#ifdef	ACPI_POWER_BUTTON

#include <sys/acpi/acpi.h>
#include <sys/acpica.h>

#else

#include <sys/epic.h>
/*
 * Some #defs that must be here as they differ for power.c
 * and epic.c
 */
#define	EPIC_REGS_OFFSET	0x00
#define	EPIC_REGS_LEN		0x82


/*
 * This flag, which is set for platforms,  that have EPIC processor
 * to process power button interrupt, helps in executing platform
 * specific code.
 */
static char 	hasEPIC = B_FALSE;
#endif	/* ACPI_POWER_BUTTON */

/*
 * Maximum number of clone minors that is allowed.  This value
 * is defined relatively low to save memory.
 */
#define	POWER_MAX_CLONE	256

/*
 * Minor number is instance << 8 + clone minor from range 1-255; clone 0
 * is reserved for "original" minor.
 */
#define	POWER_MINOR_TO_CLONE(minor) ((minor) & (POWER_MAX_CLONE - 1))

/*
 * Power Button Abort Delay
 */
#define	ABORT_INCREMENT_DELAY	10

/*
 * FWARC 2005/687: power device compatible property
 */
#define	POWER_DEVICE_TYPE "power-device-type"

/*
 * Driver global variables
 */
static void *power_state;
static int power_inst = -1;

static hrtime_t	power_button_debounce = MSEC2NSEC(10);
static hrtime_t power_button_abort_interval = 1.5 * NANOSEC;
static int	power_button_abort_presses = 3;
static int	power_button_abort_enable = 1;
static int	power_button_enable = 1;

static int	power_button_pressed = 0;
static int	power_button_cancel = 0;
static int	power_button_timeouts = 0;
static int	timeout_cancel = 0;
static int	additional_presses = 0;

/*
 * Function prototypes
 */
static int power_attach(dev_info_t *, ddi_attach_cmd_t);
static int power_detach(dev_info_t *, ddi_detach_cmd_t);
static int power_getinfo(dev_info_t *, ddi_info_cmd_t, void *, void **);
static int power_open(dev_t *, int, int, cred_t *);
static int power_close(dev_t, int, int, cred_t *);
static int power_ioctl(dev_t, int, intptr_t, int, cred_t *, int *);
static int power_chpoll(dev_t, short, int, short *, struct pollhead **);
#ifndef	ACPI_POWER_BUTTON
static uint_t power_high_intr(caddr_t);
#endif
static uint_t power_soft_intr(caddr_t);
static uint_t power_issue_shutdown(caddr_t);
static void power_timeout(caddr_t);
static void power_log_message(void);

/*
 * Structure used in the driver
 */
struct power_soft_state {
	dev_info_t	*dip;		/* device info pointer */
	kmutex_t	power_mutex;	/* mutex lock */
	kmutex_t	power_intr_mutex; /* interrupt mutex lock */
	ddi_iblock_cookie_t soft_iblock_cookie; /* holds interrupt cookie */
	ddi_iblock_cookie_t high_iblock_cookie; /* holds interrupt cookie */
	ddi_softintr_t	softintr_id;	/* soft interrupt id */
	uchar_t		clones[POWER_MAX_CLONE]; /* array of minor clones */
	int		monitor_on;	/* clone monitoring the button event */
					/* clone 0 indicates no one is */
					/* monitoring the button event */
	pollhead_t	pollhd;		/* poll head struct */
	int		events;		/* bit map of occured events */
	int		shutdown_pending; /* system shutdown in progress */
#ifdef	ACPI_POWER_BUTTON
	boolean_t	fixed_attached;	/* true means fixed is attached */
	boolean_t	gpe_attached;	/* true means GPE is attached */
	ACPI_HANDLE	button_obj;	/* handle to device power button */
#else
	ddi_acc_handle_t power_rhandle; /* power button register handle */
	uint8_t		*power_btn_reg;	/* power button register address */
	uint8_t		power_btn_bit;	/* power button register bit */
	boolean_t	power_regs_mapped; /* flag to tell if regs mapped */
	boolean_t	power_btn_ioctl; /* flag to specify ioctl request */
#endif
};

static void power_gen_sysevent(struct power_soft_state *);

#ifdef	ACPI_POWER_BUTTON
static int power_attach_acpi(struct power_soft_state *softsp);
static void power_detach_acpi(struct power_soft_state *softsp);
static UINT32 power_acpi_fixed_event(void *ctx);
#else
static int power_setup_regs(struct power_soft_state *softsp);
static void power_free_regs(struct power_soft_state *softsp);
#endif	/* ACPI_POWER_BUTTON */

/*
 * Configuration data structures
 */
static struct cb_ops power_cb_ops = {
	power_open,		/* open */
	power_close,		/* close */
	nodev,			/* strategy */
	nodev,			/* print */
	nodev,			/* dump */
	nodev,			/* read */
	nodev,			/* write */
	power_ioctl,		/* ioctl */
	nodev,			/* devmap */
	nodev,			/* mmap */
	nodev,			/* segmap */
	power_chpoll,		/* poll */
	ddi_prop_op,		/* cb_prop_op */
	NULL,			/* streamtab */
	D_MP | D_NEW,		/* Driver compatibility flag */
	CB_REV,			/* rev */
	nodev,			/* cb_aread */
	nodev			/* cb_awrite */
};

static struct dev_ops power_ops = {
	DEVO_REV,		/* devo_rev, */
	0,			/* refcnt */
	power_getinfo,		/* getinfo */
	nulldev,		/* identify */
	nulldev,		/* probe */
	power_attach,		/* attach */
	power_detach,		/* detach */
	nodev,			/* reset */
	&power_cb_ops,		/* cb_ops */
	(struct bus_ops *)NULL,	/* bus_ops */
	NULL,			/* power */
	ddi_quiesce_not_supported,	/* devo_quiesce */
};

static struct modldrv modldrv = {
	&mod_driverops,		/* Type of module.  This one is a driver */
	"power button driver",	/* name of module */
	&power_ops,		/* driver ops */
};

static struct modlinkage modlinkage = {
	MODREV_1,
	(void *)&modldrv,
	NULL
};

/*
 * These are the module initialization routines.
 */

int
_init(void)
{
	int error;

	if ((error = ddi_soft_state_init(&power_state,
	    sizeof (struct power_soft_state), 0)) != 0)
		return (error);

	if ((error = mod_install(&modlinkage)) != 0)
		ddi_soft_state_fini(&power_state);

	return (error);
}

int
_fini(void)
{
	int error;

	if ((error = mod_remove(&modlinkage)) == 0)
		ddi_soft_state_fini(&power_state);

	return (error);
}

int
_info(struct modinfo *modinfop)
{
	return (mod_info(&modlinkage, modinfop));
}

/*ARGSUSED*/
static int
power_getinfo(dev_info_t *dip, ddi_info_cmd_t infocmd, void *arg,
    void **result)
{
	struct power_soft_state *softsp;

	if (power_inst == -1)
		return (DDI_FAILURE);

	switch (infocmd) {
	case DDI_INFO_DEVT2DEVINFO:
		if ((softsp = ddi_get_soft_state(power_state, power_inst))
		    == NULL)
			return (DDI_FAILURE);
		*result = (void *)softsp->dip;
		return (DDI_SUCCESS);

	case DDI_INFO_DEVT2INSTANCE:
		*result = (void *)(uintptr_t)power_inst;
		return (DDI_SUCCESS);

	default:
		return (DDI_FAILURE);
	}
}

static int
power_attach(dev_info_t *dip, ddi_attach_cmd_t cmd)
{
	struct power_soft_state *softsp;

	switch (cmd) {
	case DDI_ATTACH:
		break;
	case DDI_RESUME:
		return (DDI_SUCCESS);
	default:
		return (DDI_FAILURE);
	}

	/*
	 * If the power node doesn't have "button" property, quietly
	 * fail to attach.
	 */
	if (ddi_prop_exists(DDI_DEV_T_ANY, dip, DDI_PROP_DONTPASS,
	    "button") == 0)
		return (DDI_FAILURE);

	if (power_inst != -1)
		return (DDI_FAILURE);

	power_inst = ddi_get_instance(dip);

	if (ddi_soft_state_zalloc(power_state, power_inst) != DDI_SUCCESS)
		return (DDI_FAILURE);

	if (ddi_create_minor_node(dip, "power_button", S_IFCHR,
	    (power_inst << 8) + 0, "ddi_power_button", 0) != DDI_SUCCESS)
		return (DDI_FAILURE);

	softsp = ddi_get_soft_state(power_state, power_inst);
	softsp->dip = dip;

#ifdef	ACPI_POWER_BUTTON
	(void) power_attach_acpi(softsp);
#else
	if (power_setup_regs(softsp) != DDI_SUCCESS) {
		cmn_err(CE_WARN, "power_attach: failed to setup registers");
		goto error;
	}

	if (ddi_get_iblock_cookie(dip, 0,
	    &softsp->high_iblock_cookie) != DDI_SUCCESS) {
		cmn_err(CE_WARN, "power_attach: ddi_get_soft_iblock_cookie "
		    "failed.");
		goto error;
	}
	mutex_init(&softsp->power_intr_mutex, NULL, MUTEX_DRIVER,
	    softsp->high_iblock_cookie);

	if (ddi_add_intr(dip, 0, &softsp->high_iblock_cookie, NULL,
	    power_high_intr, (caddr_t)softsp) != DDI_SUCCESS) {
		cmn_err(CE_WARN, "power_attach: failed to add high-level "
		    " interrupt handler.");
		mutex_destroy(&softsp->power_intr_mutex);
		goto error;
	}
#endif	/* ACPI_POWER_BUTTON */

	if (ddi_get_soft_iblock_cookie(dip, DDI_SOFTINT_LOW,
	    &softsp->soft_iblock_cookie) != DDI_SUCCESS) {
		cmn_err(CE_WARN, "power_attach: ddi_get_soft_iblock_cookie "
		    "failed.");
		mutex_destroy(&softsp->power_intr_mutex);
		ddi_remove_intr(dip, 0, NULL);
		goto error;
	}

	mutex_init(&softsp->power_mutex, NULL, MUTEX_DRIVER,
	    (void *)softsp->soft_iblock_cookie);

	if (ddi_add_softintr(dip, DDI_SOFTINT_LOW, &softsp->softintr_id,
	    NULL, NULL, power_soft_intr, (caddr_t)softsp) != DDI_SUCCESS) {
		cmn_err(CE_WARN, "power_attach: failed to add soft "
		    "interrupt handler.");
		mutex_destroy(&softsp->power_mutex);
		mutex_destroy(&softsp->power_intr_mutex);
		ddi_remove_intr(dip, 0, NULL);
		goto error;
	}

	ddi_report_dev(dip);

	return (DDI_SUCCESS);

error:
#ifdef	ACPI_POWER_BUTTON
	/*
	 * detach ACPI power button
	 */
	power_detach_acpi(softsp);
#else
	power_free_regs(softsp);
#endif	/* ACPI_POWER_BUTTON */
	ddi_remove_minor_node(dip, "power_button");
	ddi_soft_state_free(power_state, power_inst);
	return (DDI_FAILURE);
}

/*ARGSUSED*/
/*
 * This driver doesn't detach.
 */
static int
power_detach(dev_info_t *dip, ddi_detach_cmd_t cmd)
{
	/*
	 * Since the "power" node has "reg" property, as part of
	 * the suspend operation, detach(9E) entry point is called.
	 * There is no state to save, since this register is used
	 * by OBP to power off the system and the state of the
	 * power off is preserved by hardware.
	 */
	return ((cmd == DDI_SUSPEND) ? DDI_SUCCESS :
	    DDI_FAILURE);
}


#ifndef	ACPI_POWER_BUTTON
/*
 * Handler for the high-level interrupt.
 */
static uint_t
power_high_intr(caddr_t arg)
{
	struct power_soft_state *softsp = (struct power_soft_state *)arg;
	ddi_acc_handle_t hdl = softsp->power_rhandle;
	uint8_t		reg;

	hrtime_t tstamp;
	static hrtime_t o_tstamp = 0;
	static hrtime_t power_button_tstamp = 0;
	static int power_button_cnt;

	if (softsp->power_regs_mapped) {
		mutex_enter(&softsp->power_intr_mutex);

		/* Check if power button interrupt is delivered by EPIC HW */
		if (hasEPIC) {
			/* read isr - first issue command */
			EPIC_WR(hdl, softsp->power_btn_reg,
			    EPIC_ATOM_INTR_READ);
			/* next, read the reg */
			EPIC_RD(hdl, softsp->power_btn_reg, reg);

			if (reg & EPIC_FIRE_INTERRUPT) {  /* PB pressed */
				/* clear the interrupt */
				EPIC_WR(hdl, softsp->power_btn_reg,
				    EPIC_ATOM_INTR_CLEAR);
			} else {
				if (!softsp->power_btn_ioctl) {
					mutex_exit(&softsp->power_intr_mutex);
					return (DDI_INTR_CLAIMED);
				}
				softsp->power_btn_ioctl = B_FALSE;
			}
		} else {
			reg = ddi_get8(hdl, softsp->power_btn_reg);
			if (reg & softsp->power_btn_bit) {
				reg &= softsp->power_btn_bit;
				ddi_put8(hdl, softsp->power_btn_reg, reg);
				(void) ddi_get8(hdl, softsp->power_btn_reg);
			} else {
				if (!softsp->power_btn_ioctl) {
					mutex_exit(&softsp->power_intr_mutex);
					return (DDI_INTR_CLAIMED);
				}
				softsp->power_btn_ioctl = B_FALSE;
			}
		}
		mutex_exit(&softsp->power_intr_mutex);
	}

	tstamp = gethrtime();

	/* need to deal with power button debounce */
	if (o_tstamp && (tstamp - o_tstamp) < power_button_debounce) {
		o_tstamp = tstamp;
		return (DDI_INTR_CLAIMED);
	}
	o_tstamp = tstamp;

	power_button_cnt++;

	mutex_enter(&softsp->power_intr_mutex);
	power_button_pressed++;
	mutex_exit(&softsp->power_intr_mutex);

	/*
	 * If power button abort is enabled and power button was pressed
	 * power_button_abort_presses times within power_button_abort_interval
	 * then call abort_sequence_enter();
	 */
	if (power_button_abort_enable) {
		if (power_button_abort_presses == 1 ||
		    tstamp < (power_button_tstamp +
		    power_button_abort_interval)) {
			if (power_button_cnt == power_button_abort_presses) {
				mutex_enter(&softsp->power_intr_mutex);
				power_button_cancel += power_button_timeouts;
				power_button_pressed = 0;
				mutex_exit(&softsp->power_intr_mutex);
				power_button_cnt = 0;
				abort_sequence_enter("Power Button Abort");
				return (DDI_INTR_CLAIMED);
			}
		} else {
			power_button_cnt = 1;
			power_button_tstamp = tstamp;
		}
	}

	if (!power_button_enable)
		return (DDI_INTR_CLAIMED);

	/* post softint to issue timeout for power button action */
	if (softsp->softintr_id != NULL)
		ddi_trigger_softintr(softsp->softintr_id);

	return (DDI_INTR_CLAIMED);
}
#endif	/* ifndef ACPI_POWER_BUTTON */

/*
 * Handle the softints....
 *
 * If only one softint is posted for several button presses, record
 * the number of additional presses just incase this was actually not quite
 * an Abort sequence so that we can log this event later.
 *
 * Issue a timeout with a duration being a fraction larger than
 * the specified Abort interval inorder to perform a power down if required.
 */
static uint_t
power_soft_intr(caddr_t arg)
{
	struct power_soft_state *softsp = (struct power_soft_state *)arg;

	if (!power_button_abort_enable)
		return (power_issue_shutdown(arg));

	mutex_enter(&softsp->power_intr_mutex);
	if (!power_button_pressed) {
		mutex_exit(&softsp->power_intr_mutex);
		return (DDI_INTR_CLAIMED);
	}

	/*
	 * Schedule a timeout to do the necessary
	 * work for shutdown, only one timeout for
	 * n presses if power button was pressed
	 * more than once before softint fired
	 */
	if (power_button_pressed > 1)
		additional_presses += power_button_pressed - 1;

	timeout_cancel = 0;
	power_button_pressed = 0;
	power_button_timeouts++;
	mutex_exit(&softsp->power_intr_mutex);
	(void) timeout((void(*)(void *))power_timeout,
	    softsp, NSEC_TO_TICK(power_button_abort_interval) +
	    ABORT_INCREMENT_DELAY);

	return (DDI_INTR_CLAIMED);
}

/*
 * Upon receiving a timeout the following is determined:
 *
 * If an  Abort sequence was issued, then we cancel all outstanding timeouts
 * and additional presses prior to the Abort sequence.
 *
 * If we had multiple timeouts issued and the abort sequence was not met,
 * then we had more than one button press to power down the machine. We
 * were probably trying to issue an abort. So log a message indicating this
 * and cancel all outstanding timeouts.
 *
 * If we had just one timeout and the abort sequence was not met then
 * we really did want to power down the machine, so call power_issue_shutdown()
 * to do the work and schedule a power down
 */
static void
power_timeout(caddr_t arg)
{
	struct power_soft_state *softsp = (struct power_soft_state *)arg;
	static int first = 0;

	/*
	 * Abort was generated cancel all outstanding power
	 * button timeouts
	 */
	mutex_enter(&softsp->power_intr_mutex);
	if (power_button_cancel) {
		power_button_cancel--;
		power_button_timeouts--;
		if (!first) {
			first++;
			additional_presses = 0;
		}
		mutex_exit(&softsp->power_intr_mutex);
		return;
	}
	first = 0;

	/*
	 * We get here if the timeout(s) have fired and they were
	 * not issued prior to an abort.
	 *
	 * If we had more than one press in the interval we were
	 * probably trying to issue an abort, but didnt press the
	 * required number within the interval. Hence cancel all
	 * timeouts and do not continue towards shutdown.
	 */
	if (!timeout_cancel) {
		timeout_cancel = power_button_timeouts +
		    additional_presses;

		power_button_timeouts--;
		if (!power_button_timeouts)
			additional_presses = 0;

		if (timeout_cancel > 1) {
			mutex_exit(&softsp->power_intr_mutex);
			cmn_err(CE_NOTE, "Power Button pressed "
			    "%d times, cancelling all requests",
			    timeout_cancel);
			return;
		}
		mutex_exit(&softsp->power_intr_mutex);

		/* Go and do the work to request shutdown */
		(void) power_issue_shutdown((caddr_t)softsp);
		return;
	}

	power_button_timeouts--;
	if (!power_button_timeouts)
		additional_presses = 0;
	mutex_exit(&softsp->power_intr_mutex);
}

#ifdef ACPI_POWER_BUTTON
static void
do_shutdown(void)
{
	proc_t *initpp;

	/*
	 * If we're still booting and init(1) isn't set up yet, simply halt.
	 */
	mutex_enter(&pidlock);
	initpp = prfind(P_INITPID);
	mutex_exit(&pidlock);
	if (initpp == NULL) {
		extern void halt(char *);
		halt("Power off the System");   /* just in case */
	}

	/*
	 * else, graceful shutdown with inittab and all getting involved
	 */
	psignal(initpp, SIGPWR);
}
#endif

static uint_t
power_issue_shutdown(caddr_t arg)
{
	struct power_soft_state *softsp = (struct power_soft_state *)arg;

	mutex_enter(&softsp->power_mutex);
	softsp->events |= PB_BUTTON_PRESS;
	if (softsp->monitor_on != 0) {
		mutex_exit(&softsp->power_mutex);
		pollwakeup(&softsp->pollhd, POLLRDNORM);
		pollwakeup(&softsp->pollhd, POLLIN);
		power_gen_sysevent(softsp);
		return (DDI_INTR_CLAIMED);
	}

	if (!softsp->shutdown_pending) {
		cmn_err(CE_WARN, "Power off requested from power button or "
		    "SC, powering down the system!");
		softsp->shutdown_pending = 1;
		do_shutdown();

		/*
		 * Wait a while for "do_shutdown()" to shut down the system
		 * before logging an error message.
		 */
		(void) timeout((void(*)(void *))power_log_message, NULL,
		    100 * hz);
	}
	mutex_exit(&softsp->power_mutex);

	return (DDI_INTR_CLAIMED);
}

static void
power_gen_sysevent(struct power_soft_state *softsp)
{
	nvlist_t *attr_list = NULL;
	int err;
	char pathname[MAXPATHLEN];
	char hid[9] = "\0";

	/* Allocate and build sysevent attribute list */
	err = nvlist_alloc(&attr_list, NV_UNIQUE_NAME_TYPE, DDI_NOSLEEP);
	if (err != 0) {
		cmn_err(CE_WARN,
		    "cannot allocate memory for sysevent attributes\n");
		return;
	}

#ifdef ACPI_POWER_BUTTON
	/* Only control method power button has HID */
	if (softsp->gpe_attached) {
		(void) strlcpy(hid, "PNP0C0C", sizeof (hid));
	}
#endif

	err = nvlist_add_string(attr_list, PWRCTL_DEV_HID, hid);
	if (err != 0) {
		cmn_err(CE_WARN,
		    "Failed to add attr [%s] for %s/%s event",
		    PWRCTL_DEV_HID, EC_PWRCTL, ESC_PWRCTL_POWER_BUTTON);
		nvlist_free(attr_list);
		return;
	}

	(void) ddi_pathname(softsp->dip, pathname);
	err = nvlist_add_string(attr_list, PWRCTL_DEV_PHYS_PATH, pathname);
	if (err != 0) {
		cmn_err(CE_WARN,
		    "Failed to add attr [%s] for %s/%s event",
		    PWRCTL_DEV_PHYS_PATH, EC_PWRCTL, ESC_PWRCTL_POWER_BUTTON);
		nvlist_free(attr_list);
		return;
	}

	/* Generate/log sysevent */
	err = ddi_log_sysevent(softsp->dip, DDI_VENDOR_SUNW, EC_PWRCTL,
	    ESC_PWRCTL_POWER_BUTTON, attr_list, NULL, DDI_NOSLEEP);
	if (err != DDI_SUCCESS) {
		cmn_err(CE_WARN,
		    "cannot log sysevent, err code %x\n", err);
	}

	nvlist_free(attr_list);
}

/*
 * Open the device.
 */
/*ARGSUSED*/
static int
power_open(dev_t *devp, int openflags, int otyp, cred_t *credp)
{
	struct power_soft_state *softsp;
	int clone;

	if (otyp != OTYP_CHR)
		return (EINVAL);

	if ((softsp = ddi_get_soft_state(power_state, power_inst)) ==
	    NULL)
		return (ENXIO);

	mutex_enter(&softsp->power_mutex);
	for (clone = 1; clone < POWER_MAX_CLONE; clone++)
		if (!softsp->clones[clone])
			break;

	if (clone == POWER_MAX_CLONE) {
		cmn_err(CE_WARN, "power_open: No more allocation left "
		    "to create a clone minor.");
		mutex_exit(&softsp->power_mutex);
		return (ENXIO);
	}

	*devp = makedevice(getmajor(*devp), (power_inst << 8) + clone);
	softsp->clones[clone] = 1;
	mutex_exit(&softsp->power_mutex);

	return (0);
}

/*
 * Close the device.
 */
/*ARGSUSED*/
static  int
power_close(dev_t dev, int openflags, int otyp, cred_t *credp)
{
	struct power_soft_state *softsp;
	int clone;

	if (otyp != OTYP_CHR)
		return (EINVAL);

	if ((softsp = ddi_get_soft_state(power_state, power_inst)) ==
	    NULL)
		return (ENXIO);

	clone = POWER_MINOR_TO_CLONE(getminor(dev));
	mutex_enter(&softsp->power_mutex);
	if (softsp->monitor_on == clone)
		softsp->monitor_on = 0;
	softsp->clones[clone] = 0;
	mutex_exit(&softsp->power_mutex);

	return (0);
}

/*ARGSUSED*/
static  int
power_ioctl(dev_t dev, int cmd, intptr_t arg, int mode, cred_t *cred_p,
    int *rval_p)
{
	struct power_soft_state *softsp;
	int clone;

	if ((softsp = ddi_get_soft_state(power_state, power_inst)) ==
	    NULL)
		return (ENXIO);

	clone = POWER_MINOR_TO_CLONE(getminor(dev));
	switch (cmd) {
	case PB_BEGIN_MONITOR:
		mutex_enter(&softsp->power_mutex);
		if (softsp->monitor_on) {
			mutex_exit(&softsp->power_mutex);
			return (EBUSY);
		}
		softsp->monitor_on = clone;
		mutex_exit(&softsp->power_mutex);
		return (0);

	case PB_END_MONITOR:
		mutex_enter(&softsp->power_mutex);

		/*
		 * If PB_END_MONITOR is called without first
		 * calling PB_BEGIN_MONITOR, an error will be
		 * returned.
		 */
		if (!softsp->monitor_on) {
			mutex_exit(&softsp->power_mutex);
			return (ENXIO);
		}

		/*
		 * This clone is not monitoring the button.
		 */
		if (softsp->monitor_on != clone) {
			mutex_exit(&softsp->power_mutex);
			return (EINVAL);
		}
		softsp->monitor_on = 0;
		mutex_exit(&softsp->power_mutex);
		return (0);

	case PB_GET_EVENTS:
		mutex_enter(&softsp->power_mutex);
		if (ddi_copyout((void *)&softsp->events, (void *)arg,
		    sizeof (int), mode) != 0) {
			mutex_exit(&softsp->power_mutex);
			return (EFAULT);
		}

		/*
		 * This ioctl returned the events detected since last
		 * call.  Note that any application can get the events
		 * and clear the event register.
		 */
		softsp->events = 0;
		mutex_exit(&softsp->power_mutex);
		return (0);

	/*
	 * This ioctl is used by the test suite.
	 */
	case PB_CREATE_BUTTON_EVENT:
#ifdef	ACPI_POWER_BUTTON
		(UINT32)power_acpi_fixed_event((void *)softsp);
#else
		if (softsp->power_regs_mapped) {
			mutex_enter(&softsp->power_intr_mutex);
			softsp->power_btn_ioctl = B_TRUE;
			mutex_exit(&softsp->power_intr_mutex);
		}
		(void) power_high_intr((caddr_t)softsp);
#endif	/* ACPI_POWER_BUTTON */
		return (0);

	default:
		return (ENOTTY);
	}
}

/*ARGSUSED*/
static int
power_chpoll(dev_t dev, short events, int anyyet,
    short *reventsp, struct pollhead **phpp)
{
	struct power_soft_state *softsp;

	if ((softsp = ddi_get_soft_state(power_state, power_inst)) == NULL)
		return (ENXIO);

	mutex_enter(&softsp->power_mutex);
	*reventsp = 0;
	if (softsp->events)
		*reventsp = POLLRDNORM|POLLIN;
	else {
		if (!anyyet)
			*phpp = &softsp->pollhd;
	}
	mutex_exit(&softsp->power_mutex);

	return (0);
}

static void
power_log_message(void)
{
	struct power_soft_state *softsp;

	if ((softsp = ddi_get_soft_state(power_state, power_inst)) == NULL) {
		cmn_err(CE_WARN, "Failed to get internal state!");
		return;
	}

	mutex_enter(&softsp->power_mutex);
	softsp->shutdown_pending = 0;
	cmn_err(CE_WARN, "Failed to shut down the system!");
	mutex_exit(&softsp->power_mutex);
}

#ifdef	ACPI_POWER_BUTTON
/*
 *
 */
/*ARGSUSED*/
static ACPI_STATUS
acpi_device(ACPI_HANDLE obj, UINT32 nesting, void *context, void **rv)
{

	*((ACPI_HANDLE *)context) = obj;
	return (AE_OK);
}

/*
 *
 */
static ACPI_HANDLE
probe_acpi_pwrbutton()
{
	ACPI_HANDLE obj = NULL;

	(void) AcpiGetDevices("PNP0C0C", acpi_device, (void *)&obj, NULL);
	return (obj);
}

static UINT32
power_acpi_fixed_event(void *ctx)
{

	mutex_enter(&((struct power_soft_state *)ctx)->power_intr_mutex);
	power_button_pressed++;
	mutex_exit(&((struct power_soft_state *)ctx)->power_intr_mutex);

	/* post softint to issue timeout for power button action */
	if (((struct power_soft_state *)ctx)->softintr_id != NULL)
		ddi_trigger_softintr(
		    ((struct power_soft_state *)ctx)->softintr_id);

	return (AE_OK);
}

/*ARGSUSED*/
static void
power_acpi_notify_event(ACPI_HANDLE obj, UINT32 val, void *ctx)
{
	if (val == 0x80)
		(void) power_acpi_fixed_event(ctx);
}

/*
 *
 */
static int
power_probe_method_button(struct power_soft_state *softsp)
{
	ACPI_HANDLE button_obj;

	button_obj = probe_acpi_pwrbutton();
	softsp->button_obj = button_obj;	/* remember obj */
	if ((button_obj != NULL) &&
	    (AcpiInstallNotifyHandler(button_obj, ACPI_DEVICE_NOTIFY,
	    power_acpi_notify_event, (void*)softsp) == AE_OK))
		return (1);
	return (0);
}

/*
 *
 */
static int
power_probe_fixed_button(struct power_soft_state *softsp)
{
	ACPI_TABLE_FADT *fadt;

	if (AcpiGetTable(ACPI_SIG_FADT, 1, (ACPI_TABLE_HEADER **) &fadt) !=
	    AE_OK)
		return (0);

	if ((fadt->Flags & ACPI_FADT_POWER_BUTTON) == 0) {
		if (AcpiInstallFixedEventHandler(ACPI_EVENT_POWER_BUTTON,
		    power_acpi_fixed_event, (void *)softsp) == AE_OK)
			return (1);
	}
	return (0);
}


/*
 *
 */
static int
power_attach_acpi(struct power_soft_state *softsp)
{

	/*
	 * If we've attached anything already, return an error
	 */
	if ((softsp->gpe_attached) || (softsp->fixed_attached))
		return (DDI_FAILURE);

	/*
	 * attempt to attach both a fixed-event handler and a GPE
	 * handler; remember what we got
	 */
	softsp->fixed_attached = (power_probe_fixed_button(softsp) != 0);
	softsp->gpe_attached = (power_probe_method_button(softsp) != 0);

	/*
	 * If we've attached anything now, return success
	 */
	if ((softsp->gpe_attached) || (softsp->fixed_attached))
		return (DDI_SUCCESS);

	return (DDI_FAILURE);
}

/*
 *
 */
static void
power_detach_acpi(struct power_soft_state *softsp)
{
	if (softsp->gpe_attached) {
		if (AcpiRemoveNotifyHandler(softsp->button_obj,
		    ACPI_DEVICE_NOTIFY, power_acpi_notify_event) != AE_OK)
			cmn_err(CE_WARN, "!power: failed to remove Notify"
			    " handler");
	}

	if (softsp->fixed_attached) {
		if (AcpiRemoveFixedEventHandler(ACPI_EVENT_POWER_BUTTON,
		    power_acpi_fixed_event) != AE_OK)
			cmn_err(CE_WARN, "!power: failed to remove Power"
			    " Button handler");
	}
}

#else
/*
 * Code for platforms that have EPIC processor for processing power
 * button interrupts.
 */
static int
power_setup_epic_regs(dev_info_t *dip, struct power_soft_state *softsp)
{
	ddi_device_acc_attr_t	attr;
	uint8_t *reg_base;

	attr.devacc_attr_version = DDI_DEVICE_ATTR_V0;
	attr.devacc_attr_endian_flags = DDI_STRUCTURE_LE_ACC;
	attr.devacc_attr_dataorder = DDI_STRICTORDER_ACC;
	if (ddi_regs_map_setup(dip, 0, (caddr_t *)&reg_base,
	    EPIC_REGS_OFFSET, EPIC_REGS_LEN, &attr,
	    &softsp->power_rhandle) != DDI_SUCCESS) {
		return (DDI_FAILURE);
	}

	softsp->power_btn_reg = reg_base;
	softsp->power_regs_mapped = B_TRUE;

	/* Clear power button interrupt first */
	EPIC_WR(softsp->power_rhandle, softsp->power_btn_reg,
	    EPIC_ATOM_INTR_CLEAR);

	/* Enable EPIC interrupt for power button single press event */
	EPIC_WR(softsp->power_rhandle, softsp->power_btn_reg,
	    EPIC_ATOM_INTR_ENABLE);

	/*
	 * At this point, EPIC interrupt processing is fully initialised.
	 */
	hasEPIC = B_TRUE;
	return (DDI_SUCCESS);
}

/*
 *
 * power button register definitions for acpi register on m1535d
 */
#define	M1535D_PWR_BTN_REG_01		0x1
#define	M1535D_PWR_BTN_EVENT_FLAG	0x1

static int
power_setup_m1535_regs(dev_info_t *dip, struct power_soft_state *softsp)
{
	ddi_device_acc_attr_t	attr;
	uint8_t *reg_base;

	attr.devacc_attr_version = DDI_DEVICE_ATTR_V0;
	attr.devacc_attr_endian_flags = DDI_STRUCTURE_LE_ACC;
	attr.devacc_attr_dataorder = DDI_STRICTORDER_ACC;
	if (ddi_regs_map_setup(dip, 0, (caddr_t *)&reg_base, 0, 0, &attr,
	    &softsp->power_rhandle) != DDI_SUCCESS) {
		return (DDI_FAILURE);
	}
	softsp->power_btn_reg = &reg_base[M1535D_PWR_BTN_REG_01];
	softsp->power_btn_bit = M1535D_PWR_BTN_EVENT_FLAG;
	softsp->power_regs_mapped = B_TRUE;
	return (DDI_SUCCESS);
}

/*
 * MBC Fire/SSI Interrupt Status Register definitions
 */
#define	FIRE_SSI_ISR			0x0
#define	FIRE_SSI_INTR_ENA		0x8
#define	FIRE_SSI_SHUTDOWN_REQ		0x4

static int
power_setup_mbc_regs(dev_info_t *dip, struct power_soft_state *softsp)
{
	ddi_device_acc_attr_t   attr;
	uint8_t *reg_base;
	ddi_acc_handle_t hdl;
	uint8_t reg;

	attr.devacc_attr_version = DDI_DEVICE_ATTR_V0;
	attr.devacc_attr_endian_flags = DDI_STRUCTURE_LE_ACC;
	attr.devacc_attr_dataorder = DDI_STRICTORDER_ACC;
	if (ddi_regs_map_setup(dip, 0, (caddr_t *)&reg_base, 0, 0, &attr,
	    &softsp->power_rhandle) != DDI_SUCCESS) {
		return (DDI_FAILURE);
	}
	softsp->power_btn_reg = &reg_base[FIRE_SSI_ISR];
	softsp->power_btn_bit = FIRE_SSI_SHUTDOWN_REQ;
	hdl = softsp->power_rhandle;
	/*
	 * Clear MBC Fire Power Button interrupt, if set.
	 */
	reg = ddi_get8(hdl, softsp->power_btn_reg);
	if (reg & softsp->power_btn_bit) {
		reg &= softsp->power_btn_bit;
		ddi_put8(hdl, softsp->power_btn_reg, reg);
		(void) ddi_get8(hdl, softsp->power_btn_reg);
	}
	/*
	 * Enable MBC Fire Power Button interrupt.
	 */
	reg = ddi_get8(hdl, &reg_base[FIRE_SSI_INTR_ENA]);
	reg |= FIRE_SSI_SHUTDOWN_REQ;
	ddi_put8(hdl, &reg_base[FIRE_SSI_INTR_ENA], reg);

	softsp->power_regs_mapped = B_TRUE;

	return (DDI_SUCCESS);
}

/*
 * Setup register map for the power button
 * NOTE:- we only map registers for platforms if
 * the OBP power device has any of the following
 * properties:
 *
 * a) Boston:  power-device-type set to "SUNW,mbc"
 * b) Seattle: power-device-type set to "SUNW,pic18lf65j10"
 * c) Chalupa: compatible set to "ali1535d+-power"
 *
 * Cases (a) and (b) are defined in FWARC 2005/687.
 * If none of the above conditions are true, then we
 * do not need to map in any registers, and this
 * function can simply return DDI_SUCCESS.
 */
static int
power_setup_regs(struct power_soft_state *softsp)
{
	char	*binding_name;
	char	*power_type = NULL;
	int	retval = DDI_SUCCESS;

	softsp->power_regs_mapped = B_FALSE;
	softsp->power_btn_ioctl = B_FALSE;
	binding_name = ddi_binding_name(softsp->dip);
	if (ddi_prop_lookup_string(DDI_DEV_T_ANY, softsp->dip,
	    DDI_PROP_DONTPASS, POWER_DEVICE_TYPE,
	    &power_type) == DDI_PROP_SUCCESS) {
		if (strcmp(power_type, "SUNW,mbc") == 0) {
			retval = power_setup_mbc_regs(softsp->dip, softsp);
		} else if (strcmp(power_type, "SUNW,pic18lf65j10") == 0) {
			retval = power_setup_epic_regs(softsp->dip, softsp);
		} else {
			cmn_err(CE_WARN, "unexpected power-device-type: %s\n",
			    power_type);
			retval = DDI_FAILURE;
		}
		ddi_prop_free(power_type);
	} else if (strcmp(binding_name, "ali1535d+-power") == 0) {
		retval = power_setup_m1535_regs(softsp->dip, softsp);
	}

	/*
	 * If power-device-type does not exist AND the binding name is not
	 * "ali1535d+-power", that means there is no additional HW and hence
	 * no extra processing is necessary. In that case, retval should still
	 * be set to its initial value of DDI_SUCCESS.
	 */
	return (retval);
}

static void
power_free_regs(struct power_soft_state *softsp)
{
	if (softsp->power_regs_mapped)
		ddi_regs_map_free(&softsp->power_rhandle);
}
#endif	/* ACPI_POWER_BUTTON */

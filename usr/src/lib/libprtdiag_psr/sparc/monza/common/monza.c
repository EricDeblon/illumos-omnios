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
 * Copyright 2007 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*
 * Sun4v Platform specific functions.
 *
 * 	called when :
 *      machine_type ==  monza
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <kstat.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <libintl.h>
#include <note.h>
#include <sys/systeminfo.h>
#include <sys/openpromio.h>
#include <sys/sysmacros.h>
#include <picl.h>
#include "picldefs.h"
#include <pdevinfo.h>
#include <display.h>
#include <display_sun4v.h>
#include <libprtdiag.h>
#include "monza.h"

#if !defined(TEXT_DOMAIN)
#define	TEXT_DOMAIN	"SYS_TEST"
#endif

/*
 * These functions will overlay the symbol table of libprtdiag
 * at runtime
 */
void sun4v_display_pci(picl_nodehdl_t plafh);
void sun4v_display_diaginfo(int flag, Prom_node *root, picl_nodehdl_t plafh);


/*
 * local functions
 */
static void sun4v_display_hw_revisions(Prom_node *root, picl_nodehdl_t plafh);
static int monza_pci_callback(picl_nodehdl_t pcih, void *args);
static int monza_get_first_compatible_value(picl_nodehdl_t nodeh,
    char **outbuf);
static int64_t monza_get_int_propval(picl_nodehdl_t modh, char *prop_name,
    int *ret);

static void
monza_get_bus_type(char *path, struct io_card *card)
{
	if (strcmp(path, MONZA_NIU) == 0) {
		(void) strcpy(card->bus_type, "NIU");
	} else {
		(void) strcpy(card->bus_type, "PCIE");
	}
}

static void
monza_get_slot_number(char *path, struct io_card *card)
{
	if (strcmp(path, MONZA_N2_XAUI0) == 0) {
		(void) strcpy(card->slot_str, "0");
		card->slot = 0;
	} else if (strcmp(path, MONZA_N2_XAUI1) == 0) {
		(void) strcpy(card->slot_str, "1");
		card->slot = 1;
	} else {
		(void) strcpy(card->slot_str, MOTHERBOARD);
		card->slot = -1;
	}
}

static int
monza_get_network_instance(char *path)
{

	if (strncmp(path, MONZA_NETWORK_0, strlen(MONZA_NETWORK_0)) == 0) {
		return (0);
	} else if (strncmp(path, MONZA_NETWORK_1, strlen(MONZA_NETWORK_1))
	    == 0) {
		return (1);
	} else if (strncmp(path, MONZA_NETWORK_2, strlen(MONZA_NETWORK_2))
	    == 0) {
		return (2);
	} else if (strncmp(path, MONZA_ENET_2, strlen(MONZA_ENET_2))
	    == 0) {
		return (2);
	} else if (strncmp(path, MONZA_NETWORK_3, strlen(MONZA_NETWORK_3))
	    == 0) {
		return (3);
	} else if (strncmp(path, MONZA_ENET_3, strlen(MONZA_ENET_3))
	    == 0) {
		return (3);
	} else if (strncmp(path, MONZA_NETWORK_4, strlen(MONZA_NETWORK_4))
	    == 0) {
		return (4);
	} else if (strncmp(path, MONZA_NETWORK_5, strlen(MONZA_NETWORK_5))
	    == 0) {
		return (5);
	} else if (strncmp(path, MONZA_N2_XAUI0, strlen(MONZA_N2_XAUI0))
	    == 0) {
		return (0);
	} else if (strncmp(path, MONZA_N2_XAUI1, strlen(MONZA_N2_XAUI1))
	    == 0) {
		return (1);
	} else {
		return (-1);
	}
}

/*
 * add all io devices under pci in io list
 */
/* ARGSUSED */
static int
monza_pci_callback(picl_nodehdl_t pcih, void *args)
{
	int		err = PICL_SUCCESS;
	picl_nodehdl_t	nodeh;
	char		path[MAXSTRLEN];
	char		parent_path[MAXSTRLEN];
	char		piclclass[PICL_CLASSNAMELEN_MAX];
	char		name[MAXSTRLEN];
	char		model[MAXSTRLEN];
	char		*compatible;
	char		binding_name[MAXSTRLEN];
	struct io_card	pci_card;
	int32_t		instance;

	err = picl_get_propval_by_name(pcih, PICL_PROP_DEVFS_PATH, parent_path,
	    sizeof (parent_path));
	if (err != PICL_SUCCESS) {
		return (err);
	}

	/*
	 * Walk through the children
	 */
	err = picl_get_propval_by_name(pcih, PICL_PROP_CHILD, &nodeh,
	    sizeof (picl_nodehdl_t));

	while (err == PICL_SUCCESS) {
		err = picl_get_propval_by_name(nodeh, PICL_PROP_CLASSNAME,
		    piclclass, sizeof (piclclass));
		if (err !=  PICL_SUCCESS)
			return (err);

		/*
		 * Skip PCI and PCIEX devices because they will be processed
		 * later in the picl tree walk.
		 */
		if ((strcmp(piclclass, PCIEX) == 0) ||
		    (strcmp(piclclass, PCI) == 0)) {
			err = picl_get_propval_by_name(nodeh, PICL_PROP_PEER,
			    &nodeh, sizeof (picl_nodehdl_t));
			continue;
		}

		err = picl_get_propval_by_name(nodeh, PICL_PROP_DEVFS_PATH,
		    path, sizeof (path));
		if (err != PICL_SUCCESS) {
			return (err);
		}

		(void) strlcpy(pci_card.notes, path, sizeof (pci_card.notes));

		monza_get_bus_type(parent_path, &pci_card);

		err = picl_get_propval_by_name(nodeh, PICL_PROP_NAME, &name,
		    sizeof (name));
		if (err == PICL_PROPNOTFOUND)
			(void) strcpy(name, "");
		else if (err != PICL_SUCCESS)
			return (err);

		if (strcmp(parent_path, MONZA_NIU) == 0)
			monza_get_slot_number(path, &pci_card);
		else
			monza_get_slot_number(parent_path, &pci_card);

		err = picl_get_propval_by_name(nodeh, PICL_PROP_NAME, &name,
		    sizeof (name));
		if (err == PICL_PROPNOTFOUND)
			(void) strcpy(name, "");
		else if (err != PICL_SUCCESS)
			return (err);

		/* Figure NAC name */
		if ((strcmp(name, NETWORK) == 0 ||
		    strcmp(name, ETHERNET) == 0) &&
		    (strcmp(pci_card.slot_str, MOTHERBOARD) == 0)) {
			instance = monza_get_network_instance(path);

			(void) snprintf(pci_card.status,
			    sizeof (pci_card.status), "%s/%s%d", MOTHERBOARD,
			    "NET", instance);
		} else {
			if (pci_card.slot != -1) {
				(void) snprintf(pci_card.status,
				    sizeof (pci_card.status), "%s/%s%d",
				    IOBOARD, pci_card.bus_type, pci_card.slot);
			} else {
				(void) snprintf(pci_card.status,
				    sizeof (pci_card.status),
				"%s/%s", MOTHERBOARD, pci_card.bus_type);
			}
		}

		/*
		 * Get the name of this card. If binding_name is found,
		 * name will be <nodename>-<binding_name>
		 */

		err = picl_get_propval_by_name(nodeh, PICL_PROP_BINDING_NAME,
		    &binding_name, sizeof (binding_name));
		if (err == PICL_PROPNOTFOUND) {
			/*
			 * if compatible prop is found, name will be
			 * <nodename>-<compatible>
			 */
			err = monza_get_first_compatible_value(nodeh,
			    &compatible);
			if (err == PICL_SUCCESS) {
				(void) strlcat(name, "-", MAXSTRLEN);
				(void) strlcat(name, compatible, MAXSTRLEN);
				free(compatible);
			} else if (err != PICL_PROPNOTFOUND) {
				return (err);
			}
		} else if (err != PICL_SUCCESS) {
			return (err);
		} else if (strcmp(name, binding_name) != 0) {
			(void) strlcat(name, "-", MAXSTRLEN);
			(void) strlcat(name, binding_name, MAXSTRLEN);
		}

		(void) strlcpy(pci_card.name, name, sizeof (pci_card.name));

		/* Get the model of this card */

		err = picl_get_propval_by_name(nodeh, OBP_PROP_MODEL,
		    &model, sizeof (model));
		if (err == PICL_PROPNOTFOUND)
			(void) strcpy(model, "");
		else if (err != PICL_SUCCESS)
			return (err);
		(void) strlcpy(pci_card.model, model, sizeof (pci_card.model));

		/* Print NAC name */
		log_printf("%-11s", pci_card.status);
		/* Print IO Type */
		log_printf("%6s", pci_card.bus_type);
		/* Print Slot # */
		log_printf("%5s", pci_card.slot_str);
		/* Print Parent Path */
		log_printf("%46.45s", pci_card.notes);
		/* Printf Card Name */
		if (strlen(pci_card.name) > 24)
			log_printf("%25.24s+", pci_card.name);
		else
			log_printf("%26s", pci_card.name);
		/* Print Card Model */
		if (strlen(pci_card.model) > 10)
			log_printf("%10.9s+", pci_card.model);
		else
			log_printf("%10s", pci_card.model);
		log_printf("\n");

		err = picl_get_propval_by_name(nodeh, PICL_PROP_PEER, &nodeh,
		    sizeof (picl_nodehdl_t));

	}

	return (PICL_WALK_CONTINUE);
}

/*
 * display_pci
 * Display all the PCI IO cards on this board.
 */
void
sun4v_display_pci(picl_nodehdl_t plafh)
{
	char    platbuf[MAXSTRLEN];
	char	*fmt = "%-11s %-7s %-4s %-46s %-25s %-8s";
	static int banner = FALSE; /* Have we printed the column headings? */

	if (banner == FALSE) {
		log_printf("\n", 0);
		log_printf("=========================", 0);
		log_printf(dgettext(TEXT_DOMAIN, " IO Configuration "), 0);
		log_printf("=========================", 0);
		log_printf("\n", 0);
		log_printf("\n", 0);
		log_printf(fmt, "", "IO", "", "", "", "", 0);
		log_printf("\n", 0);
		log_printf(fmt, "Location", "Type", "Slot", "Path",
		    "Name", "Model", 0);
		log_printf("\n");
		log_printf(fmt, "-----------", "-----", "----",
		    "---------------------------------------------",
		    "-------------------------", "---------", 0);
		log_printf("\n");
		banner = TRUE;
	}

	/* Get platform name, if that fails, use monza name by default */
	if (sysinfo(SI_PLATFORM, platbuf, sizeof (platbuf)) == -1) {
		(void) strcpy(platbuf, MONZA_PLATFORM);
	}

	(void) picl_walk_tree_by_class(plafh, PCIEX, PCIEX, monza_pci_callback);
	(void) picl_walk_tree_by_class(plafh, PCI, PCI, monza_pci_callback);
}

/* ARGSUSED */
void
sun4v_display_diaginfo(int flag, Prom_node *root, picl_nodehdl_t plafh)
{
	/* NOTE(ARGUNUSED(kstats)) */
	/*
	 * Now display the last powerfail time and the fatal hardware
	 * reset information. We do this under a couple of conditions.
	 * First if the user asks for it. The second is if the user
	 * told us to do logging, and we found a system failure.
	 */
	if (flag) {
		/*
		 * display time of latest powerfail. Not all systems
		 * have this capability. For those that do not, this
		 * is just a no-op.
		 */
		disp_powerfail(root);

		/* platform_disp_prom_version(tree); */
		sun4v_display_hw_revisions(root, plafh);
	}
}

/*
 * local functions
 */
/*
 * add all io devices under pci in io list
 */
/* ARGSUSED */
static int
monza_hw_rev_callback(picl_nodehdl_t pcih, void *args)
{
	int		err = PICL_SUCCESS;
	char		path[MAXSTRLEN] = "";
	char		device_path[MAXSTRLEN];
	char		NAC[MAXSTRLEN];
	char		*compatible;
	int32_t		revision;
	int		device_found;

	device_found = 0;

	err = picl_get_propval_by_name(pcih, PICL_PROP_DEVFS_PATH, path,
	    sizeof (path));
	if (err != PICL_SUCCESS) {
		return (err);
	}


	if ((strncmp(path, MONZA_NETWORK_0, strlen(MONZA_NETWORK_0)) == 0) ||
	    (strncmp(path, MONZA_NETWORK_1, strlen(MONZA_NETWORK_1)) == 0)) {
		device_found = 1;
		(void) snprintf(NAC, sizeof (NAC), "%s/%s%d",
		    MOTHERBOARD, OPHIR, 0);
		revision = monza_get_int_propval(pcih, OBP_PROP_REVISION_ID,
		    &err);
	}

	if ((strncmp(path, MONZA_NETWORK_2, strlen(MONZA_NETWORK_2)) == 0) ||
	    (strncmp(path, MONZA_NETWORK_3, strlen(MONZA_NETWORK_3)) == 0)) {
		device_found = 1;
		(void) snprintf(NAC, sizeof (NAC), "%s/%s%d",
		    MOTHERBOARD, OPHIR, 1);
		revision = monza_get_int_propval(pcih, OBP_PROP_REVISION_ID,
		    &err);
	}

	if ((strncmp(path, MONZA_ENET_2, strlen(MONZA_ENET_2)) == 0) ||
	    (strncmp(path, MONZA_ENET_3, strlen(MONZA_ENET_3)) == 0)) {
		device_found = 1;
		(void) snprintf(NAC, sizeof (NAC), "%s/%s%d",
		    MOTHERBOARD, OPHIR, 1);
		revision = monza_get_int_propval(pcih, OBP_PROP_REVISION_ID,
		    &err);
	}

	if ((strncmp(path, MONZA_NETWORK_4, strlen(MONZA_NETWORK_4)) == 0) ||
	    (strncmp(path, MONZA_NETWORK_5, strlen(MONZA_NETWORK_5)) == 0)) {
		device_found = 1;
		(void) snprintf(NAC, sizeof (NAC), "%s/%s%d",
		    MOTHERBOARD, OPHIR, 2);
		revision = monza_get_int_propval(pcih, OBP_PROP_REVISION_ID,
		    &err);
	}

	if (strcmp(path, MONZA_PCIE_SWITCH_PATH) == 0) {
		device_found = 1;
		(void) snprintf(NAC, sizeof (NAC), "%s/%s",
		    MOTHERBOARD, PCIE_SWITCH);
		revision = monza_get_int_propval(pcih,
		    OBP_PROP_REVISION_ID, &err);
	}

	if (strcmp(path, MONZA_RTM_PATH) == 0) {
		device_found = 1;
		(void) snprintf(NAC, sizeof (NAC), "%s/%s", RTM, AMC);
		revision = monza_get_int_propval(pcih, OBP_PROP_REVISION_ID,
		    &err);
	}

	if (strcmp(path, MONZA_CF_PATH) == 0) {
		device_found = 1;
		(void) snprintf(NAC, sizeof (NAC), "%s/%s", MOTHERBOARD,
		    MONZA_CF_DEVICE);
		revision = monza_get_int_propval(pcih, OBP_PROP_REVISION_ID,
		    &err);
	}

	if (device_found == 1) {
		(void) strcpy(device_path, path);
		err = monza_get_first_compatible_value(pcih, &compatible);

		/* Print NAC name */
		log_printf("%-20s", NAC);
		/* Print Device Path */
		if (strlen(device_path) > 38)
			log_printf("%38.37s+", device_path);
		else
			log_printf("%39s", device_path);

		/* Print Compatible # */
		if (err == PICL_SUCCESS) {
			log_printf("%31s", compatible);
			free(compatible);
		} else
			log_printf("%31s", " ");

		/* Print Revision */
		log_printf("%6d", revision);
		log_printf("\n");
	}

	return (PICL_WALK_CONTINUE);
}

/*ARGSUSED*/
static void
sun4v_display_hw_revisions(Prom_node *root, picl_nodehdl_t plafh)
{
	Prom_node	*pnode;
	char		*value;
	char 		platbuf[MAXSTRLEN];
	char	*fmt = "%-20s %-40s %-30s %-9s";

	log_printf(dgettext(TEXT_DOMAIN, "\n"
	    "========================= HW Revisions "
	    "=======================================\n\n"));

	log_printf(dgettext(TEXT_DOMAIN,
	    "System PROM revisions:\n"
	    "----------------------\n"));

	pnode = dev_find_node(root, "openprom");
	if (pnode != NULL) {
		value = (char *)get_prop_val(find_prop(pnode, "version"));
		log_printf(value);
	}

	log_printf(dgettext(TEXT_DOMAIN, "\n\n"
	    "IO ASIC revisions:\n"
	    "------------------\n"));
	log_printf(fmt, "Location", "Path", "Device", "Revision\n", 0);
	log_printf(fmt, "--------------------",
	    "----------------------------------------",
	    "------------------------------",
	    "---------\n", 0);

	/* Get platform name, if that fails, use monza name by default */
	if (sysinfo(SI_PLATFORM, platbuf, sizeof (platbuf)) == -1) {
		(void) strcpy(platbuf, MONZA_PLATFORM);
	}

	(void) picl_walk_tree_by_class(plafh, PCIEX,
	    PCIEX, monza_hw_rev_callback);
	(void) picl_walk_tree_by_class(plafh, PCI,
	    PCI, monza_hw_rev_callback);
	(void) picl_walk_tree_by_class(plafh, NETWORK,
	    NETWORK, monza_hw_rev_callback);
}

/*
 * return the first compatible value
 */
static int
monza_get_first_compatible_value(picl_nodehdl_t nodeh, char **outbuf)
{
	int		err;
	picl_prophdl_t	proph;
	picl_propinfo_t	pinfo;
	picl_prophdl_t	tblh;
	picl_prophdl_t	rowproph;
	char		*pval;

	err = picl_get_propinfo_by_name(nodeh, OBP_PROP_COMPATIBLE,
	    &pinfo, &proph);
	if (err != PICL_SUCCESS)
		return (err);

	if (pinfo.type == PICL_PTYPE_CHARSTRING) {
		pval = malloc(pinfo.size);
		if (pval == NULL)
			return (PICL_FAILURE);
		err = picl_get_propval(proph, pval, pinfo.size);
		if (err != PICL_SUCCESS) {
			free(pval);
			return (err);
		}
		*outbuf = pval;
		return (PICL_SUCCESS);
	}

	if (pinfo.type != PICL_PTYPE_TABLE)
		return (PICL_FAILURE);

	/* get first string from table */
	err = picl_get_propval(proph, &tblh, pinfo.size);
	if (err != PICL_SUCCESS)
		return (err);

	err = picl_get_next_by_row(tblh, &rowproph);
	if (err != PICL_SUCCESS)
		return (err);

	err = picl_get_propinfo(rowproph, &pinfo);
	if (err != PICL_SUCCESS)
		return (err);

	pval = malloc(pinfo.size);
	if (pval == NULL)
		return (PICL_FAILURE);

	err = picl_get_propval(rowproph, pval, pinfo.size);
	if (err != PICL_SUCCESS) {
		free(pval);
		return (err);
	}

	*outbuf = pval;
	return (PICL_SUCCESS);
}

static int64_t
monza_get_int_propval(picl_nodehdl_t modh, char *prop_name, int *ret)
{
	int		err;
	picl_prophdl_t	proph;
	picl_propinfo_t	pinfo;
	int8_t		int8v;
	int16_t		int16v;
	int32_t		int32v;
	int64_t		int64v;

	err = picl_get_propinfo_by_name(modh, prop_name, &pinfo, &proph);
	if (err != PICL_SUCCESS) {
		*ret = err;
		return (0);
	}

	/*
	 * If it is not an int, uint or byte array prop, return failure
	 */
	if ((pinfo.type != PICL_PTYPE_INT) &&
	    (pinfo.type != PICL_PTYPE_UNSIGNED_INT) &&
	    (pinfo.type != PICL_PTYPE_BYTEARRAY)) {
		*ret = PICL_FAILURE;
		return (0);
	}

	switch (pinfo.size) {
	case sizeof (int8_t):
		err = picl_get_propval(proph, &int8v, sizeof (int8v));
		*ret = err;
		return (int8v);
	case sizeof (int16_t):
		err = picl_get_propval(proph, &int16v, sizeof (int16v));
		*ret = err;
		return (int16v);
	case sizeof (int32_t):
		err = picl_get_propval(proph, &int32v, sizeof (int32v));
		*ret = err;
		return (int32v);
	case sizeof (int64_t):
		err = picl_get_propval(proph, &int64v, sizeof (int64v));
		*ret = err;
		return (int64v);
	default:	/* not supported size */
		*ret = PICL_FAILURE;
		return (0);
	}
}

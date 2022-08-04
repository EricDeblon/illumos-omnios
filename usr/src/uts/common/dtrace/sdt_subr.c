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
 * Copyright (c) 2004, 2010, Oracle and/or its affiliates. All rights reserved.
 * Copyright (c) 2012, Joyent, Inc. All rights reserved.
 * Copyright 2018 Nexenta Systems, Inc.  All rights reserved.
 */

#include <sys/sdt_impl.h>

static dtrace_pattr_t vtrace_attr = {
{ DTRACE_STABILITY_UNSTABLE, DTRACE_STABILITY_UNSTABLE, DTRACE_CLASS_ISA },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_UNKNOWN },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_UNKNOWN },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_UNKNOWN },
{ DTRACE_STABILITY_UNSTABLE, DTRACE_STABILITY_UNSTABLE, DTRACE_CLASS_ISA },
};

static dtrace_pattr_t info_attr = {
{ DTRACE_STABILITY_EVOLVING, DTRACE_STABILITY_EVOLVING, DTRACE_CLASS_ISA },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_UNKNOWN },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_UNKNOWN },
{ DTRACE_STABILITY_EVOLVING, DTRACE_STABILITY_EVOLVING, DTRACE_CLASS_ISA },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_ISA },
};

static dtrace_pattr_t fc_attr = {
{ DTRACE_STABILITY_EVOLVING, DTRACE_STABILITY_EVOLVING, DTRACE_CLASS_ISA },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_UNKNOWN },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_UNKNOWN },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_ISA },
{ DTRACE_STABILITY_EVOLVING, DTRACE_STABILITY_EVOLVING, DTRACE_CLASS_ISA },
};

static dtrace_pattr_t fpu_attr = {
{ DTRACE_STABILITY_EVOLVING, DTRACE_STABILITY_EVOLVING, DTRACE_CLASS_ISA },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_UNKNOWN },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_UNKNOWN },
{ DTRACE_STABILITY_EVOLVING, DTRACE_STABILITY_EVOLVING, DTRACE_CLASS_CPU },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_ISA },
};

static dtrace_pattr_t fsinfo_attr = {
{ DTRACE_STABILITY_EVOLVING, DTRACE_STABILITY_EVOLVING, DTRACE_CLASS_ISA },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_UNKNOWN },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_UNKNOWN },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_UNKNOWN },
{ DTRACE_STABILITY_EVOLVING, DTRACE_STABILITY_EVOLVING, DTRACE_CLASS_ISA },
};

static dtrace_pattr_t stab_attr = {
{ DTRACE_STABILITY_EVOLVING, DTRACE_STABILITY_EVOLVING, DTRACE_CLASS_ISA },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_UNKNOWN },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_UNKNOWN },
{ DTRACE_STABILITY_EVOLVING, DTRACE_STABILITY_EVOLVING, DTRACE_CLASS_ISA },
{ DTRACE_STABILITY_EVOLVING, DTRACE_STABILITY_EVOLVING, DTRACE_CLASS_ISA },
};

static dtrace_pattr_t sdt_attr = {
{ DTRACE_STABILITY_EVOLVING, DTRACE_STABILITY_EVOLVING, DTRACE_CLASS_ISA },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_UNKNOWN },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_UNKNOWN },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_ISA },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_ISA },
};

static dtrace_pattr_t xpv_attr = {
{ DTRACE_STABILITY_EVOLVING, DTRACE_STABILITY_EVOLVING, DTRACE_CLASS_PLATFORM },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_UNKNOWN },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_UNKNOWN },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_PLATFORM },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_PLATFORM },
};

static dtrace_pattr_t iscsi_attr = {
{ DTRACE_STABILITY_EVOLVING, DTRACE_STABILITY_EVOLVING, DTRACE_CLASS_ISA },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_UNKNOWN },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_UNKNOWN },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_ISA },
{ DTRACE_STABILITY_EVOLVING, DTRACE_STABILITY_EVOLVING, DTRACE_CLASS_ISA },
};

static dtrace_pattr_t nfs_attr = {
{ DTRACE_STABILITY_EVOLVING, DTRACE_STABILITY_EVOLVING, DTRACE_CLASS_ISA },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_UNKNOWN },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_UNKNOWN },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_ISA },
{ DTRACE_STABILITY_EVOLVING, DTRACE_STABILITY_EVOLVING, DTRACE_CLASS_ISA },
};

static dtrace_pattr_t smb_attr = {
{ DTRACE_STABILITY_EVOLVING, DTRACE_STABILITY_EVOLVING, DTRACE_CLASS_ISA },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_UNKNOWN },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_UNKNOWN },
{ DTRACE_STABILITY_PRIVATE, DTRACE_STABILITY_PRIVATE, DTRACE_CLASS_ISA },
{ DTRACE_STABILITY_EVOLVING, DTRACE_STABILITY_EVOLVING, DTRACE_CLASS_ISA },
};

sdt_provider_t sdt_providers[] = {
	{ "vtrace", "__vtrace_", &vtrace_attr },
	{ "sysinfo", "__cpu_sysinfo_", &info_attr, DTRACE_PRIV_USER },
	{ "vminfo", "__cpu_vminfo_", &info_attr, DTRACE_PRIV_USER },
	{ "fpuinfo", "__fpuinfo_", &fpu_attr },
	{ "sched", "__sched_", &stab_attr, DTRACE_PRIV_USER },
	{ "proc", "__proc_", &stab_attr, DTRACE_PRIV_USER },
	{ "io", "__io_", &stab_attr },
	{ "ip", "__ip_", &stab_attr },
	{ "tcp", "__tcp_", &stab_attr },
	{ "udp", "__udp_", &stab_attr },
	{ "mib", "__mib_", &stab_attr },
	{ "fsinfo", "__fsinfo_", &fsinfo_attr },
	{ "iscsi", "__iscsi_", &iscsi_attr },
	{ "nfsv3", "__nfsv3_", &nfs_attr },
	{ "nfsv4", "__nfsv4_", &nfs_attr },
	{ "smb", "__smb_", &smb_attr },
	{ "smb2", "__smb2_", &smb_attr },
	{ "xpv", "__xpv_", &xpv_attr },
	{ "fc", "__fc_", &fc_attr },
	{ "srp", "__srp_", &fc_attr },
	{ "sysevent", "__sysevent_", &stab_attr },
	{ "sdt", NULL, &sdt_attr },
	{ NULL }
};

sdt_argdesc_t sdt_args[] = {
	{ "sched", "wakeup", 0, 0, "kthread_t *", "lwpsinfo_t *" },
	{ "sched", "wakeup", 1, 0, "kthread_t *", "psinfo_t *" },
	{ "sched", "dequeue", 0, 0, "kthread_t *", "lwpsinfo_t *" },
	{ "sched", "dequeue", 1, 0, "kthread_t *", "psinfo_t *" },
	{ "sched", "dequeue", 2, 1, "disp_t *", "cpuinfo_t *" },
	{ "sched", "enqueue", 0, 0, "kthread_t *", "lwpsinfo_t *" },
	{ "sched", "enqueue", 1, 0, "kthread_t *", "psinfo_t *" },
	{ "sched", "enqueue", 2, 1, "disp_t *", "cpuinfo_t *" },
	{ "sched", "enqueue", 3, 2, "int" },
	{ "sched", "off-cpu", 0, 0, "kthread_t *", "lwpsinfo_t *" },
	{ "sched", "off-cpu", 1, 0, "kthread_t *", "psinfo_t *" },
	{ "sched", "tick", 0, 0, "kthread_t *", "lwpsinfo_t *" },
	{ "sched", "tick", 1, 0, "kthread_t *", "psinfo_t *" },
	{ "sched", "change-pri", 0, 0, "kthread_t *", "lwpsinfo_t *" },
	{ "sched", "change-pri", 1, 0, "kthread_t *", "psinfo_t *" },
	{ "sched", "change-pri", 2, 1, "pri_t" },
	{ "sched", "schedctl-nopreempt", 0, 0, "kthread_t *", "lwpsinfo_t *" },
	{ "sched", "schedctl-nopreempt", 1, 0, "kthread_t *", "psinfo_t *" },
	{ "sched", "schedctl-nopreempt", 2, 1, "int" },
	{ "sched", "schedctl-preempt", 0, 0, "kthread_t *", "lwpsinfo_t *" },
	{ "sched", "schedctl-preempt", 1, 0, "kthread_t *", "psinfo_t *" },
	{ "sched", "schedctl-failsafe", 0, 0, "kthread_t *", "lwpsinfo_t *" },
	{ "sched", "schedctl-failsafe", 1, 0, "kthread_t *", "psinfo_t *" },
	{ "sched", "schedctl-yield", 0, 0, "int" },
	{ "sched", "surrender", 0, 0, "kthread_t *", "lwpsinfo_t *" },
	{ "sched", "surrender", 1, 0, "kthread_t *", "psinfo_t *" },
	{ "sched", "swapin-lwp", 0, 0, "kthread_t *", "lwpsinfo_t *" },
	{ "sched", "swapin-lwp", 1, 0, "kthread_t *", "psinfo_t *" },
	{ "sched", "swapout-lwp", 0, 0, "kthread_t *", "lwpsinfo_t *" },
	{ "sched", "swapout-lwp", 1, 0, "kthread_t *", "psinfo_t *" },
	{ "sched", "swapout-process", 0, 0, "proc_t *", "psinfo_t *" },
	{ "sched", "cpucaps-sleep", 0, 0, "kthread_t *", "lwpsinfo_t *" },
	{ "sched", "cpucaps-sleep", 1, 0, "kthread_t *", "psinfo_t *" },
	{ "sched", "cpucaps-wakeup", 0, 0, "kthread_t *", "lwpsinfo_t *" },
	{ "sched", "cpucaps-wakeup", 1, 0, "kthread_t *", "psinfo_t *" },

	{ "proc", "create", 0, 0, "proc_t *", "psinfo_t *" },
	{ "proc", "exec", 0, 0, "string" },
	{ "proc", "exec-failure", 0, 0, "int" },
	{ "proc", "exit", 0, 0, "int" },
	{ "proc", "fault", 0, 0, "int" },
	{ "proc", "fault", 1, 1, "siginfo_t *" },
	{ "proc", "lwp-create", 0, 0, "kthread_t *", "lwpsinfo_t *" },
	{ "proc", "lwp-create", 1, 0, "kthread_t *", "psinfo_t *" },
	{ "proc", "signal-clear", 0, 0, "int" },
	{ "proc", "signal-clear", 1, 1, "siginfo_t *" },
	{ "proc", "signal-discard", 0, 0, "kthread_t *", "lwpsinfo_t *" },
	{ "proc", "signal-discard", 1, 1, "proc_t *", "psinfo_t *" },
	{ "proc", "signal-discard", 2, 2, "int" },
	{ "proc", "signal-handle", 0, 0, "int" },
	{ "proc", "signal-handle", 1, 1, "siginfo_t *" },
	{ "proc", "signal-handle", 2, 2, "void (*)(void)" },
	{ "proc", "signal-send", 0, 0, "kthread_t *", "lwpsinfo_t *" },
	{ "proc", "signal-send", 1, 0, "kthread_t *", "psinfo_t *" },
	{ "proc", "signal-send", 2, 1, "int" },

	{ "io", "start", 0, 0, "buf_t *", "bufinfo_t *" },
	{ "io", "start", 1, 0, "buf_t *", "devinfo_t *" },
	{ "io", "start", 2, 0, "buf_t *", "fileinfo_t *" },
	{ "io", "done", 0, 0, "buf_t *", "bufinfo_t *" },
	{ "io", "done", 1, 0, "buf_t *", "devinfo_t *" },
	{ "io", "done", 2, 0, "buf_t *", "fileinfo_t *" },
	{ "io", "wait-start", 0, 0, "buf_t *", "bufinfo_t *" },
	{ "io", "wait-start", 1, 0, "buf_t *", "devinfo_t *" },
	{ "io", "wait-start", 2, 0, "buf_t *", "fileinfo_t *" },
	{ "io", "wait-done", 0, 0, "buf_t *", "bufinfo_t *" },
	{ "io", "wait-done", 1, 0, "buf_t *", "devinfo_t *" },
	{ "io", "wait-done", 2, 0, "buf_t *", "fileinfo_t *" },

	{ "mib", NULL, 0, 0, "int" },

	{ "fsinfo", NULL, 0, 0, "vnode_t *", "fileinfo_t *" },
	{ "fsinfo", NULL, 1, 1, "int", "int" },

	{ "iscsi", "async-send", 0, 0, "idm_conn_t *", "conninfo_t *" },
	{ "iscsi", "async-send", 1, 1, "iscsi_async_evt_hdr_t *",
	    "iscsiinfo_t *" },
	{ "iscsi", "login-command", 0, 0, "idm_conn_t *", "conninfo_t *" },
	{ "iscsi", "login-command", 1, 1, "iscsi_login_hdr_t *",
	    "iscsiinfo_t *" },
	{ "iscsi", "login-response", 0, 0, "idm_conn_t *", "conninfo_t *" },
	{ "iscsi", "login-response", 1, 1, "iscsi_login_rsp_hdr_t *",
	    "iscsiinfo_t *" },
	{ "iscsi", "logout-command", 0, 0, "idm_conn_t *", "conninfo_t *" },
	{ "iscsi", "logout-command", 1, 1, "iscsi_logout_hdr_t *",
	    "iscsiinfo_t *" },
	{ "iscsi", "logout-response", 0, 0, "idm_conn_t *", "conninfo_t *" },
	{ "iscsi", "logout-response", 1, 1, "iscsi_logout_rsp_hdr_t *",
	    "iscsiinfo_t *" },
	{ "iscsi", "data-request", 0, 0, "idm_conn_t *", "conninfo_t *" },
	{ "iscsi", "data-request", 1, 1, "iscsi_rtt_hdr_t *",
	    "iscsiinfo_t *" },
	{ "iscsi", "data-send", 0, 0, "idm_conn_t *", "conninfo_t *" },
	{ "iscsi", "data-send", 1, 1, "iscsi_data_rsp_hdr_t *",
	    "iscsiinfo_t *" },
	{ "iscsi", "data-receive", 0, 0, "idm_conn_t *", "conninfo_t *" },
	{ "iscsi", "data-receive", 1, 1, "iscsi_data_hdr_t *",
	    "iscsiinfo_t *" },
	{ "iscsi", "nop-send", 0, 0, "idm_conn_t *", "conninfo_t *" },
	{ "iscsi", "nop-send", 1, 1, "iscsi_nop_in_hdr_t *", "iscsiinfo_t *" },
	{ "iscsi", "nop-receive", 0, 0, "idm_conn_t *", "conninfo_t *" },
	{ "iscsi", "nop-receive", 1, 1, "iscsi_nop_out_hdr_t *",
	    "iscsiinfo_t *" },
	{ "iscsi", "scsi-command", 0, 0, "idm_conn_t *", "conninfo_t *" },
	{ "iscsi", "scsi-command", 1, 1, "iscsi_scsi_cmd_hdr_t *",
	    "iscsiinfo_t *" },
	{ "iscsi", "scsi-command", 2, 2, "scsi_task_t *", "scsicmd_t *" },
	{ "iscsi", "scsi-response", 0, 0, "idm_conn_t *", "conninfo_t *" },
	{ "iscsi", "scsi-response", 1, 1, "iscsi_scsi_rsp_hdr_t *",
	    "iscsiinfo_t *" },
	{ "iscsi", "task-command", 0, 0, "idm_conn_t *", "conninfo_t *" },
	{ "iscsi", "task-command", 1, 1, "iscsi_scsi_task_mgt_hdr_t *",
	    "iscsiinfo_t *" },
	{ "iscsi", "task-response", 0, 0, "idm_conn_t *", "conninfo_t *" },
	{ "iscsi", "task-response", 1, 1, "iscsi_scsi_task_mgt_rsp_hdr_t *",
	    "iscsiinfo_t *" },
	{ "iscsi", "text-command", 0, 0, "idm_conn_t *", "conninfo_t *" },
	{ "iscsi", "text-command", 1, 1, "iscsi_text_hdr_t *",
	    "iscsiinfo_t *" },
	{ "iscsi", "text-response", 0, 0, "idm_conn_t *", "conninfo_t *" },
	{ "iscsi", "text-response", 1, 1, "iscsi_text_rsp_hdr_t *",
	    "iscsiinfo_t *" },
	{ "iscsi", "xfer-start", 0, 0, "idm_conn_t *", "conninfo_t *" },
	{ "iscsi", "xfer-start", 1, 0, "idm_conn_t *", "iscsiinfo_t *" },
	{ "iscsi", "xfer-start", 2, 1, "uintptr_t", "xferinfo_t *" },
	{ "iscsi", "xfer-start", 3, 2, "uint32_t"},
	{ "iscsi", "xfer-start", 4, 3, "uintptr_t"},
	{ "iscsi", "xfer-start", 5, 4, "uint32_t"},
	{ "iscsi", "xfer-start", 6, 5, "uint32_t"},
	{ "iscsi", "xfer-start", 7, 6, "uint32_t"},
	{ "iscsi", "xfer-start", 8, 7, "int"},
	{ "iscsi", "xfer-done", 0, 0, "idm_conn_t *", "conninfo_t *" },
	{ "iscsi", "xfer-done", 1, 0, "idm_conn_t *", "iscsiinfo_t *" },
	{ "iscsi", "xfer-done", 2, 1, "uintptr_t", "xferinfo_t *" },
	{ "iscsi", "xfer-done", 3, 2, "uint32_t"},
	{ "iscsi", "xfer-done", 4, 3, "uintptr_t"},
	{ "iscsi", "xfer-done", 5, 4, "uint32_t"},
	{ "iscsi", "xfer-done", 6, 5, "uint32_t"},
	{ "iscsi", "xfer-done", 7, 6, "uint32_t"},
	{ "iscsi", "xfer-done", 8, 7, "int"},

	/* Tables like this get really ugly when line-wrapped. */
	/* BEGIN CSTYLED */
	{ "nfsv3", "op-getattr-start", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-getattr-start", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-getattr-start", 2, 4, "GETATTR3args *" },

	{ "nfsv3", "op-getattr-done", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-getattr-done", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-getattr-done", 2, 4, "GETATTR3res *" },

	{ "nfsv3", "op-setattr-start", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-setattr-start", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-setattr-start", 2, 4, "SETATTR3args *" },

	{ "nfsv3", "op-setattr-done", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-setattr-done", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-setattr-done", 2, 4, "SETATTR3res *" },

	{ "nfsv3", "op-lookup-start", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-lookup-start", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-lookup-start", 2, 4, "LOOKUP3args *" },

	{ "nfsv3", "op-lookup-done", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-lookup-done", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-lookup-done", 2, 4, "LOOKUP3res *" },

	{ "nfsv3", "op-access-start", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-access-start", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-access-start", 2, 4, "ACCESS3args *" },

	{ "nfsv3", "op-access-done", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-access-done", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-access-done", 2, 4, "ACCESS3res *" },

	{ "nfsv3", "op-commit-start", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-commit-start", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-commit-start", 2, 4, "COMMIT3args *" },

	{ "nfsv3", "op-commit-done", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-commit-done", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-commit-done", 2, 4, "COMMIT3res *" },

	{ "nfsv3", "op-create-start", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-create-start", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-create-start", 2, 4, "CREATE3args *" },

	{ "nfsv3", "op-create-done", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-create-done", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-create-done", 2, 4, "CREATE3res *" },

	{ "nfsv3", "op-fsinfo-start", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-fsinfo-start", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-fsinfo-start", 2, 4, "FSINFO3args *" },

	{ "nfsv3", "op-fsinfo-done", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-fsinfo-done", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-fsinfo-done", 2, 4, "FSINFO3res *" },

	{ "nfsv3", "op-fsstat-start", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-fsstat-start", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-fsstat-start", 2, 4, "FSSTAT3args *" },

	{ "nfsv3", "op-fsstat-done", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-fsstat-done", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-fsstat-done", 2, 4, "FSSTAT3res *" },

	{ "nfsv3", "op-link-start", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-link-start", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-link-start", 2, 4, "LINK3args *" },

	{ "nfsv3", "op-link-done", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-link-done", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-link-done", 2, 4, "LINK3res *" },

	{ "nfsv3", "op-mkdir-start", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-mkdir-start", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-mkdir-start", 2, 4, "MKDIR3args *" },

	{ "nfsv3", "op-mkdir-done", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-mkdir-done", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-mkdir-done", 2, 4, "MKDIR3res *" },

	{ "nfsv3", "op-mknod-start", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-mknod-start", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-mknod-start", 2, 4, "MKNOD3args *" },

	{ "nfsv3", "op-mknod-done", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-mknod-done", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-mknod-done", 2, 4, "MKNOD3res *" },

	{ "nfsv3", "op-null-start", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-null-start", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },

	{ "nfsv3", "op-null-done", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-null-done", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },

	{ "nfsv3", "op-pathconf-start", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-pathconf-start", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-pathconf-start", 2, 4, "PATHCONF3args *" },

	{ "nfsv3", "op-pathconf-done", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-pathconf-done", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-pathconf-done", 2, 4, "PATHCONF3res *" },

	{ "nfsv3", "op-read-start", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-read-start", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-read-start", 2, 4, "READ3args *" },

	{ "nfsv3", "op-read-done", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-read-done", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-read-done", 2, 4, "READ3res *" },

	{ "nfsv3", "op-readdir-start", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-readdir-start", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-readdir-start", 2, 4, "READDIR3args *" },

	{ "nfsv3", "op-readdir-done", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-readdir-done", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-readdir-done", 2, 4, "READDIR3res *" },

	{ "nfsv3", "op-readdirplus-start", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-readdirplus-start", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-readdirplus-start", 2, 4, "READDIRPLUS3args *" },

	{ "nfsv3", "op-readdirplus-done", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-readdirplus-done", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-readdirplus-done", 2, 4, "READDIRPLUS3res *" },

	{ "nfsv3", "op-readlink-start", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-readlink-start", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-readlink-start", 2, 4, "READLINK3args *" },

	{ "nfsv3", "op-readlink-done", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-readlink-done", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-readlink-done", 2, 4, "READLINK3res *" },

	{ "nfsv3", "op-remove-start", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-remove-start", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-remove-start", 2, 4, "REMOVE3args *" },

	{ "nfsv3", "op-remove-done", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-remove-done", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-remove-done", 2, 4, "REMOVE3res *" },

	{ "nfsv3", "op-rename-start", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-rename-start", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-rename-start", 2, 4, "RENAME3args *" },

	{ "nfsv3", "op-rename-done", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-rename-done", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-rename-done", 2, 4, "RENAME3res *" },

	{ "nfsv3", "op-rmdir-start", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-rmdir-start", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-rmdir-start", 2, 4, "RMDIR3args *" },

	{ "nfsv3", "op-rmdir-done", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-rmdir-done", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-rmdir-done", 2, 4, "RMDIR3res *" },

	{ "nfsv3", "op-symlink-start", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-symlink-start", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-symlink-start", 2, 4, "SYMLINK3args *" },

	{ "nfsv3", "op-symlink-done", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-symlink-done", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-symlink-done", 2, 4, "SYMLINK3res *" },

	{ "nfsv3", "op-write-start", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-write-start", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-write-start", 2, 4, "WRITE3args *" },

	{ "nfsv3", "op-write-done", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv3", "op-write-done", 1, 1, "nfsv3oparg_t *", "nfsv3opinfo_t *" },
	{ "nfsv3", "op-write-done", 2, 4, "WRITE3res *" },

	{ "nfsv4", "null-start", 0, 0, "struct svc_req *", "conninfo_t *" },
	{ "nfsv4", "null-done", 0, 0, "struct svc_req *", "conninfo_t *" },

	{ "nfsv4", "compound-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "compound-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "compound-start", 2, 1, "COMPOUND4args *" },

	{ "nfsv4", "compound-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "compound-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "compound-done", 2, 1, "COMPOUND4res *" },

	{ "nfsv4", "op-access-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-access-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-access-start", 2, 1, "ACCESS4args *" },

	{ "nfsv4", "op-access-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-access-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-access-done", 2, 1, "ACCESS4res *" },

	{ "nfsv4", "op-close-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-close-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-close-start", 2, 1, "CLOSE4args *" },

	{ "nfsv4", "op-close-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-close-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-close-done", 2, 1, "CLOSE4res *" },

	{ "nfsv4", "op-commit-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-commit-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-commit-start", 2, 1, "COMMIT4args *" },

	{ "nfsv4", "op-commit-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-commit-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-commit-done", 2, 1, "COMMIT4res *" },

	{ "nfsv4", "op-create-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-create-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-create-start", 2, 1, "CREATE4args *" },

	{ "nfsv4", "op-create-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-create-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-create-done", 2, 1, "CREATE4res *" },

	{ "nfsv4", "op-delegpurge-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-delegpurge-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-delegpurge-start", 2, 1, "DELEGPURGE4args *" },

	{ "nfsv4", "op-delegpurge-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-delegpurge-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-delegpurge-done", 2, 1, "DELEGPURGE4res *" },

	{ "nfsv4", "op-delegreturn-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-delegreturn-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-delegreturn-start", 2, 1, "DELEGRETURN4args *" },

	{ "nfsv4", "op-delegreturn-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-delegreturn-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-delegreturn-done", 2, 1, "DELEGRETURN4res *" },

	{ "nfsv4", "op-getattr-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-getattr-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-getattr-start", 2, 1, "GETATTR4args *" },

	{ "nfsv4", "op-getattr-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-getattr-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-getattr-done", 2, 1, "GETATTR4res *" },

	{ "nfsv4", "op-getfh-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-getfh-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },

	{ "nfsv4", "op-getfh-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-getfh-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-getfh-done", 2, 1, "GETFH4res *" },

	{ "nfsv4", "op-link-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-link-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-link-start", 2, 1, "LINK4args *" },

	{ "nfsv4", "op-link-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-link-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-link-done", 2, 1, "LINK4res *" },

	{ "nfsv4", "op-lock-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-lock-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-lock-start", 2, 1, "LOCK4args *" },

	{ "nfsv4", "op-lock-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-lock-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-lock-done", 2, 1, "LOCK4res *" },

	{ "nfsv4", "op-lockt-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-lockt-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-lockt-start", 2, 1, "LOCKT4args *" },

	{ "nfsv4", "op-lockt-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-lockt-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-lockt-done", 2, 1, "LOCKT4res *" },

	{ "nfsv4", "op-locku-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-locku-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-locku-start", 2, 1, "LOCKU4args *" },

	{ "nfsv4", "op-locku-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-locku-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-locku-done", 2, 1, "LOCKU4res *" },

	{ "nfsv4", "op-lookup-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-lookup-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-lookup-start", 2, 1, "LOOKUP4args *" },

	{ "nfsv4", "op-lookup-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-lookup-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-lookup-done", 2, 1, "LOOKUP4res *" },

	{ "nfsv4", "op-lookupp-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-lookupp-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },

	{ "nfsv4", "op-lookupp-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-lookupp-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-lookupp-done", 2, 1, "LOOKUPP4res *" },

	{ "nfsv4", "op-nverify-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-nverify-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-nverify-start", 2, 1, "NVERIFY4args *" },

	{ "nfsv4", "op-nverify-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-nverify-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-nverify-done", 2, 1, "NVERIFY4res *" },

	{ "nfsv4", "op-open-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-open-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-open-start", 2, 1, "OPEN4args *" },

	{ "nfsv4", "op-open-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-open-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-open-done", 2, 1, "OPEN4res *" },

	{ "nfsv4", "op-open-confirm-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-open-confirm-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-open-confirm-start", 2, 1, "OPEN_CONFIRM4args *" },

	{ "nfsv4", "op-open-confirm-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-open-confirm-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-open-confirm-done", 2, 1, "OPEN_CONFIRM4res *" },

	{ "nfsv4", "op-open-downgrade-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-open-downgrade-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-open-downgrade-start", 2, 1, "OPEN_DOWNGRADE4args *" },

	{ "nfsv4", "op-open-downgrade-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-open-downgrade-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-open-downgrade-done", 2, 1, "OPEN_DOWNGRADE4res *" },

	{ "nfsv4", "op-openattr-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-openattr-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-openattr-start", 2, 1, "OPENATTR4args *" },

	{ "nfsv4", "op-openattr-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-openattr-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-openattr-done", 2, 1, "OPENATTR4res *" },

	{ "nfsv4", "op-putfh-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-putfh-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-putfh-start", 2, 1, "PUTFH4args *" },

	{ "nfsv4", "op-putfh-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-putfh-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-putfh-done", 2, 1, "PUTFH4res *" },

	{ "nfsv4", "op-putpubfh-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-putpubfh-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },

	{ "nfsv4", "op-putpubfh-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-putpubfh-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-putpubfh-done", 2, 1, "PUTPUBFH4res *" },

	{ "nfsv4", "op-putrootfh-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-putrootfh-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },

	{ "nfsv4", "op-putrootfh-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-putrootfh-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-putrootfh-done", 2, 1, "PUTROOTFH4res *" },

	{ "nfsv4", "op-read-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-read-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-read-start", 2, 1, "READ4args *" },

	{ "nfsv4", "op-read-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-read-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-read-done", 2, 1, "READ4res *" },

	{ "nfsv4", "op-readdir-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-readdir-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-readdir-start", 2, 1, "READDIR4args *" },

	{ "nfsv4", "op-readdir-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-readdir-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-readdir-done", 2, 1, "READDIR4res *" },

	{ "nfsv4", "op-readlink-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-readlink-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },

	{ "nfsv4", "op-readlink-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-readlink-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-readlink-done", 2, 1, "READLINK4res *" },

	{ "nfsv4", "op-release-lockowner-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-release-lockowner-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-release-lockowner-start", 2, 1, "RELEASE_LOCKOWNER4args *" },

	{ "nfsv4", "op-release-lockowner-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-release-lockowner-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-release-lockowner-done", 2, 1, "RELEASE_LOCKOWNER4res *" },

	{ "nfsv4", "op-remove-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-remove-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-remove-start", 2, 1, "REMOVE4args *" },

	{ "nfsv4", "op-remove-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-remove-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-remove-done", 2, 1, "REMOVE4res *" },

	{ "nfsv4", "op-rename-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-rename-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-rename-start", 2, 1, "RENAME4args *" },

	{ "nfsv4", "op-rename-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-rename-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-rename-done", 2, 1, "RENAME4res *" },

	{ "nfsv4", "op-renew-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-renew-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-renew-start", 2, 1, "RENEW4args *" },

	{ "nfsv4", "op-renew-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-renew-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-renew-done", 2, 1, "RENEW4res *" },

	{ "nfsv4", "op-restorefh-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-restorefh-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },

	{ "nfsv4", "op-restorefh-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-restorefh-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-restorefh-done", 2, 1, "RESTOREFH4res *" },

	{ "nfsv4", "op-savefh-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-savefh-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },

	{ "nfsv4", "op-savefh-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-savefh-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-savefh-done", 2, 1, "SAVEFH4res *" },

	{ "nfsv4", "op-secinfo-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-secinfo-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-secinfo-start", 2, 1, "SECINFO4args *" },

	{ "nfsv4", "op-secinfo-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-secinfo-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-secinfo-done", 2, 1, "SECINFO4res *" },

	{ "nfsv4", "op-setattr-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-setattr-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-setattr-start", 2, 1, "SETATTR4args *" },

	{ "nfsv4", "op-setattr-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-setattr-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-setattr-done", 2, 1, "SETATTR4res *" },

	{ "nfsv4", "op-setclientid-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-setclientid-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-setclientid-start", 2, 1, "SETCLIENTID4args *" },

	{ "nfsv4", "op-setclientid-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-setclientid-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-setclientid-done", 2, 1, "SETCLIENTID4res *" },

	{ "nfsv4", "op-setclientid-confirm-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-setclientid-confirm-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-setclientid-confirm-start", 2, 1, "SETCLIENTID_CONFIRM4args *" },

	{ "nfsv4", "op-setclientid-confirm-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-setclientid-confirm-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-setclientid-confirm-done", 2, 1, "SETCLIENTID_CONFIRM4res *" },

	{ "nfsv4", "op-verify-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-verify-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-verify-start", 2, 1, "VERIFY4args *" },

	{ "nfsv4", "op-verify-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-verify-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-verify-done", 2, 1, "VERIFY4res *" },

	{ "nfsv4", "op-write-start", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-write-start", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-write-start", 2, 1, "WRITE4args *" },

	{ "nfsv4", "op-write-done", 0, 0, "struct compound_state *", "conninfo_t *" },
	{ "nfsv4", "op-write-done", 1, 0, "struct compound_state *", "nfsv4opinfo_t *" },
	{ "nfsv4", "op-write-done", 2, 1, "WRITE4res *" },

	{ "nfsv4", "cb-recall-start", 0, 0, "rfs4_client_t *", "conninfo_t *" },
	{ "nfsv4", "cb-recall-start", 1, 1, "rfs4_deleg_state_t *", "nfsv4cbinfo_t *" },
	{ "nfsv4", "cb-recall-start", 2, 2, "CB_RECALL4args *" },

	{ "nfsv4", "cb-recall-done", 0, 0, "rfs4_client_t *", "conninfo_t *" },
	{ "nfsv4", "cb-recall-done", 1, 1, "rfs4_deleg_state_t *", "nfsv4cbinfo_t *" },
	{ "nfsv4", "cb-recall-done", 2, 2, "CB_RECALL4res *" },

	{ "smb", "op-Close-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Close-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-Close-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Close-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-CloseAndTreeDisconnect-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-CloseAndTreeDisconnect-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-CloseAndTreeDisconnect-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-CloseAndTreeDisconnect-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-Transaction-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Transaction-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-Transaction-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Transaction-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-TransactionSecondary-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-TransactionSecondary-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-TransactionSecondary-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-TransactionSecondary-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-Ioctl-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Ioctl-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-Ioctl-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Ioctl-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-Transaction2-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Transaction2-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-Transaction2-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Transaction2-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-Transaction2Secondary-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Transaction2Secondary-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-Transaction2Secondary-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Transaction2Secondary-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-NtTransact-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-NtTransact-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-NtTransact-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-NtTransact-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-NtTransactSecondary-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-NtTransactSecondary-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-NtTransactSecondary-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-NtTransactSecondary-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-Create-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Create-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-Create-start", 2, 0, "smb_request_t *", "smb_open_args_t *" },
	{ "smb", "op-Create-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Create-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-CreateNew-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-CreateNew-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-CreateNew-start", 2, 0, "smb_request_t *", "smb_open_args_t *" },
	{ "smb", "op-CreateNew-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-CreateNew-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-CreateTemporary-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-CreateTemporary-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-CreateTemporary-start", 2, 0, "smb_request_t *", "smb_open_args_t *" },
	{ "smb", "op-CreateTemporary-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-CreateTemporary-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-Delete-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Delete-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-Delete-start", 2, 0, "smb_request_t *", "smb_name_args_t *" },
	{ "smb", "op-Delete-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Delete-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-CreateDirectory-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-CreateDirectory-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-CreateDirectory-start", 2, 0, "smb_request_t *", "smb_name_args_t *" },
	{ "smb", "op-CreateDirectory-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-CreateDirectory-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-DeleteDirectory-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-DeleteDirectory-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-DeleteDirectory-start", 2, 0, "smb_request_t *", "smb_name_args_t *" },
	{ "smb", "op-DeleteDirectory-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-DeleteDirectory-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-CheckDirectory-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-CheckDirectory-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-CheckDirectory-start", 2, 0, "smb_request_t *", "smb_name_args_t *" },
	{ "smb", "op-CheckDirectory-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-CheckDirectory-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-Invalid-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Invalid-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-Invalid-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Invalid-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-Echo-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Echo-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-Echo-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Echo-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-Search-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Search-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-Search-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Search-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-Find-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Find-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-Find-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Find-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-FindClose-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-FindClose-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-FindClose-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-FindClose-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-FindUnique-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-FindUnique-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-FindUnique-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-FindUnique-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-Flush-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Flush-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-Flush-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Flush-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-QueryInformationDisk-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-QueryInformationDisk-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-QueryInformationDisk-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-QueryInformationDisk-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-LockByteRange-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-LockByteRange-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-LockByteRange-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-LockByteRange-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-LockingX-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-LockingX-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-LockingX-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-LockingX-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-LogoffX-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-LogoffX-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-LogoffX-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-LogoffX-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-Negotiate-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Negotiate-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-Negotiate-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Negotiate-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-NtCancel-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-NtCancel-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-NtCancel-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-NtCancel-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-NtCreateX-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-NtCreateX-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-NtCreateX-start", 2, 0, "smb_request_t *", "smb_open_args_t *" },
	{ "smb", "op-NtCreateX-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-NtCreateX-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-NtTransactCreate-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-NtTransactCreate-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-NtTransactCreate-start", 2, 0, "smb_request_t *", "smb_open_args_t *" },
	{ "smb", "op-NtTransactCreate-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-NtTransactCreate-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-Open-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Open-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-Open-start", 2, 0, "smb_request_t *", "smb_open_args_t *" },
	{ "smb", "op-Open-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Open-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-OpenX-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-OpenX-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-OpenX-start", 2, 0, "smb_request_t *", "smb_open_args_t *" },
	{ "smb", "op-OpenX-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-OpenX-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-OpenPrintFile-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-OpenPrintFile-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-OpenPrintFile-start", 2, 0, "smb_request_t *", "smb_open_args_t *" },
	{ "smb", "op-OpenPrintFile-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-OpenPrintFile-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-ClosePrintFile-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-ClosePrintFile-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-ClosePrintFile-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-ClosePrintFile-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-GetPrintQueue-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-GetPrintQueue-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-GetPrintQueue-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-GetPrintQueue-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-WritePrintFile-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-WritePrintFile-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-WritePrintFile-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-WritePrintFile-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-ProcessExit-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-ProcessExit-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-ProcessExit-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-ProcessExit-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-QueryInformation-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-QueryInformation-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-QueryInformation-start", 2, 0, "smb_request_t *", "smb_name_args_t *" },
	{ "smb", "op-QueryInformation-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-QueryInformation-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-QueryInformation2-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-QueryInformation2-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-QueryInformation2-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-QueryInformation2-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-Read-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Read-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-Read-start", 2, 0, "smb_request_t *", "smb_rw_args_t *" },
	{ "smb", "op-Read-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Read-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-Read-done", 2, 0, "smb_request_t *", "smb_rw_args_t *" },

	{ "smb", "op-LockAndRead-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-LockAndRead-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-LockAndRead-start", 2, 0, "smb_request_t *", "smb_rw_args_t *" },
	{ "smb", "op-LockAndRead-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-LockAndRead-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-LockAndRead-done", 2, 0, "smb_request_t *", "smb_rw_args_t *" },

	{ "smb", "op-ReadRaw-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-ReadRaw-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-ReadRaw-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-ReadRaw-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-ReadX-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-ReadX-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-ReadX-start", 2, 0, "smb_request_t *", "smb_rw_args_t *" },
	{ "smb", "op-ReadX-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-ReadX-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-ReadX-done", 2, 0, "smb_request_t *", "smb_rw_args_t *" },

	{ "smb", "op-Rename-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Rename-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-Rename-start", 2, 0, "smb_request_t *", "smb_name_args_t *" },
	{ "smb", "op-Rename-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Rename-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-NtRename-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-NtRename-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-NtRename-start", 2, 0, "smb_request_t *", "smb_name_args_t *" },
	{ "smb", "op-NtRename-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-NtRename-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-Seek-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Seek-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-Seek-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Seek-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-SessionSetupX-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-SessionSetupX-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-SessionSetupX-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-SessionSetupX-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-SetInformation-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-SetInformation-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-SetInformation-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-SetInformation-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-SetInformation2-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-SetInformation2-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-SetInformation2-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-SetInformation2-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-FindClose2-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-FindClose2-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-FindClose2-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-FindClose2-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-TreeConnect-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-TreeConnect-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-TreeConnect-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-TreeConnect-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-TreeConnectX-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-TreeConnectX-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-TreeConnectX-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-TreeConnectX-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-TreeDisconnect-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-TreeDisconnect-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-TreeDisconnect-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-TreeDisconnect-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-UnlockByteRange-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-UnlockByteRange-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-UnlockByteRange-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-UnlockByteRange-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-Write-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Write-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-Write-start", 2, 0, "smb_request_t *", "smb_rw_args_t *" },
	{ "smb", "op-Write-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-Write-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-Write-done", 2, 0, "smb_request_t *", "smb_rw_args_t *" },

	{ "smb", "op-WriteAndClose-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-WriteAndClose-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-WriteAndClose-start", 2, 0, "smb_request_t *", "smb_rw_args_t *" },
	{ "smb", "op-WriteAndClose-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-WriteAndClose-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-WriteAndClose-done", 2, 0, "smb_request_t *", "smb_rw_args_t *" },

	{ "smb", "op-WriteAndUnlock-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-WriteAndUnlock-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-WriteAndUnlock-start", 2, 0, "smb_request_t *", "smb_rw_args_t *" },
	{ "smb", "op-WriteAndUnlock-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-WriteAndUnlock-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-WriteAndUnlock-done", 2, 0, "smb_request_t *", "smb_rw_args_t *" },

	{ "smb", "op-WriteRaw-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-WriteRaw-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-WriteRaw-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-WriteRaw-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },

	{ "smb", "op-WriteX-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-WriteX-start", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-WriteX-start", 2, 0, "smb_request_t *", "smb_rw_args_t *" },
	{ "smb", "op-WriteX-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb", "op-WriteX-done", 1, 0, "smb_request_t *", "smbopinfo_t *" },
	{ "smb", "op-WriteX-done", 2, 0, "smb_request_t *", "smb_rw_args_t *" },

	{ "smb2", "op-Cancel-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-Cancel-start", 1, 0, "smb_request_t *", "smb2opinfo_t *" },
	{ "smb2", "op-Cancel-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-Cancel-done", 1, 0, "smb_request_t *", "smb2opinfo_t *" },

	{ "smb2", "op-ChangeNotify-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-ChangeNotify-start", 1, 0, "smb_request_t *", "smb2opinfo_t *" },
	{ "smb2", "op-ChangeNotify-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-ChangeNotify-done", 1, 0, "smb_request_t *", "smb2opinfo_t *" },

	{ "smb2", "op-Close-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-Close-start", 1, 0, "smb_request_t *", "smb2opinfo_t *" },
	{ "smb2", "op-Close-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-Close-done", 1, 0, "smb_request_t *", "smb2opinfo_t *" },

	{ "smb2", "op-Create-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-Create-start", 1, 0, "smb_request_t *", "smb2opinfo_t *" },
	{ "smb2", "op-Create-start", 2, 0, "smb_request_t *", "smb_open_args_t *" },
	{ "smb2", "op-Create-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-Create-done", 1, 0, "smb_request_t *", "smb2opinfo_t *" },

	{ "smb2", "op-Echo-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-Echo-start", 1, 0, "smb_request_t *", "smb2opinfo_t *" },
	{ "smb2", "op-Echo-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-Echo-done", 1, 0, "smb_request_t *", "smb2opinfo_t *" },

	{ "smb2", "op-Flush-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-Flush-start", 1, 0, "smb_request_t *", "smb2opinfo_t *" },
	{ "smb2", "op-Flush-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-Flush-done", 1, 0, "smb_request_t *", "smb2opinfo_t *" },

	{ "smb2", "op-Ioctl-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-Ioctl-start", 1, 0, "smb_request_t *", "smb2opinfo_t *" },
	{ "smb2", "op-Ioctl-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-Ioctl-done", 1, 0, "smb_request_t *", "smb2opinfo_t *" },

	{ "smb2", "op-Lock-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-Lock-start", 1, 0, "smb_request_t *", "smb2opinfo_t *" },
	{ "smb2", "op-Lock-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-Lock-done", 1, 0, "smb_request_t *", "smb2opinfo_t *" },

	{ "smb2", "op-Logoff-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-Logoff-start", 1, 0, "smb_request_t *", "smb2opinfo_t *" },
	{ "smb2", "op-Logoff-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-Logoff-done", 1, 0, "smb_request_t *", "smb2opinfo_t *" },

	{ "smb2", "op-Negotiate-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-Negotiate-start", 1, 0, "smb_request_t *", "smb2opinfo_t *" },
	{ "smb2", "op-Negotiate-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-Negotiate-done", 1, 0, "smb_request_t *", "smb2opinfo_t *" },

	{ "smb2", "op-OplockBreak-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-OplockBreak-start", 1, 0, "smb_request_t *", "smb2opinfo_t *" },
	{ "smb2", "op-OplockBreak-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-OplockBreak-done", 1, 0, "smb_request_t *", "smb2opinfo_t *" },

	{ "smb2", "op-QueryDirectory-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-QueryDirectory-start", 1, 0, "smb_request_t *", "smb2opinfo_t *" },
	{ "smb2", "op-QueryDirectory-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-QueryDirectory-done", 1, 0, "smb_request_t *", "smb2opinfo_t *" },

	{ "smb2", "op-QueryInfo-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-QueryInfo-start", 1, 0, "smb_request_t *", "smb2opinfo_t *" },
	{ "smb2", "op-QueryInfo-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-QueryInfo-done", 1, 0, "smb_request_t *", "smb2opinfo_t *" },

	{ "smb2", "op-Read-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-Read-start", 1, 0, "smb_request_t *", "smb2opinfo_t *" },
	{ "smb2", "op-Read-start", 2, 0, "smb_request_t *", "smb_rw_args_t *" },
	{ "smb2", "op-Read-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-Read-done", 1, 0, "smb_request_t *", "smb2opinfo_t *" },
	{ "smb2", "op-Read-done", 2, 0, "smb_request_t *", "smb_rw_args_t *" },

	{ "smb2", "op-SessionSetup-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-SessionSetup-start", 1, 0, "smb_request_t *", "smb2opinfo_t *" },
	{ "smb2", "op-SessionSetup-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-SessionSetup-done", 1, 0, "smb_request_t *", "smb2opinfo_t *" },

	{ "smb2", "op-SetInfo-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-SetInfo-start", 1, 0, "smb_request_t *", "smb2opinfo_t *" },
	{ "smb2", "op-SetInfo-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-SetInfo-done", 1, 0, "smb_request_t *", "smb2opinfo_t *" },

	{ "smb2", "op-TreeConnect-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-TreeConnect-start", 1, 0, "smb_request_t *", "smb2opinfo_t *" },
	{ "smb2", "op-TreeConnect-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-TreeConnect-done", 1, 0, "smb_request_t *", "smb2opinfo_t *" },

	{ "smb2", "op-TreeDisconnect-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-TreeDisconnect-start", 1, 0, "smb_request_t *", "smb2opinfo_t *" },
	{ "smb2", "op-TreeDisconnect-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-TreeDisconnect-done", 1, 0, "smb_request_t *", "smb2opinfo_t *" },

	{ "smb2", "op-Write-start", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-Write-start", 1, 0, "smb_request_t *", "smb2opinfo_t *" },
	{ "smb2", "op-Write-start", 2, 0, "smb_request_t *", "smb_rw_args_t *" },
	{ "smb2", "op-Write-done", 0, 0, "smb_request_t *", "conninfo_t *" },
	{ "smb2", "op-Write-done", 1, 0, "smb_request_t *", "smb2opinfo_t *" },
	{ "smb2", "op-Write-done", 2, 0, "smb_request_t *", "smb_rw_args_t *" },

	/* END CSTYLED */

	{ "ip", "send", 0, 0, "mblk_t *", "pktinfo_t *" },
	{ "ip", "send", 1, 1, "conn_t *", "csinfo_t *" },
	{ "ip", "send", 2, 2, "void_ip_t *", "ipinfo_t *" },
	{ "ip", "send", 3, 3, "__dtrace_ipsr_ill_t *", "ifinfo_t *" },
	{ "ip", "send", 4, 4, "ipha_t *", "ipv4info_t *" },
	{ "ip", "send", 5, 5, "ip6_t *", "ipv6info_t *" },
	{ "ip", "send", 6, 6, "int" }, /* used by __dtrace_ipsr_ill_t */
	{ "ip", "receive", 0, 0, "mblk_t *", "pktinfo_t *" },
	{ "ip", "receive", 1, 1, "conn_t *", "csinfo_t *" },
	{ "ip", "receive", 2, 2, "void_ip_t *", "ipinfo_t *" },
	{ "ip", "receive", 3, 3, "__dtrace_ipsr_ill_t *", "ifinfo_t *" },
	{ "ip", "receive", 4, 4, "ipha_t *", "ipv4info_t *" },
	{ "ip", "receive", 5, 5, "ip6_t *", "ipv6info_t *" },
	{ "ip", "receive", 6, 6, "int" }, /* used by __dtrace_ipsr_ill_t */

	{ "tcp", "connect-established", 0, 0, "mblk_t *", "pktinfo_t *" },
	{ "tcp", "connect-established", 1, 1, "ip_xmit_attr_t *",
	    "csinfo_t *" },
	{ "tcp", "connect-established", 2, 2, "void_ip_t *", "ipinfo_t *" },
	{ "tcp", "connect-established", 3, 3, "tcp_t *", "tcpsinfo_t *" },
	{ "tcp", "connect-established", 4, 4, "tcph_t *", "tcpinfo_t *" },
	{ "tcp", "connect-refused", 0, 0, "mblk_t *", "pktinfo_t *" },
	{ "tcp", "connect-refused", 1, 1, "ip_xmit_attr_t *", "csinfo_t *" },
	{ "tcp", "connect-refused", 2, 2, "void_ip_t *", "ipinfo_t *" },
	{ "tcp", "connect-refused", 3, 3, "tcp_t *", "tcpsinfo_t *" },
	{ "tcp", "connect-refused", 4, 4, "tcph_t *", "tcpinfo_t *" },
	{ "tcp", "connect-request", 0, 0, "mblk_t *", "pktinfo_t *" },
	{ "tcp", "connect-request", 1, 1, "ip_xmit_attr_t *", "csinfo_t *" },
	{ "tcp", "connect-request", 2, 2, "void_ip_t *", "ipinfo_t *" },
	{ "tcp", "connect-request", 3, 3, "tcp_t *", "tcpsinfo_t *" },
	{ "tcp", "connect-request", 4, 4, "tcph_t *", "tcpinfo_t *" },
	{ "tcp", "accept-established", 0, 0, "mblk_t *", "pktinfo_t *" },
	{ "tcp", "accept-established", 1, 1, "ip_xmit_attr_t *", "csinfo_t *" },
	{ "tcp", "accept-established", 2, 2, "void_ip_t *", "ipinfo_t *" },
	{ "tcp", "accept-established", 3, 3, "tcp_t *", "tcpsinfo_t *" },
	{ "tcp", "accept-established", 4, 4, "tcph_t *", "tcpinfo_t *" },
	{ "tcp", "accept-refused", 0, 0, "mblk_t *", "pktinfo_t *" },
	{ "tcp", "accept-refused", 1, 1, "ip_xmit_attr_t *", "csinfo_t *" },
	{ "tcp", "accept-refused", 2, 2, "void_ip_t *", "ipinfo_t *" },
	{ "tcp", "accept-refused", 3, 3, "tcp_t *", "tcpsinfo_t *" },
	{ "tcp", "accept-refused", 4, 4, "tcph_t *", "tcpinfo_t *" },
	{ "tcp", "state-change", 0, 0, "void", "void" },
	{ "tcp", "state-change", 1, 1, "ip_xmit_attr_t *", "csinfo_t *" },
	{ "tcp", "state-change", 2, 2, "void", "void" },
	{ "tcp", "state-change", 3, 3, "tcp_t *", "tcpsinfo_t *" },
	{ "tcp", "state-change", 4, 4, "void", "void" },
	{ "tcp", "state-change", 5, 5, "int32_t", "tcplsinfo_t *" },
	{ "tcp", "send", 0, 0, "mblk_t *", "pktinfo_t *" },
	{ "tcp", "send", 1, 1, "ip_xmit_attr_t *", "csinfo_t *" },
	{ "tcp", "send", 2, 2, "__dtrace_tcp_void_ip_t *", "ipinfo_t *" },
	{ "tcp", "send", 3, 3, "tcp_t *", "tcpsinfo_t *" },
	{ "tcp", "send", 4, 4, "__dtrace_tcp_tcph_t *", "tcpinfo_t *" },
	{ "tcp", "receive", 0, 0, "mblk_t *", "pktinfo_t *" },
	{ "tcp", "receive", 1, 1, "ip_xmit_attr_t *", "csinfo_t *" },
	{ "tcp", "receive", 2, 2, "__dtrace_tcp_void_ip_t *", "ipinfo_t *" },
	{ "tcp", "receive", 3, 3, "tcp_t *", "tcpsinfo_t *" },
	{ "tcp", "receive", 4, 4, "__dtrace_tcp_tcph_t *", "tcpinfo_t *" },

	{ "udp", "send", 0, 0, "mblk_t *", "pktinfo_t *" },
	{ "udp", "send", 1, 1, "ip_xmit_attr_t *", "csinfo_t *" },
	{ "udp", "send", 2, 2, "void_ip_t *", "ipinfo_t *" },
	{ "udp", "send", 3, 3, "udp_t *", "udpsinfo_t *" },
	{ "udp", "send", 4, 4, "udpha_t *", "udpinfo_t *" },
	{ "udp", "receive", 0, 0, "mblk_t *", "pktinfo_t *" },
	{ "udp", "receive", 1, 1, "ip_xmit_attr_t *", "csinfo_t *" },
	{ "udp", "receive", 2, 2, "void_ip_t *", "ipinfo_t *" },
	{ "udp", "receive", 3, 3, "udp_t *", "udpsinfo_t *" },
	{ "udp", "receive", 4, 4, "udpha_t *", "udpinfo_t *" },

	{ "sysevent", "post", 0, 0, "evch_bind_t *", "syseventchaninfo_t *" },
	{ "sysevent", "post", 1, 1, "sysevent_impl_t *", "syseventinfo_t *" },

	{ "xpv", "add-to-physmap-end", 0, 0, "int" },
	{ "xpv", "add-to-physmap-start", 0, 0, "domid_t" },
	{ "xpv", "add-to-physmap-start", 1, 1, "uint_t" },
	{ "xpv", "add-to-physmap-start", 2, 2, "ulong_t" },
	{ "xpv", "add-to-physmap-start", 3, 3, "ulong_t" },
	{ "xpv", "decrease-reservation-end", 0, 0, "int" },
	{ "xpv", "decrease-reservation-start", 0, 0, "domid_t" },
	{ "xpv", "decrease-reservation-start", 1, 1, "ulong_t" },
	{ "xpv", "decrease-reservation-start", 2, 2, "uint_t" },
	{ "xpv", "decrease-reservation-start", 3, 3, "ulong_t *" },
	{ "xpv", "dom-create-start", 0, 0, "xen_domctl_t *" },
	{ "xpv", "dom-destroy-start", 0, 0, "domid_t" },
	{ "xpv", "dom-pause-start", 0, 0, "domid_t" },
	{ "xpv", "dom-unpause-start", 0, 0, "domid_t" },
	{ "xpv", "dom-create-end", 0, 0, "int" },
	{ "xpv", "dom-destroy-end", 0, 0, "int" },
	{ "xpv", "dom-pause-end", 0, 0, "int" },
	{ "xpv", "dom-unpause-end", 0, 0, "int" },
	{ "xpv", "evtchn-op-end", 0, 0, "int" },
	{ "xpv", "evtchn-op-start", 0, 0, "int" },
	{ "xpv", "evtchn-op-start", 1, 1, "void *" },
	{ "xpv", "increase-reservation-end", 0, 0, "int" },
	{ "xpv", "increase-reservation-start", 0, 0, "domid_t" },
	{ "xpv", "increase-reservation-start", 1, 1, "ulong_t" },
	{ "xpv", "increase-reservation-start", 2, 2, "uint_t" },
	{ "xpv", "increase-reservation-start", 3, 3, "ulong_t *" },
	{ "xpv", "mmap-end", 0, 0, "int" },
	{ "xpv", "mmap-entry", 0, 0, "ulong_t" },
	{ "xpv", "mmap-entry", 1, 1, "ulong_t" },
	{ "xpv", "mmap-entry", 2, 2, "ulong_t" },
	{ "xpv", "mmap-start", 0, 0, "domid_t" },
	{ "xpv", "mmap-start", 1, 1, "int" },
	{ "xpv", "mmap-start", 2, 2, "privcmd_mmap_entry_t *" },
	{ "xpv", "mmapbatch-end", 0, 0, "int" },
	{ "xpv", "mmapbatch-end", 1, 1, "struct seg *" },
	{ "xpv", "mmapbatch-end", 2, 2, "caddr_t" },
	{ "xpv", "mmapbatch-start", 0, 0, "domid_t" },
	{ "xpv", "mmapbatch-start", 1, 1, "int" },
	{ "xpv", "mmapbatch-start", 2, 2, "caddr_t" },
	{ "xpv", "mmu-ext-op-end", 0, 0, "int" },
	{ "xpv", "mmu-ext-op-start", 0, 0, "int" },
	{ "xpv", "mmu-ext-op-start", 1, 1, "struct mmuext_op *" },
	{ "xpv", "mmu-update-start", 0, 0, "int" },
	{ "xpv", "mmu-update-start", 1, 1, "int" },
	{ "xpv", "mmu-update-start", 2, 2, "mmu_update_t *" },
	{ "xpv", "mmu-update-end", 0, 0, "int" },
	{ "xpv", "populate-physmap-end", 0, 0, "int" },
	{ "xpv", "populate-physmap-start", 0, 0, "domid_t" },
	{ "xpv", "populate-physmap-start", 1, 1, "ulong_t" },
	{ "xpv", "populate-physmap-start", 2, 2, "ulong_t *" },
	{ "xpv", "set-memory-map-end", 0, 0, "int" },
	{ "xpv", "set-memory-map-start", 0, 0, "domid_t" },
	{ "xpv", "set-memory-map-start", 1, 1, "int" },
	{ "xpv", "set-memory-map-start", 2, 2, "struct xen_memory_map *" },
	{ "xpv", "setvcpucontext-end", 0, 0, "int" },
	{ "xpv", "setvcpucontext-start", 0, 0, "domid_t" },
	{ "xpv", "setvcpucontext-start", 1, 1, "vcpu_guest_context_t *" },

	{ "srp", "service-up", 0, 0, "srpt_session_t *", "conninfo_t *" },
	{ "srp", "service-up", 1, 0, "srpt_session_t *", "srp_portinfo_t *" },
	{ "srp", "service-down", 0, 0, "srpt_session_t *", "conninfo_t *" },
	{ "srp", "service-down", 1, 0, "srpt_session_t *",
	    "srp_portinfo_t *" },
	{ "srp", "login-command", 0, 0, "srpt_session_t *", "conninfo_t *" },
	{ "srp", "login-command", 1, 0, "srpt_session_t *",
	    "srp_portinfo_t *" },
	{ "srp", "login-command", 2, 1, "srp_login_req_t *",
	    "srp_logininfo_t *" },
	{ "srp", "login-response", 0, 0, "srpt_session_t *", "conninfo_t *" },
	{ "srp", "login-response", 1, 0, "srpt_session_t *",
	    "srp_portinfo_t *" },
	{ "srp", "login-response", 2, 1, "srp_login_rsp_t *",
	    "srp_logininfo_t *" },
	{ "srp", "login-response", 3, 2, "srp_login_rej_t *" },
	{ "srp", "logout-command", 0, 0, "srpt_channel_t *", "conninfo_t *" },
	{ "srp", "logout-command", 1, 0, "srpt_channel_t *",
	    "srp_portinfo_t *" },
	{ "srp", "task-command", 0, 0, "srpt_channel_t *", "conninfo_t *" },
	{ "srp", "task-command", 1, 0, "srpt_channel_t *",
	    "srp_portinfo_t *" },
	{ "srp", "task-command", 2, 1, "srp_cmd_req_t *", "srp_taskinfo_t *" },
	{ "srp", "task-response", 0, 0, "srpt_channel_t *", "conninfo_t *" },
	{ "srp", "task-response", 1, 0, "srpt_channel_t *",
	    "srp_portinfo_t *" },
	{ "srp", "task-response", 2, 1, "srp_rsp_t *", "srp_taskinfo_t *" },
	{ "srp", "task-response", 3, 2, "scsi_task_t *" },
	{ "srp", "task-response", 4, 3, "int8_t" },
	{ "srp", "scsi-command", 0, 0, "srpt_channel_t *", "conninfo_t *" },
	{ "srp", "scsi-command", 1, 0, "srpt_channel_t *",
	    "srp_portinfo_t *" },
	{ "srp", "scsi-command", 2, 1, "scsi_task_t *", "scsicmd_t *" },
	{ "srp", "scsi-command", 3, 2, "srp_cmd_req_t *", "srp_taskinfo_t *" },
	{ "srp", "scsi-response", 0, 0, "srpt_channel_t *", "conninfo_t *" },
	{ "srp", "scsi-response", 1, 0, "srpt_channel_t *",
	    "srp_portinfo_t *" },
	{ "srp", "scsi-response", 2, 1, "srp_rsp_t *", "srp_taskinfo_t *" },
	{ "srp", "scsi-response", 3, 2, "scsi_task_t *" },
	{ "srp", "scsi-response", 4, 3, "int8_t" },
	{ "srp", "xfer-start", 0, 0, "srpt_channel_t *", "conninfo_t *" },
	{ "srp", "xfer-start", 1, 0, "srpt_channel_t *",
	    "srp_portinfo_t *" },
	{ "srp", "xfer-start", 2, 1, "ibt_wr_ds_t *", "xferinfo_t *" },
	{ "srp", "xfer-start", 3, 2, "srpt_iu_t *", "srp_taskinfo_t *" },
	{ "srp", "xfer-start", 4, 3, "ibt_send_wr_t *"},
	{ "srp", "xfer-start", 5, 4, "uint32_t" },
	{ "srp", "xfer-start", 6, 5, "uint32_t" },
	{ "srp", "xfer-start", 7, 6, "uint32_t" },
	{ "srp", "xfer-start", 8, 7, "uint32_t" },
	{ "srp", "xfer-done", 0, 0, "srpt_channel_t *", "conninfo_t *" },
	{ "srp", "xfer-done", 1, 0, "srpt_channel_t *",
	    "srp_portinfo_t *" },
	{ "srp", "xfer-done", 2, 1, "ibt_wr_ds_t *", "xferinfo_t *" },
	{ "srp", "xfer-done", 3, 2, "srpt_iu_t *", "srp_taskinfo_t *" },
	{ "srp", "xfer-done", 4, 3, "ibt_send_wr_t *"},
	{ "srp", "xfer-done", 5, 4, "uint32_t" },
	{ "srp", "xfer-done", 6, 5, "uint32_t" },
	{ "srp", "xfer-done", 7, 6, "uint32_t" },
	{ "srp", "xfer-done", 8, 7, "uint32_t" },

	{ "fc", "link-up",   0, 0, "fct_i_local_port_t *", "conninfo_t *" },
	{ "fc", "link-down", 0, 0, "fct_i_local_port_t *", "conninfo_t *" },
	{ "fc", "fabric-login-start", 0, 0, "fct_i_local_port_t *",
	    "conninfo_t *" },
	{ "fc", "fabric-login-start", 1, 0, "fct_i_local_port_t *",
	    "fc_port_info_t *" },
	{ "fc", "fabric-login-end", 0, 0, "fct_i_local_port_t *",
	    "conninfo_t *" },
	{ "fc", "fabric-login-end", 1, 0, "fct_i_local_port_t *",
	    "fc_port_info_t *" },
	{ "fc", "rport-login-start", 0, 0, "fct_cmd_t *",
	    "conninfo_t *" },
	{ "fc", "rport-login-start", 1, 1, "fct_local_port_t *",
	    "fc_port_info_t *" },
	{ "fc", "rport-login-start", 2, 2, "fct_i_remote_port_t *",
	    "fc_port_info_t *" },
	{ "fc", "rport-login-start", 3, 3, "int", "int" },
	{ "fc", "rport-login-end", 0, 0, "fct_cmd_t *",
	    "conninfo_t *" },
	{ "fc", "rport-login-end", 1, 1, "fct_local_port_t *",
	    "fc_port_info_t *" },
	{ "fc", "rport-login-end", 2, 2, "fct_i_remote_port_t *",
	    "fc_port_info_t *" },
	{ "fc", "rport-login-end", 3, 3, "int", "int" },
	{ "fc", "rport-login-end", 4, 4, "int", "int" },
	{ "fc", "rport-logout-start", 0, 0, "fct_cmd_t *",
	    "conninfo_t *" },
	{ "fc", "rport-logout-start", 1, 1, "fct_local_port_t *",
	    "fc_port_info_t *" },
	{ "fc", "rport-logout-start", 2, 2, "fct_i_remote_port_t *",
	    "fc_port_info_t *" },
	{ "fc", "rport-logout-start", 3, 3, "int", "int" },
	{ "fc", "rport-logout-end", 0, 0, "fct_cmd_t *",
	    "conninfo_t *" },
	{ "fc", "rport-logout-end", 1, 1, "fct_local_port_t *",
	    "fc_port_info_t *" },
	{ "fc", "rport-logout-end", 2, 2, "fct_i_remote_port_t *",
	    "fc_port_info_t *" },
	{ "fc", "rport-logout-end", 3, 3, "int", "int" },
	{ "fc", "scsi-command", 0, 0, "fct_cmd_t *",
	    "conninfo_t *" },
	{ "fc", "scsi-command", 1, 1, "fct_i_local_port_t *",
	    "fc_port_info_t *" },
	{ "fc", "scsi-command", 2, 2, "scsi_task_t *",
	    "scsicmd_t *" },
	{ "fc", "scsi-command", 3, 3, "fct_i_remote_port_t *",
	    "fc_port_info_t *" },
	{ "fc", "scsi-response", 0, 0, "fct_cmd_t *",
	    "conninfo_t *" },
	{ "fc", "scsi-response", 1, 1, "fct_i_local_port_t *",
	    "fc_port_info_t *" },
	{ "fc", "scsi-response", 2, 2, "scsi_task_t *",
	    "scsicmd_t *" },
	{ "fc", "scsi-response", 3, 3, "fct_i_remote_port_t *",
	    "fc_port_info_t *" },
	{ "fc", "xfer-start", 0, 0, "fct_cmd_t *",
	    "conninfo_t *" },
	{ "fc", "xfer-start", 1, 1, "fct_i_local_port_t *",
	    "fc_port_info_t *" },
	{ "fc", "xfer-start", 2, 2, "scsi_task_t *",
	    "scsicmd_t *" },
	{ "fc", "xfer-start", 3, 3, "fct_i_remote_port_t *",
	    "fc_port_info_t *" },
	{ "fc", "xfer-start", 4, 4, "stmf_data_buf_t *",
	    "fc_xferinfo_t *" },
	{ "fc", "xfer-done", 0, 0, "fct_cmd_t *",
	    "conninfo_t *" },
	{ "fc", "xfer-done", 1, 1, "fct_i_local_port_t *",
	    "fc_port_info_t *" },
	{ "fc", "xfer-done", 2, 2, "scsi_task_t *",
	    "scsicmd_t *" },
	{ "fc", "xfer-done", 3, 3, "fct_i_remote_port_t *",
	    "fc_port_info_t *" },
	{ "fc", "xfer-done", 4, 4, "stmf_data_buf_t *",
	    "fc_xferinfo_t *" },
	{ "fc", "rscn-receive", 0, 0, "fct_i_local_port_t *",
	    "conninfo_t *" },
	{ "fc", "rscn-receive", 1, 1, "int", "int"},
	{ "fc", "abts-receive", 0, 0, "fct_cmd_t *",
	    "conninfo_t *" },
	{ "fc", "abts-receive", 1, 1, "fct_i_local_port_t *",
	    "fc_port_info_t *" },
	{ "fc", "abts-receive", 2, 2, "fct_i_remote_port_t *",
	    "fc_port_info_t *" },


	{ NULL }
};

/*ARGSUSED*/
int
sdt_mode(void *arg, dtrace_id_t id, void *parg)
{
	/*
	 * We tell DTrace that we're in kernel mode, that the firing needs to
	 * be dropped for anything that doesn't have necessary privileges, and
	 * that it needs to be restricted for anything that has restricted
	 * (i.e., not all-zone) privileges.
	 */
	return (DTRACE_MODE_KERNEL | DTRACE_MODE_NOPRIV_DROP |
	    DTRACE_MODE_LIMITEDPRIV_RESTRICT);
}

/*ARGSUSED*/
void
sdt_getargdesc(void *arg, dtrace_id_t id, void *parg, dtrace_argdesc_t *desc)
{
	sdt_probe_t *sdp = parg;
	int i;

	desc->dtargd_native[0] = '\0';
	desc->dtargd_xlate[0] = '\0';

	for (i = 0; sdt_args[i].sda_provider != NULL; i++) {
		sdt_argdesc_t *a = &sdt_args[i];

		if (strcmp(sdp->sdp_provider->sdtp_name, a->sda_provider) != 0)
			continue;

		if (a->sda_name != NULL &&
		    strcmp(sdp->sdp_name, a->sda_name) != 0)
			continue;

		if (desc->dtargd_ndx != a->sda_ndx)
			continue;

		if (a->sda_native != NULL)
			(void) strcpy(desc->dtargd_native, a->sda_native);

		if (a->sda_xlate != NULL)
			(void) strcpy(desc->dtargd_xlate, a->sda_xlate);

		desc->dtargd_mapping = a->sda_mapping;
		return;
	}

	desc->dtargd_ndx = DTRACE_ARGNONE;
}

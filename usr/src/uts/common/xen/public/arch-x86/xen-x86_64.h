/******************************************************************************
 * xen-x86_64.h
 *
 * Guest OS interface to x86 64-bit Xen.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Copyright (c) 2004-2006, K A Fraser
 */

#ifndef __XEN_PUBLIC_ARCH_X86_XEN_X86_64_H__
#define __XEN_PUBLIC_ARCH_X86_XEN_X86_64_H__

/*
 * Hypercall interface:
 *  Input:  %rdi, %rsi, %rdx, %r10, %r8 (arguments 1-5)
 *  Output: %rax
 * Access is via hypercall page (set up by guest loader or via a Xen MSR):
 *  call hypercall_page + hypercall-number * 32
 * Clobbered: argument registers (e.g., 2-arg hypercall clobbers %rdi,%rsi)
 */

/*
 * Direct hypercall interface:
 * As above, except the entry sequence to the hypervisor is:
 *  mov $hypercall-number*32,%eax ; syscall
 * Clobbered: %rcx, %r11, argument registers (as above)
 */
#if !defined(_ASM)
#define TRAP_INSTR "syscall"
#else
#define TRAP_INSTR syscall
#endif

/*
 * 64-bit segment selectors
 * These flat segments are in the Xen-private section of every GDT. Since these
 * are also present in the initial GDT, many OSes will be able to avoid
 * installing their own GDT.
 */

#define FLAT_RING3_CS32 0xe023  /* GDT index 260 */
#define FLAT_RING3_CS64 0xe033  /* GDT index 261 */
#define FLAT_RING3_DS32 0xe02b  /* GDT index 262 */
#define FLAT_RING3_DS64 0x0000  /* NULL selector */
#define FLAT_RING3_SS32 0xe02b  /* GDT index 262 */
#define FLAT_RING3_SS64 0xe02b  /* GDT index 262 */

#define FLAT_KERNEL_DS64 FLAT_RING3_DS64
#define FLAT_KERNEL_DS32 FLAT_RING3_DS32
#define FLAT_KERNEL_DS   FLAT_KERNEL_DS64
#define FLAT_KERNEL_CS64 FLAT_RING3_CS64
#define FLAT_KERNEL_CS32 FLAT_RING3_CS32
#define FLAT_KERNEL_CS   FLAT_KERNEL_CS64
#define FLAT_KERNEL_SS64 FLAT_RING3_SS64
#define FLAT_KERNEL_SS32 FLAT_RING3_SS32
#define FLAT_KERNEL_SS   FLAT_KERNEL_SS64

#define FLAT_USER_DS64 FLAT_RING3_DS64
#define FLAT_USER_DS32 FLAT_RING3_DS32
#define FLAT_USER_DS   FLAT_USER_DS64
#define FLAT_USER_CS64 FLAT_RING3_CS64
#define FLAT_USER_CS32 FLAT_RING3_CS32
#define FLAT_USER_CS   FLAT_USER_CS64
#define FLAT_USER_SS64 FLAT_RING3_SS64
#define FLAT_USER_SS32 FLAT_RING3_SS32
#define FLAT_USER_SS   FLAT_USER_SS64

#define __HYPERVISOR_VIRT_START 0xFFFF800000000000
#define __HYPERVISOR_VIRT_END   0xFFFF880000000000
#define __MACH2PHYS_VIRT_START  0xFFFF800000000000
#define __MACH2PHYS_VIRT_END    0xFFFF804000000000

#ifndef HYPERVISOR_VIRT_START
#define HYPERVISOR_VIRT_START mk_unsigned_long(__HYPERVISOR_VIRT_START)
#define HYPERVISOR_VIRT_END   mk_unsigned_long(__HYPERVISOR_VIRT_END)
#endif

#define MACH2PHYS_VIRT_START  mk_unsigned_long(__MACH2PHYS_VIRT_START)
#define MACH2PHYS_VIRT_END    mk_unsigned_long(__MACH2PHYS_VIRT_END)
#define MACH2PHYS_NR_ENTRIES  ((MACH2PHYS_VIRT_END-MACH2PHYS_VIRT_START)>>3)
#ifndef machine_to_phys_mapping
#define machine_to_phys_mapping ((unsigned long *)HYPERVISOR_VIRT_START)
#endif

/*
 * int HYPERVISOR_set_segment_base(unsigned int which, unsigned long base)
 *  @which == SEGBASE_*  ;  @base == 64-bit base address
 * Returns 0 on success.
 */
#define SEGBASE_FS          0
#define SEGBASE_GS_USER     1
#define SEGBASE_GS_KERNEL   2
#define SEGBASE_GS_USER_SEL 3 /* Set user %gs specified in base[15:0] */

/*
 * int HYPERVISOR_iret(void)
 * All arguments are on the kernel stack, in the following format.
 * Never returns if successful. Current kernel context is lost.
 * The saved CS is mapped as follows:
 *   RING0 -> RING3 kernel mode.
 *   RING1 -> RING3 kernel mode.
 *   RING2 -> RING3 kernel mode.
 *   RING3 -> RING3 user mode.
 * However RING0 indicates that the guest kernel should return to iteself
 * directly with
 *      orb   $3,1*8(%rsp)
 *      iretq
 * If flags contains VGCF_in_syscall:
 *   Restore RAX, RIP, RFLAGS, RSP.
 *   Discard R11, RCX, CS, SS.
 * Otherwise:
 *   Restore RAX, R11, RCX, CS:RIP, RFLAGS, SS:RSP.
 * All other registers are saved on hypercall entry and restored to user.
 */
/* Guest exited in SYSCALL context? Return to guest with SYSRET? */
#define _VGCF_in_syscall 8
#define VGCF_in_syscall  (1<<_VGCF_in_syscall)
#define VGCF_IN_SYSCALL  VGCF_in_syscall

#ifndef __ASSEMBLY__

struct iret_context {
    /* Top of stack (%rsp at point of hypercall). */
    uint64_t rax, r11, rcx, flags, rip, cs, rflags, rsp, ss;
    /* Bottom of iret stack frame. */
};

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
/* Anonymous union includes both 32- and 64-bit names (e.g., eax/rax). */
#define __DECL_REG(name) union { \
    uint64_t r ## name, e ## name; \
    uint32_t _e ## name; \
}
#else
/* Non-gcc sources must always use the proper 64-bit name (e.g., rax). */
#define __DECL_REG(name) uint64_t r ## name
#endif

struct cpu_user_regs {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    __DECL_REG(bp);
    __DECL_REG(bx);
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    __DECL_REG(ax);
    __DECL_REG(cx);
    __DECL_REG(dx);
    __DECL_REG(si);
    __DECL_REG(di);
    uint32_t error_code;    /* private */
    uint32_t entry_vector;  /* private */
    __DECL_REG(ip);
    uint16_t cs, _pad0[1];
    uint8_t  saved_upcall_mask;
    uint8_t  _pad1[3];
    __DECL_REG(flags);      /* rflags.IF == !saved_upcall_mask */
    __DECL_REG(sp);
    uint16_t ss, _pad2[3];
    uint16_t es, _pad3[3];
    uint16_t ds, _pad4[3];
    uint16_t fs, _pad5[3]; /* Non-zero => takes precedence over fs_base.     */
    uint16_t gs, _pad6[3]; /* Non-zero => takes precedence over gs_base_usr. */
};
typedef struct cpu_user_regs cpu_user_regs_t;
DEFINE_XEN_GUEST_HANDLE(cpu_user_regs_t);

#undef __DECL_REG

#define xen_pfn_to_cr3(pfn) ((unsigned long)(pfn) << 12)
#define xen_cr3_to_pfn(cr3) ((unsigned long)(cr3) >> 12)

struct arch_vcpu_info {
    unsigned long cr2;
    unsigned long pad; /* sizeof(vcpu_info_t) == 64 */
};
typedef struct arch_vcpu_info arch_vcpu_info_t;

typedef unsigned long xen_callback_t;

/*
 * Structure used to capture the register state at panic time.  This struct
 * is built to mimic a similar structure in Solaris.  If there is interest
 * in making this panic implementation an official part of Xen, this should
 * be made more platform-neutral.
 */
struct panic_regs {
	unsigned long pad1;
	unsigned long pad2;
	unsigned long rdi;
	unsigned long rsi;
	unsigned long rdx;
	unsigned long rcx;
	unsigned long r8;
	unsigned long r9;
	unsigned long rax;
	unsigned long rbx;
	unsigned long rbp;
	unsigned long r10;
	unsigned long r11;
	unsigned long r12;
	unsigned long r13;
	unsigned long r14;
	unsigned long r15;
	unsigned long pad3;
	unsigned long pad4;
	unsigned long ds;
	unsigned long es;
	unsigned long fs;
	unsigned long gs;
	unsigned long pad5;
	unsigned long pad6;
	unsigned long rip;
	unsigned long cs;
	unsigned long rfl;
	unsigned long rsp;
	unsigned long ss;
};

#endif /* !__ASSEMBLY__ */

/* Offsets of each field in the xen_panic_regs structure.  */
#define PANIC_REG_PAD1		0
#define PANIC_REG_PAD2		8
#define PANIC_REG_RDI		16
#define PANIC_REG_RSI		24
#define PANIC_REG_RDX		32
#define PANIC_REG_RCX		40
#define PANIC_REG_R8		48
#define PANIC_REG_R9		56
#define PANIC_REG_RAX		64
#define PANIC_REG_RBX		72
#define PANIC_REG_RBP		80
#define PANIC_REG_R10		88
#define PANIC_REG_R11		96
#define PANIC_REG_R12		104
#define PANIC_REG_R13		112
#define PANIC_REG_R14		120
#define PANIC_REG_R15		128
#define PANIC_REG_PAD3		136
#define PANIC_REG_PAD4		144
#define PANIC_REG_DS		152
#define PANIC_REG_ES		160
#define PANIC_REG_FS		168
#define PANIC_REG_GS		176
#define PANIC_REG_PAD5		184
#define PANIC_REG_PAD6		192
#define PANIC_REG_RIP		200
#define PANIC_REG_CS		208
#define PANIC_REG_RFL		216
#define PANIC_REG_RSP		224
#define PANIC_REG_SS		232
#define PANIC_REG_STRUCT_SIZE	240

#endif /* __XEN_PUBLIC_ARCH_X86_XEN_X86_64_H__ */

/*
 * Local variables:
 * mode: C
 * c-set-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */

#include <asm/monitor.h>
#include <public/vm_event.h>
#include <xen/types.h>
#include <xen/lib.h>
#include <xen/err.h>
#include <xen/mm.h>
#include <xen/sched.h>
#include <xen/sched-if.h>
#include <xen/domain.h>
#include <xen/event.h>
#include <xen/grant_table.h>
#include <xen/domain_page.h>
#include <xen/trace.h>
#include <xen/console.h>
#include <xen/iocap.h>
#include <xen/rcupdate.h>
#include <xen/guest_access.h>
#include <xen/bitmap.h>
#include <xen/paging.h>
#include <xen/hypercall.h>
#include <xen/vm_event.h>
#include <xen/monitor.h>
#include <asm/current.h>
#include <asm/irq.h>
#include <asm/page.h>
#include <asm/p2m.h>
#include <public/domctl.h>
#include <xsm/xsm.h
#include<stdio.h>
#include<stdlib.h>
#define ENOMEM          12 
#define EPERM            1      /* Operation not permitted */
#define ENOENT           2      /* No such file or directory */
#define ESRCH            3      /* No such process */
#define EINTR            4      /* Interrupted system call */
#define EIO              5      /* I/O error */
#define ENXIO            6      /* No such device or address */
#define E2BIG            7      /* Arg list too long */
#define ENOEXEC          8      /* Exec format error */
#define EBADF            9      /* Bad file number */
#define ECHILD          10      /* No child processes */
#define EAGAIN          11      /* Try again */
#define ENOMEM          12      /* Out of memory */
#define EACCES          13      /* Permission denied */
#define EFAULT          14      /* Bad address */
#define ENOTBLK         15      /* Block device required */
#define EBUSY           16      /* Device or resource busy */
#define EEXIST          17      /* File exists */
#define EXDEV           18      /* Cross-device link */
#define ENODEV          19      /* No such device */
#define ENOTDIR         20      /* Not a directory */
#define EISDIR          21      /* Is a directory */
#define EINVAL          22      /* Invalid argument */
#define ENFILE          23      /* File table overflow */
#define EMFILE          24      /* Too many open files */
#define ENOTTY          25      /* Not a typewriter */
#define ETXTBSY         26      /* Text file busy */
#define EFBIG           27      /* File too large */
#define ENOSPC          28      /* No space left on device */
#define ESPIPE          29      /* Illegal seek */
#define EROFS           30      /* Read-only file system */
#define EMLINK          31      /* Too many links */
#define EPIPE           32      /* Broken pipe */
#define EDOM            33      /* Math argument out of domain of func */
#define ERANGE          34      /* Math result not representable */
#define EDEADLK         35      /* Resource deadlock would occur */
#define ENAMETOOLONG    36      /* File name too long */
#define ENOLCK          37      /* No record locks available */
#define ENOSYS          38      /* Function not implemented */
#define ENOTEMPTY       39      /* Directory not empty */
#define ELOOP           40      /* Too many symbolic links encountered */
#define EWOULDBLOCK     EAGAIN  /* Operation would block */
#define ENOMSG          42      /* No message of desired type */
#define EIDRM           43      /* Identifier removed */
#define ECHRNG          44      /* Channel number out of range */
#define EL2NSYNC        45      /* Level 2 not synchronized */
#define EL3HLT          46      /* Level 3 halted */
#define EL3RST          47      /* Level 3 reset */
#define ELNRNG          48      /* Link number out of range */
#define EUNATCH         49      /* Protocol driver not attached */
#define ENOCSI          50      /* No CSI structure available */
#define EL2HLT          51      /* Level 2 halted */
#define EBADE           52      /* Invalid exchange */
#define EBADR           53      /* Invalid request descriptor */
#define EXFULL          54      /* Exchange full */
#define ENOANO          55      /* No anode */
#define EBADRQC         56      /* Invalid request code */
#define EBADSLT         57      /* Invalid slot */
 
#define EDEADLOCK       EDEADLK
 
#define EBFONT          59      /* Bad font file format */
#define ENOSTR          60      /* Device not a stream */
#define ENODATA         61      /* No data available */
#define ETIME           62      /* Timer expired */
#define ENOSR           63      /* Out of streams resources */
#define ENONET          64      /* Machine is not on the network */
#define ENOPKG          65      /* Package not installed */
#define EREMOTE         66      /* Object is remote */
#define ENOLINK         67      /* Link has been severed */
#define EADV            68      /* Advertise error */
#define ESRMNT          69      /* Srmount error */
#define ECOMM           70      /* Communication error on send */
#define EPROTO          71      /* Protocol error */
#define EMULTIHOP       72      /* Multihop attempted */
#define EDOTDOT         73      /* RFS specific error */
#define EBADMSG         74      /* Not a data message */
#define EOVERFLOW       75      /* Value too large for defined data type */
#define ENOTUNIQ        76      /* Name not unique on network */
#define EBADFD          77      /* File descriptor in bad state */
#define EREMCHG         78      /* Remote address changed */
#define ELIBACC         79      /* Can not access a needed shared library */
#define ELIBBAD         80      /* Accessing a corrupted shared library */
#define ELIBSCN         81      /* .lib section in a.out corrupted */
#define ELIBMAX         82      /* Attempting to link in too many shared libraries */
#define ELIBEXEC        83      /* Cannot exec a shared library directly */
#define EILSEQ          84      /* Illegal byte sequence */
#define ERESTART        85      /* Interrupted system call should be restarted */
#define ESTRPIPE        86      /* Streams pipe error */
#define EUSERS          87      /* Too many users */
#define ENOTSOCK        88      /* Socket operation on non-socket */
#define EDESTADDRREQ    89      /* Destination address required */
#define EMSGSIZE        90      /* Message too long */
#define EPROTOTYPE      91      /* Protocol wrong type for socket */
#define ENOPROTOOPT     92      /* Protocol not available */
#define EPROTONOSUPPORT 93      /* Protocol not supported */
#define ESOCKTNOSUPPORT 94      /* Socket type not supported */
#define EOPNOTSUPP      95      /* Operation not supported on transport endpoint */
#define EPFNOSUPPORT    96      /* Protocol family not supported */
#define EAFNOSUPPORT    97      /* Address family not supported by protocol */
#define EADDRINUSE      98      /* Address already in use */
#define EADDRNOTAVAIL   99      /* Cannot assign requested address */
#define ENETDOWN        100     /* Network is down */
#define ENETUNREACH     101     /* Network is unreachable */
#define ENETRESET       102     /* Network dropped connection because of reset */
#define ECONNABORTED    103     /* Software caused connection abort */
#define ECONNRESET      104     /* Connection reset by peer */
#define ENOBUFS         105     /* No buffer space available */
#define EISCONN         106     /* Transport endpoint is already connected */
#define ENOTCONN        107     /* Transport endpoint is not connected */
#define ESHUTDOWN       108     /* Cannot send after transport endpoint shutdown */
#define ETOOMANYREFS    109     /* Too many references: cannot splice */
#define ETIMEDOUT       110     /* Connection timed out */
#define ECONNREFUSED    111     /* Connection refused */
#define EHOSTDOWN       112     /* Host is down */
#define EHOSTUNREACH    113     /* No route to host */
#define EALREADY        114     /* Operation already in progress */
#define EINPROGRESS     115     /* Operation now in progress */
#define ESTALE          116     /* Stale NFS file handle */
#define EUCLEAN         117     /* Structure needs cleaning */
#define ENOTNAM         118     /* Not a XENIX named type file */
#define ENAVAIL         119     /* No XENIX semaphores available */
#define EISNAM          120     /* Is a named type file */
#define EREMOTEIO       121     /* Remote I/O error */
#define EDQUOT          122     /* Quota exceeded */
 
#define ENOMEDIUM       123     /* No medium found */
#define EMEDIUMTYPE     124  
#define XEN_DOMCTL_MONITOR_OP_GET_CAPABILITIES  2
#define XEN_DOMCTL_MONITOR_OP_EMULATE_EACH_REP  3
#define XEN_DOMCTL_MONITOR_EVENT_WRITE_CTRLREG         0
#define XEN_DOMCTL_MONITOR_EVENT_MOV_TO_MSR            1
#define XEN_DOMCTL_MONITOR_EVENT_SINGLESTEP            2
#define XEN_DOMCTL_MONITOR_EVENT_SOFTWARE_BREAKPOINT   3
#define XEN_DOMCTL_MONITOR_EVENT_GUEST_REQUEST         4
#define XEN_DOMCTL_MONITOR_EVENT_DEBUG_EXCEPTION       5
#define XEN_DOMCTL_MONITOR_EVENT_CPUID                 6
#define XEN_DOMCTL_MONITOR_EVENT_PRIVILEGED_CALL       7
#define XEN_DOMCTL_MONITOR_EVENT_INTERRUPT             8
#define XEN_DOMCTL_MONITOR_EVENT_DESC_ACCESS           9
#define XEN_DOMCTL_MONITOR_EVENT_WRITE_CTRLREG                                0
#define XEN_DOMCTL_MONITOR_EVENT_MOV_TO_MSR                                   1
#define XEN_DOMCTL_MONITOR_EVENT_SINGLESTEP                                   2
#define XEN_DOMCTL_MONITOR_EVENT_SOFTWARE_BREAKPOINT                          3
#define XEN_DOMCTL_MONITOR_EVENT_GUEST_REQUEST                                4
#define XEN_DOMCTL_MONITOR_EVENT_DEBUG_EXCEPTION                              5
#define XEN_DOMCTL_MONITOR_EVENT_CPUID                                        6
#define XEN_DOMCTL_MONITOR_EVENT_PRIVILEGED_CALL                              7
#define XEN_DOMCTL_MONITOR_EVENT_INTERRUPT                                    8
#define XEN_DOMCTL_MONITOR_EVENT_DESC_ACCESS                                  9
#define XEN_DOMCTL_MONITOR_EVENT_GUEST_USERSPACE_EVENT                       10
#define XEN_DOMCTL_MONITOR_EVENT_INGUEST_PAGEFAULT 11
#define XEN_DOMCTL_MONITOR_EVENT_EMUL_UNIMPLEMENTED 12
#define XEN_DOMCTL_MONITOR_OP_ENABLE 13
#define VM_EVENT_X86_CR0    0	
#define VM_EVENT_X86_CR3    1	 
#define VM_EVENT_X86_CR4    2	 
#define VM_EVENT_X86_XCR0   3	


int ARRAY_SIZE(int array[]) 
{
  return 0 ;
}
    
struct low_{
 
};
 
struct monitor_ {
  int msr_bitmap;
  struct low_ low;
  int write_ctrlreg_mask[25] ;
  unsigned int write_ctrlreg_sync ;
  unsigned int write_ctrlreg_onchangeonly ;
  unsigned int write_ctrlreg_enabled ;
  int singlestep_enabled ;
  int inguest_pagefault_disabled ;
  int software_breakpoint_enabled ;
  int debug_exception_enabled ;
  int debug_exception_sync ;
  int cpuid_enabled ;
  int emul_unimplemented_enabled ;
  int descriptor_access_enabled ;
  
  
  _(invariant \this->singlestep_enabled <= INT_MAX)
  _(invariant \this->inguest_pagefault_disabled <= INT_MAX)
  _(invariant \this->cpuid_enabled <= INT_MAX)
};
 
struct arch_ {
  struct monitor_ monitor;
};
 
struct domain{
  struct arch_ arch;
  int monitor[25];
  unsigned int* low;
  unsigned int* hypervisor ;
  unsigned int* high ;
};

struct mov_to_cr_ {
  int pad1;
  int pad2;
  int index ;
  unsigned int sync;
  unsigned int onchangeonly ;
  int bitmask ;
};

struct mov_to_msr_ {
  unsigned int msr ;
  int onchangeonly ;
};

struct debug_exception_ {
  int sync ;
};

struct u_ {
  struct mov_to_cr_ mov_to_cr ;
  struct mov_to_msr_ mov_to_msr ;
  struct debug_exception_ debug_exception ;
};

struct xen_domctl_monitor_op {
  struct u_ u ;
  int op ;
};

 
int xzalloc_array(int x,int y)
{
  return 0;
}
 
int arch_monitor_init_domain(struct domain *d)
_(requires \thread_local(d))
_(requires \thread_local(&(d->arch.monitor)))
_(ensures \result >= 0)
_(writes &d->arch.monitor.msr_bitmap)

{
    if ( !d->arch.monitor.msr_bitmap )
        d->arch.monitor.msr_bitmap = xzalloc_array(d->arch.monitor.msr_bitmap,2) ; // removed struct 
 
    if ( !d->arch.monitor.msr_bitmap )
        return ENOMEM; //out of memory, defined it above
 
    return 0;
}
 
 
void xfree(int x)
{
 
}
 
void *memset(void *, int ,size_t);
 
void arch_monitor_cleanup_domain(struct domain *d)

_(requires \thread_local(&(d->arch.monitor)))

{
    xfree(d->arch.monitor.msr_bitmap); //made a function xfree
 
    memset(&d->arch.monitor, 0, sizeof(d->arch.monitor));
    memset(&d->monitor, 0, sizeof(d->monitor));
}
 
void ASSERT(int exp);

void BUILD_BUG_ON(bool x)
{}


static unsigned long *monitor_bitmap_for_msr(const struct domain *d, unsigned long *msr) // u32 changed to unsigned int 
_(requires \thread_local(d))
_(requires \thread_local(msr))
_(requires \thread_local(&(d->arch.monitor)))
_(ensures \result == NULL || \result == d->low || \result == d->hypervisor || \result == d-> high)
_(writes msr)

{
    ASSERT(d->arch.monitor.msr_bitmap && msr);
 
    switch ( *msr )
    {
    case 0x1fff: //correct syntax of cases
        BUILD_BUG_ON(sizeof(d->low) * 8 <= 0x1fff);
        return d->low;
 
    case 0x40000000 :
        BUILD_BUG_ON(
            sizeof(d->hypervisor) * 8 <= 0x1fff);
        *msr &= 0x1fff;
        return d->hypervisor;
 
    case 0xc0001fff:
        BUILD_BUG_ON(sizeof(d->high) * 8 <= 0x1fff);
        *msr &= 0x1fff;
        return d->high;
 
    default:
        return NULL;
    }
}
 
//void hvm_enable_msr_interception(struct domain *d,long msr);
void __set_bit (	unsigned long nr, unsigned long *addr);
void __clear_bit (	unsigned long nr, unsigned long *addr);
void test_bit (	unsigned long nr, unsigned long *addr);
 
//static void hvm_enable_msr_interception(struct domain *d, unsigned long msr)
  //       hvm_intercept_msr(v, msr, MSR_INTERCEPT_WRITE);
 //}
 
void hvm_enable_msr_interception(struct domain *d, unsigned long msr)
{}

static int monitor_enable_msr(struct domain *d, unsigned long msr, bool onchangeonly)
_(requires \thread_local(d))
_(requires \thread_local(&(d->arch.monitor)))
_(ensures \result <= 0)

{
    unsigned long *bitmap;
    unsigned long index = msr;
 
    if ( !d->arch.monitor.msr_bitmap )
        return -ENXIO;
 
    bitmap = monitor_bitmap_for_msr(d, &index);
 
    if ( !bitmap )
        return -EINVAL;
 
    __set_bit(index, bitmap);
 
    hvm_enable_msr_interception(d, msr);
 
    if ( onchangeonly )
        __set_bit(_(unchecked)(index + sizeof(struct monitor_msr_bitmap) * 8), bitmap);
    else
        __clear_bit(_(unchecked)(index + sizeof(struct monitor_msr_bitmap) * 8), bitmap);
 
    return 0;
}
 
static int monitor_disable_msr(struct domain *d, unsigned int msr)
_(requires \thread_local(d))
_(requires \thread_local(&(d->arch.monitor)))
_(ensures \result <= 0)
{
    unsigned long *bitmap;
 
    if ( !d->arch.monitor.msr_bitmap )
        return -ENXIO;
 
    bitmap = monitor_bitmap_for_msr(d, &msr);
 
    if ( !bitmap )
        return -EINVAL;
 
    __clear_bit(msr, bitmap);
 
    return 0;
}
 
/*bool monitored_msr(const struct domain *d, unsigned long msr)
{
    const unsigned long *bitmap;
 
    if ( !d->arch.monitor.msr_bitmap )
        return 0;
 
    bitmap = monitor_bitmap_for_msr(d, &msr);
 
    if ( !bitmap )
        return 0;
 
    return test_bit(msr, bitmap);
}
 
bool monitored_msr_onchangeonly(const struct domain *d, unsigned int msr)
{
    const unsigned long *bitmap;
 
    if ( !d->arch.monitor.msr_bitmap )
        return 0;
 
    bitmap = monitor_bitmap_for_msr(d, &msr);
 
    if ( !bitmap )
        return 0;
 
    return test_bit(msr + sizeof(struct monitor_msr_bitmap) * 8, bitmap);
}*/


//added this
struct xen_domctl_monitor_op{
  int event;
};


void domain_unpause(struct domain *d)
{}
void domain_pause(struct domain *d)
{}
void ASSERT_UNREACHABLE()
{}

bool unlikely(bool x)
{
}

unsigned int monitor_ctrlreg_bitmask(bool x);
unsigned int for_each_vcpu ( struct domain *d, struct vcpu *v );
unsigned int hvm_update_guest_cr(struct vcpu *v, int x) ;

struct hvm_funcs_{
  bool set_descriptor_access_exiting ;
  
}hvm_funcs;
bool monitored_msr(struct domain *d, unsigned int msr) ;
void hvm_set_icebp_interception(struct domain *d, bool requested_status) ;

int arch_monitor_domctl_event(struct domain *d,struct xen_domctl_monitor_op *mop)
_(requires \thread_local(d))
_(requires \thread_local(&(d->arch.monitor)))
_(requires \thread_local(mop))
_(requires \thread_local(&mop->u.mov_to_cr))
_(ensures \result <= 0)
{
    struct arch_ *ad = &d->arch;
    bool requested_status = (XEN_DOMCTL_MONITOR_OP_ENABLE == mop->op);
 
    switch ( mop->event )
    {
    case XEN_DOMCTL_MONITOR_EVENT_WRITE_CTRLREG:
    {
        unsigned int ctrlreg_bitmask;
        bool old_status;
        int temp = ARRAY_SIZE(ad->monitor.write_ctrlreg_mask) ;
        if ( unlikely(mop->u.mov_to_cr.index >= temp) )
            return -EINVAL;
 
        if ( unlikely(mop->u.mov_to_cr.pad1 || mop->u.mov_to_cr.pad2) )
            return -EINVAL;
 
        ctrlreg_bitmask = monitor_ctrlreg_bitmask(mop->u.mov_to_cr.index);
        _(assume \thread_local(&ad->monitor))
        old_status = !!(ad->monitor.write_ctrlreg_enabled & ctrlreg_bitmask);
 
        if ( !(old_status == requested_status) )
            return -EEXIST;
 
        domain_pause(d);
        _(assume \writable(&ad->monitor.write_ctrlreg_sync))
        if ( mop->u.mov_to_cr.sync )
            ad->monitor.write_ctrlreg_sync |= ctrlreg_bitmask;
        else
            ad->monitor.write_ctrlreg_sync &= ~ctrlreg_bitmask;
 
        if ( mop->u.mov_to_cr.onchangeonly )
            ad->monitor.write_ctrlreg_onchangeonly |= ctrlreg_bitmask;
        else
            ad->monitor.write_ctrlreg_onchangeonly &= ~ctrlreg_bitmask;
 
        if ( requested_status )
        {
            _(assume \writable(&ad->monitor.write_ctrlreg_mask[mop->u.mov_to_cr.index]))
            ad->monitor.write_ctrlreg_mask[mop->u.mov_to_cr.index] = mop->u.mov_to_cr.bitmask;
            ad->monitor.write_ctrlreg_enabled |= ctrlreg_bitmask;
        }
        else
        {
            _(assume \writable(&ad->monitor.write_ctrlreg_mask[mop->u.mov_to_cr.index]))
            ad->monitor.write_ctrlreg_mask[mop->u.mov_to_cr.index] = 0;
            ad->monitor.write_ctrlreg_enabled &= ~ctrlreg_bitmask;
        }
 
        if ( VM_EVENT_X86_CR3 == mop->u.mov_to_cr.index ||
             VM_EVENT_X86_CR4 == mop->u.mov_to_cr.index )
        {
            struct vcpu *v;
            /* Latches new CR3 or CR4 mask through CR0 code. */
            for_each_vcpu ( d, v );
                hvm_update_guest_cr(v, 0);
        }
 
        domain_unpause(d);
 
        break;
    }
 
    case XEN_DOMCTL_MONITOR_EVENT_MOV_TO_MSR:
    {
        bool old_status;
        int rc;
        _(assume \thread_local(&mop->u.mov_to_msr))
        unsigned int msr = mop->u.mov_to_msr.msr;
 
        domain_pause(d);
 
        old_status = monitored_msr(d, msr);
 
        if ( !(old_status == requested_status) )
        {
            domain_unpause(d);
            return -EEXIST;
        }
 
        if ( requested_status )
            rc = monitor_enable_msr(d, msr, mop->u.mov_to_msr.onchangeonly);
        else
            rc = monitor_disable_msr(d, msr);
 
        domain_unpause(d);
 
        return rc;
    }
 
    case XEN_DOMCTL_MONITOR_EVENT_SINGLESTEP:
    {
        _(assume \thread_local(&ad->monitor))
        bool old_status = ad->monitor.singlestep_enabled;
 
        if ( !(old_status == requested_status) )
            return -EEXIST;
 
        domain_pause(d);
        _(assume \writable(&ad->monitor.singlestep_enabled))
        ad->monitor.singlestep_enabled = requested_status;
        domain_unpause(d);
        break;
    }
 
    case XEN_DOMCTL_MONITOR_EVENT_INGUEST_PAGEFAULT:
    {
        _(assume \thread_local(&ad->monitor))
        bool old_status = ad->monitor.inguest_pagefault_disabled;
 
        if ( !(old_status == requested_status) )
            return -EEXIST;
 
        domain_pause(d);
        _(assume \writable(&ad->monitor.inguest_pagefault_disabled))
        ad->monitor.inguest_pagefault_disabled = requested_status;
        domain_unpause(d);
        break;
    }
 
    case XEN_DOMCTL_MONITOR_EVENT_DESC_ACCESS:
    {
        _(assume \thread_local(&ad->monitor))
        bool old_status = ad->monitor.descriptor_access_enabled;
        struct vcpu *v;
 
        if ( !(old_status == requested_status) )
            return -EEXIST;
        _(assume \thread_local(&hvm_funcs))
        if ( !hvm_funcs.set_descriptor_access_exiting )
            return -EOPNOTSUPP;
 
        domain_pause(d);
        _(assume \writable(&ad->monitor.descriptor_access_enabled))
        ad->monitor.descriptor_access_enabled = requested_status;
 

 
        domain_unpause(d);
        break;
    }
 
    case XEN_DOMCTL_MONITOR_EVENT_SOFTWARE_BREAKPOINT:
    {
        _(assume \thread_local(&ad->monitor))
        bool old_status = ad->monitor.software_breakpoint_enabled;
 
        if ( !(old_status == requested_status) )
            return -EEXIST;
 
        domain_pause(d);
        _(assume \writable(&ad->monitor.software_breakpoint_enabled))
        ad->monitor.software_breakpoint_enabled = requested_status;
        domain_unpause(d);
        break;
    }
 
    case XEN_DOMCTL_MONITOR_EVENT_DEBUG_EXCEPTION:
    {
        _(assume \thread_local(&ad->monitor))
        bool old_status = ad->monitor.debug_exception_enabled;
 
        if ( !(old_status == requested_status) )
            return -EEXIST;
 
        domain_pause(d);
        _(assume \writable(&ad->monitor.debug_exception_enabled))
        ad->monitor.debug_exception_enabled = requested_status;
        _(assume \thread_local(&mop->u.debug_exception))
        ad->monitor.debug_exception_sync = requested_status ?mop->u.debug_exception.sync :0;
 
        hvm_set_icebp_interception(d, requested_status);
 
        domain_unpause(d);
        break;
    }
 
    case XEN_DOMCTL_MONITOR_EVENT_CPUID:
    {
        _(assume \thread_local(&ad->monitor))
        bool old_status = ad->monitor.cpuid_enabled;
 
        if ( !(old_status == requested_status) )
            return -EEXIST;
 
        domain_pause(d);
        _(assume \writable(&ad->monitor.cpuid_enabled))
        ad->monitor.cpuid_enabled = requested_status;
        domain_unpause(d);
        break;
    }
 
    case XEN_DOMCTL_MONITOR_EVENT_EMUL_UNIMPLEMENTED:
    {
        _(assume \thread_local(&ad->monitor))
        bool old_status = ad->monitor.emul_unimplemented_enabled;
 
        if ( !(old_status == requested_status) )
            return -EEXIST;
 
        domain_pause(d);
        _(assume \writable(&ad->monitor.emul_unimplemented_enabled))
        ad->monitor.emul_unimplemented_enabled = requested_status;
        domain_unpause(d);
        break;
    }
 
    default:
        /*
         * Should not be reached unless arch_monitor_get_capabilities() is
         * not properly implemented.
         */
        ASSERT_UNREACHABLE();
        return -EOPNOTSUPP;
    }
 
    return 0;
}



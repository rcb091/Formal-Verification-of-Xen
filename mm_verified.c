#include <vcc.h>
#include <xen/init.h>
#include <xen/kernel.h>
#include <xen/lib.h>
#include <xen/mm.h>
#include <xen/domain.h>
#include <xen/sched.h>
#include <xen/err.h>
#include <xen/perfc.h>
#include <xen/irq.h>
#include <xen/softirq.h>
#include <xen/domain_page.h>
#include <xen/event.h>
#include <xen/iocap.h>
#include <xen/guest_access.h>
#include <xen/pfn.h>
#include <xen/vmap.h>
#include <xen/xmalloc.h>
#include <xen/efi.h>
#include <xen/grant_table.h>
#include <xen/hypercall.h>
#include <asm/paging.h>
#include <asm/shadow.h>
#include <asm/page.h>
#include <asm/flushtlb.h>
#include <asm/io.h>
#include <asm/ldt.h>
#include <asm/x86_emulate.h>
#include <asm/e820.h>
#include <asm/hypercall.h>
#include <asm/shared.h>
#include <asm/mem_sharing.h>
#include <public/memory.h>
#include <public/sched.h>
#include <xsm/xsm.h>
#include <xen/trace.h>
#include <asm/setup.h>
#include <asm/fixmap.h>
#include <asm/io_apic.h>
#include <asm/pci.h>
#include <asm/guest.h>
#include <asm/hvm/ioreq.h>

#include <asm/hvm/grant_table.h>
#include <asm/pv/domain.h>
#include <asm/pv/grant_table.h>
#include <asm/pv/mm.h>

#ifdef CONFIG_PV
#include "pv/mm.h"
#endif


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
_(ghost struct domain{})
_(ghost int get_page_from_mfn(int a,struct domain *b))
_(ghost int _get_page_type(struct page_info *a,unsigned long b, int x))
_(ghost int typeof(int a))
_(ghost int likely(int a))
_(ghost int unlikely(int a))
_(ghost int arch_fetch_and_add(int a, int b))
_(ghost struct page_info{int linear_pt_count;})
_(ghost struct p2m_get_hostp2m{struct domain *a;})
_(ghost long max_mapped_pfn)
_(ghost unsigned long arch_get_max_pfn(struct domain *a))
_(ghost bool is_hvm_domain(struct domain *a))
_(ghost int parse_bool(char *a, char *b))
_(ghost int strcmp(char *a,char *b))
_(ghost void put_page(struct page_info *a))
_(ghost struct page_info *mfn_to_page(int a))
_(ghost bool test_and_clear_bit(int a, int b))

static int parse_mmio_relax(const char *s)
_(requires \true)
_(ensures \result == 0 || \result == -EINVAL)
_(requires \thread_local(s)) 


{
  int opt_mmio_relax;
    if ( !*s)
        opt_mmio_relax = 1;
     
    else
        opt_mmio_relax = parse_bool(s, NULL);
    if ( opt_mmio_relax < 0 && strcmp(s, "all") )
    {
        opt_mmio_relax = 0;
        return -EINVAL;
    }

    return 0;
}

unsigned long domain_get_maximum_gpfn(struct domain *d)
_(requires \true)
_(requires \thread_local(d))
_(ensures d!=0)
_(writes d)
{
 
    if ( is_hvm_domain(d) )
      // return p2m_get_hostp2m(d)->max_mapped_pfn;
   
   
    return (arch_get_max_pfn(d));
    _(assert d != 0)
}

static void dec_linear_entries(struct page_info *pg)
_(requires \true)
_(requires \thread_local(pg))
_(ensures pg != NULL)
{
  int linear_pt_count;
  int oc = typeof(pg->linear_pt_count);
_(assume oc >0)
    //oc = arch_fetch_and_add(linear_pt_count, -1);
    _(assert oc > 0);
 
}

static void dec_linear_uses(struct page_info *pg)
_(requires \true)
_(requires \thread_local(pg))
_(ensures pg != NULL)

{
  int linear_pt_count;
    int oc = typeof(linear_pt_count);
  _(assume oc >0)
   // oc = arch_fetch_and_add(linear_pt_count, 1);
    _(assert oc != 0)
}


static int get_page_and_type_from_mfn(
    int mfn, unsigned long type, struct domain *d,
    int partial, int preemptible)
    _(requires \true)
    _(requires \thread_local(d))
    _(ensures type>=0 && d != 0)
{
    struct page_info *page = mfn_to_page(mfn);
    struct page_info *current;
    int rc;

    if ( likely(partial >= 0) &&
         unlikely(!get_page_from_mfn(mfn, d)) )
        return -EINVAL;

    rc = _get_page_type(page, type, preemptible);

    if ( unlikely(rc) && partial >= 0 &&
         (!preemptible || page != current) )
        put_page(page);
        
  _(assert rc >=0 || rc <0)
    return rc;
}
void ASSERT_UNREACHABLE()
_(requires \true)
{}

void page_set_owner(struct page_info *a, char *b)
_(requires \true) 
{}

void free_domheap_page(struct page_info *a)
_(requires \true)
{}

void free_shared_domheap_page(struct page_info *page)
_(requires \true)
_(requires \thread_local(page))
_(writes page)
_(ensures page != 0)
{
  int _PGC_allocated, _PGC_xen_heap,count_info;
    if ( test_and_clear_bit(_PGC_allocated, count_info) )
        put_page(page);
    if ( !test_and_clear_bit(_PGC_xen_heap, count_info) )
        ASSERT_UNREACHABLE();
    //page->u.inuse.type_info = 0;
    
    page_set_owner(page, NULL);
    free_domheap_page(page);
    _(assert page != NULL)
}
int page_get_owner(struct page_info *a)
_(requires \true)
{}

int shadow_mode_enabled(int a)
_(requires \true)
{return 1;}

void page_set_tlbflush_timestamp(struct page_info *a)
_(requires \true)
{}

static inline void set_tlbflush_timestamp(struct page_info *page)
_(requires \true)
_(requires \thread_local(page))
_(ensures page != 0)
_(writes page)
{
  int PGC_page_table,count_info;
    /*
     * Record TLB information for flush later. We do not stamp page tables
     * when running in shadow mode:
     *  1. Pointless, since it's the shadow pt's which must be tracked.
     *  2. Shadow mode reuses this field for shadowed page tables to store
     *     flags info -- we don't want to conflict with that.
     */

    if ( !(count_info & PGC_page_table) ||
         !shadow_mode_enabled(page_get_owner(page)) )
        page_set_tlbflush_timestamp(page);
        _(assert page != NULL)
}

bool writeable()
_(requires \true)
{}

void put_page_and_type(struct page_info *a)
{
  
}

static void put_data_page(struct page_info *page, bool writeable)
_(requires \true)
_(requires \thread_local(page))
_(ensures writeable>=0 && page != 0)
{
    if ( writeable )
        put_page_and_type(page);
    else
        put_page(page);
        
      _(assert page!=NULL)
}

int _put_page_type(struct page_info *a,int b, char *c)
_(requires \true)
{ return 0;}

void put_page_type(struct page_info *page)
_(requires \true)
_(requires \thread_local(page))
_(ensures page != NULL)
{
    int rc = _put_page_type(page,0, NULL);
    _(assume rc == 0)
    (void)rc;
    _(assert rc >= 0)
}

int _get_page_type(struct page_info *a,unsigned long b,int c)
_(requires \true)
{}

int get_page_type(struct page_info *page, unsigned long type)
_(requires \true)
_(requires \thread_local(page))
_(ensures page !=0 && type>=0)
{
    int rc = _get_page_type(page, type, 0);
  _(assume rc >0)
    if ( likely(rc == 0) )
        return 1;
    _(assert rc != -EINTR && rc != -ERESTART)
    return 0;
}

int put_page_type_preemptible(struct page_info *page)
_(requires \true)
_(requires \thread_local(page))
_(ensures page !=0)
_(writes page)
{
    return _put_page_type(page, 1, NULL);
  _(assert page!=NULL)
}

int get_page_type_preemptible(struct page_info *page, unsigned long type)
_(requires \true)
_(requires \thread_local(page)) 
_(ensures page !=0)

{
  int current,old_guest_table;
_(assume current !=old_guest_table)
    _(assert current != old_guest_table );

    return _get_page_type(page, type, 1);
}



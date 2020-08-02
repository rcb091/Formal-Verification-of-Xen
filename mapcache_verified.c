#include <sys/resource.h>
#include <vcc.h>
#include <xen/hvm/params.h>
#include <sys/mman.h>
#include <limits.h>

#define MCACHE_BUCKET_SHIFT 16
#define MCACHE_MAX_SIZE     (1UL<<31) /* 2GB Cap */
#define MCACHE_BUCKET_SHIFT 20
#define MCACHE_MAX_SIZE     (1UL<<35) /* 32GB Cap */
#define MCACHE_BUCKET_SIZE (1UL << 20)
#define XC_PAGE_SIZE (1UL << 16)

#define NON_MCACHE_MEMORY_SIZE (80 * 1024 * 1024)
typedef unsigned char uint8_t;
typedef unsigned long hwaddr;

struct node
{
  unsigned int active;
  _(invariant active<UINT_MAX)
};

unsigned int lck;
unsigned int apic_read(unsigned int lvr);
unsigned int GET_APIC_VERSION(unsigned int);
struct node{
  unsigned int x86_vendor;
  unsigned int x86;
  };
typedef struct MapCacheEntry {
    unsigned capacity,len;
    hwaddr paddr_index;
    char *vaddr_base;
    _(invariant len<capacity)
    _(invariant vaddr_base[len] =='\0')
    _(invariant \mine((char[capacity])vaddr_base))
    unsigned long *valid_mapping;
    uint8_t lock;
    hwaddr size;
     _(invariant size<capacity)
    struct MapCacheEntry *next;
} MapCacheEntry;

typedef struct MapCacheRev {
  unsigned capacity,len;
    uint8_t *vaddr_req;
    _(invariant len<capacity)
    _(invariant vaddr_req[len] =='\0')
    _(invariant \mine((char[capacity])vaddr_req))
    hwaddr paddr_index;
    _(invariant paddr_index<capacity)
    hwaddr size;
    _(invariant size<capacity)
} MapCacheRev;

typedef struct MapCache {
    MapCacheEntry *entry;
    _(invariant entry!=NULL)
    unsigned long nr_buckets;
    /* For most cases (>99.9%), the page address is the same. */
    MapCacheEntry *last_entry;
    unsigned long max_mcache_size;
    unsigned int mcache_bucket_shift;
    _(invariant mcache_bucket_shift<UINT_MAX)
    void *opaque;
} MapCache;


static inline void mapcache_lock(void)
          _(requires \true)
{
    unsigned int lck;
    lck=_(unchecked)(lck+1);
    return;
}
static inline void mapcache_unlock(void)
            _(requires \true)
{
    unsigned int lck;
    lck=_(unchecked)(lck-1);
    return;
}
static inline uint8_t *xen_map_cache_unlocked(hwaddr phys_addr,hwaddr size,uint8_t *lock) 
                                _(requires \true)
                                _(writes lock)
                                _(ensures lock!=NULL)
{
  uint8_t *ptr=NULL; 
  lock=ptr;
  return ptr;
}
unsigned long find_next_zero_bit(unsigned int end,unsigned int size)
                            _(requires \true)
                            _(requires end<=UINT_MAX)
                            _(ensures end>=0 && size>=0)
                          
{
    int cnt=0;
    unsigned long *addr;
    for(unsigned long i=0;i<end;i++)
    {
      cnt=_(unchecked)(cnt + 1);
    }
    return (1UL<<MCACHE_BUCKET_SHIFT);
}
unsigned int xen_invalidate_map_cache_entry_unlocked(unsigned int res)
            _(requires \true)
            _(ensures res>=0)
{
    unsigned int lvr, version;
    struct node boot_cpu_data;
   /* AMD systems use old APIC versions, so check the CPU */
    unsigned int X86_VENDOR_AMD;
    _(writes &boot_cpu_data.x86_vendor);
    _(writes &boot_cpu_data.x86);
    if (boot_cpu_data.x86_vendor == X86_VENDOR_AMD &&
        boot_cpu_data.x86 >= 0xf) 
        {
          res=1;
          return res;
        }  
        _(ghost unsigned int APIC_LVR)
    lvr = apic_read(APIC_LVR);
    version = GET_APIC_VERSION(lvr);
    if(version >= 0x14) res=0;
    return res;
}
static void xen_ram_addr_from_mapcache(void)
{
  struct node apic_pm_state;
_(writes &apic_pm_state.active)
_(ensures apic_pm_state.active == 1)
    apic_pm_state.active = 1;
}
static inline unsigned int test_bits(unsigned long  nr, unsigned long  size,unsigned long *addr,unsigned int result)
                                                                    _(requires \true)
                                                                    _(requires size>=0)
                                                                    _(ensures result>=0)
                                                                    
{
    unsigned long res = find_next_zero_bit(_(unchecked)(size + nr), nr);
    if (res >= _(unchecked)(nr + size))
    result=1;
    else
    result=0;
    return result;
}
static void xen_map_cache_unlocked(hwaddr phys_addr, hwaddr size,uint8_t lock,int *arr)
                                      _(requires \true)
                                      _(requires \thread_local(arr))
                                    
{
    MapCacheEntry *entry, *pentry = NULL;
    hwaddr address_index;
    hwaddr address_offset;
    hwaddr cache_size = size;
    hwaddr test_bit_size;
    int translated =0;
    tryagain:
    address_index  = _(unchecked)(phys_addr >> MCACHE_BUCKET_SHIFT);
    address_offset = _(unchecked)(phys_addr & (MCACHE_BUCKET_SIZE));
    if (size) {
        test_bit_size = _(unchecked)(size + (phys_addr & (XC_PAGE_SIZE - 1)));
        _(assert XC_PAGE_SIZE!=0)
        if (test_bit_size % XC_PAGE_SIZE) {
            test_bit_size = _(unchecked)(test_bit_size+XC_PAGE_SIZE - (test_bit_size % XC_PAGE_SIZE));
        }
    }
    else {
        test_bit_size = XC_PAGE_SIZE;
    }
}   
static inline void xen_map_cache(hwaddr phys_addr, hwaddr size,uint8_t *lock)
                       _(requires \true)
                       _(requires size>=0)
                        _(requires \thread_local(lock))
{
    uint8_t *p;
    mapcache_lock();
    lock=p;
    //p=xen_map_cache_unlocked(phys_addr,size,lock);
    mapcache_unlock();
}

void xen_invalidate_map_cache_entry(uint8_t *buffer)
                    _(requires \true)
                    _(requires \thread_local(buffer))
{
    mapcache_lock();
    mapcache_unlock();
}                          


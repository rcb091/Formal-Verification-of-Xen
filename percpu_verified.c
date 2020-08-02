#define NR_CPUS 1
unsigned long __per_cpu_offset[NR_CPUS];
extern char __per_cpu_start[], __per_cpu_data_end[];

/*
 * Force uses of per_cpu() with an invalid area to attempt to access the
 * middle of the non-canonical address space resulting in a #GP, rather than a
 * possible #PF at (NULL + a little) which has security implications in the
 * context of PV guests.
 */
#define INVALID_PERCPU_AREA ((unsigned long)0x8000000000000000L-(unsigned long)__per_cpu_start)
#define PERCPU_ORDER get_order_from_bytes((unsigned)(((unsigned)__per_cpu_data_end - (unsigned)__per_cpu_start)))
//typedef unsigned long paddr_t;
void call_rcu(struct rcu_head *head, 
              void (*func)(struct rcu_head *head));
_(pure) static inline unsigned int get_order_from_bytes(unsigned long size)
_(requires \true)
_(ensures \result>=0)
{
    unsigned int order;

    size = _(unchecked)(size - 1) >> 12;
    for ( order = 0; size; order = _(unchecked)(order + 1) )
    _(invariant size>=0)
        size >>= 1;

    return order;
}
void percpu_init_areas(void)
_(requires \true)
{
    unsigned int cpu;

    for ( cpu = 1; cpu < NR_CPUS; cpu++ )
        __per_cpu_offset[cpu] = _(unchecked)(unsigned __int32)(INVALID_PERCPU_AREA);
}
void free_xenheap_pages(void *v, unsigned int order);
void *alloc_xenheap_pages(unsigned int order, unsigned int memflags);
void *memset(void *, int, size_t);
static int init_percpu_area(unsigned int cpu)
_(writes \array_range(__per_cpu_offset, NR_CPUS))
_(requires \thread_local_array(__per_cpu_offset, NR_CPUS))
_(requires \thread_local(__per_cpu_data_end))
_(requires \thread_local(__per_cpu_start))
_(ensures \result==0 || \result==-1)
{
    char *p;
    _(assume cpu>=0 && cpu<NR_CPUS)
    if ( _(unchecked)(__per_cpu_offset[cpu]) != _(unchecked)((unsigned)INVALID_PERCPU_AREA) )
        return 0;
        unsigned int lim=_(unchecked)PERCPU_ORDER;
    //_(assume PERCPU_ORDER>=0 && PERCPU_ORDER<0xffffffff)
    if ( (p = alloc_xenheap_pages(lim, 0)) == NULL )
        return -1;
    _(assume (p - __per_cpu_start)>0 && (p - __per_cpu_start)<INT_MAX)
    memset(p, 0, (unsigned)(lim));
    __per_cpu_offset[cpu] =(unsigned)(p - __per_cpu_start);

    return 0;
}
struct rcu_head {
    struct rcu_head *next;
    void (*func)(struct rcu_head *head);
};
struct free_info {
    unsigned int cpu;
    struct rcu_head rcu;
}f;

//static DEFINE_PER_CPU(struct free_info, free_info);
struct free_info* container_of(struct rec_head* p,struct free_info* q,struct rcu_head* r){return q;}

static void _free_percpu_area(struct rcu_head *head)
_(requires \true)
{
    struct rec_head *rh = &f.rcu;
    struct free_info *info = container_of(rh,&f, &f.rcu);
    _(assume \thread_local(info))
    _(assume \thread_local(__per_cpu_data_end))
    _(assume \thread_local(__per_cpu_start))
    _(assume \thread_local_array(__per_cpu_offset, NR_CPUS))
    unsigned int cpu = info->cpu;
    _(assume cpu>=0 && cpu<NR_CPUS)
    char *p = __per_cpu_start + __per_cpu_offset[cpu];
    _(assume \writable(__per_cpu_offset))
    free_xenheap_pages(p, _(unchecked)PERCPU_ORDER);
    __per_cpu_offset[cpu] = _(unchecked)(unsigned __int32)(INVALID_PERCPU_AREA);
}

struct free_info *per_cpu(struct free_info* p,unsigned int cpu){return p;}

static void free_percpu_area(unsigned int cpu)
_(requires \true)
{
    struct free_info *info = per_cpu(&f, cpu);
    _(assume \writable(&info->cpu))
    info->cpu = cpu;
    call_rcu(&info->rcu, _free_percpu_area);
}
static inline int notifier_from_errno(int err)
{
    return 0x8000 | _(unchecked)-err;
}
int park_offline_cpus = 1;
#define CPU_UP_PREPARE   (0x0001 | 0x0000)
#define CPU_UP_CANCELED  (0x0002 | 0x8000)
#define CPU_DEAD         (0x0008 | 0x8000)
#define CPU_REMOVE       (0x0009 | 0x8000)
static int cpu_percpu_callback(
    struct notifier_block *nfb, unsigned long action, void *hcpu)
_(requires \thread_local(&park_offline_cpus))
_(requires \thread_local(__per_cpu_data_end))
_(requires \thread_local(__per_cpu_start))
_(writes \array_range(__per_cpu_offset,NR_CPUS)) 
{
    unsigned int cpu = (unsigned long)hcpu;
    int rc = 0;

    switch ( action )
    {
    case CPU_UP_PREPARE:
        rc = init_percpu_area(cpu);
        break;
    case CPU_UP_CANCELED:
    case CPU_DEAD:
        if ( !park_offline_cpus )
            free_percpu_area(cpu);
        break;
    case CPU_REMOVE:
        if ( park_offline_cpus )
            free_percpu_area(cpu);
        break;
    }

    return !rc ? 0x0000 : notifier_from_errno(rc);
}

static struct notifier_block cpu_percpu_nfb = {
    //.notifier_call = cpu_percpu_callback,
    //.priority = 100 /* highest priority */
};
void register_cpu_notifier(struct notifier_block *nb);
static int  percpu_presmp_init(void)
_(requires \true)
_(ensures \result==0)
{
    register_cpu_notifier(&cpu_percpu_nfb);

    return 0;
}
//presmp_initcall(percpu_presmp_init);

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
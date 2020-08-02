#include <vcc.h>
#define INT_MIN -2147483648

void __bug(char *file, int line)
{
    //printf("*** HVMLoader bug at %s:%d\n", file, line);
    //crash();
}

struct microcode_header_intel {
    unsigned int hdrver;
    unsigned int rev;
    union {
        struct {
            unsigned short year;
            unsigned char day;
            unsigned char month;
            unsigned int date;
        };
    };
    unsigned int sig;
    unsigned int cksum;
    unsigned int ldrver;
    unsigned int pf;
    unsigned int datasize;
    unsigned int totalsize;
    unsigned int reserved[3];
};

struct microcode_intel {
    struct microcode_header_intel hdr;
    unsigned int bits[0];
};


struct extended_signature {
    unsigned int sig;
    unsigned int pf;
    unsigned int cksum;
};

struct extended_sigtable {
    unsigned int count;
    unsigned int cksum;
    unsigned int reserved[3];
    struct extended_signature sigs[0];
};

struct cpuinfo_x86 {
    unsigned char x86;
    unsigned char x86_vendor;
    unsigned char x86_model;
    unsigned char x86_mask;
    int cpuid_level;
    unsigned long extended_cpuid_level;
    unsigned int x86_capability[(1000 + 1)];
    char x86_vendor_id[16];
    char x86_model_id[64];
    int x86_cache_size;
    int x86_cache_alignment;
    unsigned long x86_max_cores;
    unsigned long booted_cores;
    unsigned long x86_num_siblings;
    unsigned long apicid;
    unsigned long phys_proc_id;
    unsigned long cpu_core_id;
    unsigned long compute_unit_id;
    unsigned short x86_clflush_size;
} __cacheline_aligned;





typedef unsigned short domid_t;

typedef struct
{
  unsigned long long last_update_time;
  unsigned long long start_time;
  unsigned long long runnable_start_time;
  unsigned long long blocked_start_time;
  unsigned long long ns_since_boot;
  unsigned long long ns_oncpu_since_boot;

  int runnable_at_last_update;
  int runnable;


  int in_use;
  domid_t id;
  char name[256];
} _domain_info;

typedef struct
{
  struct
  {


    unsigned long long ns_gotten[32];
    unsigned long long ns_allocated[32];
    unsigned long long ns_waiting[32];

    unsigned long long ns_blocked[32];
    unsigned long long switchin_count[32];
    unsigned long long io_count[32];
    unsigned long long ns_passed;
    unsigned long long timestamp;
    unsigned long long lost_records;
    unsigned long long flip_free_periods;
  } qdata[100];

  _domain_info domain_info[32];


  int next_datapoint;
  int ncpu;
  int structlen;


  int measurement_frequency;

} _new_qos_data;

_new_qos_data *cpu_data;






static int collect_cpu_info(unsigned int cpu_num, struct cpu_signature *csig)
{
  _(assume \thread_local(&cpu_data))
    struct cpuinfo_x86 *c = (struct cpuinfo_x86*)&cpu_data[cpu_num];
        
    _(assume \thread_local(c))
    unsigned long long msr_content;

    char * ptrsd=(char*)csig;
    _(assume \writable(ptrsd))
    int ptrid=0;
    for(;ptrid < sizeof(*csig); ptrid++){
      *ptrsd=0;
    }


    if ( (c->x86_vendor != 1) || (c->x86 < 6) )
    {

        return -1;
    }

    _(assume \writable(&csig->sig))
    csig -> sig = 0;//cpuid_eax(0x00000001);

    if ( (c->x86_model >= 5) || (c->x86 > 6) )
    {

        //rdmsrl(0x00000017, msr_content);
        csig->pf = 1 << ((msr_content >> 50) & 7);
    }

    //wrmsrl(0x0000008b, 0x0ULL);

    //cpuid_eax(1);


    //rdmsrl(0x0000008b, msr_content);
    csig->rev = (unsigned long)(msr_content >> 32);




    return 0;
}

struct cpu_signature {
    unsigned int sig;
    unsigned int pf;
    unsigned int rev;
};
struct equiv_cpu_entry {
    unsigned long installed_cpu;
    unsigned long fixed_errata_mask;
    unsigned long fixed_errata_compare;
    unsigned short equiv_cpu;
    unsigned short reserved;
};
struct microcode_amd {
    void *mpb;
    size_t mpb_size;
    struct equiv_cpu_entry *equiv_cpu_table;
    size_t equiv_cpu_table_size;
};
struct ucode_cpu_info {
    struct cpu_signature cpu_sig;
    union {
        struct{
          struct microcode_intel *mc_intel;
          struct microcode_amd *mc_amd;
          void *mc_valid;
        };
    } mc;
}ucode_cpu_info;

static inline int microcode_update_match(
    unsigned int cpu_num, const struct microcode_header_intel *mc_header,
    int sig, int pf)
{
    struct ucode_cpu_info *uci = &ucode_cpu_info;
    _(assume (uci->cpu_sig.sig) <= INT_MAX && (uci->cpu_sig.sig) >= INT_MIN)
    _(assume (uci->cpu_sig.pf) <= INT_MAX && (uci->cpu_sig.pf) >= INT_MIN)
    _(assume \thread_local(&(uci->cpu_sig)))
    _(assume \thread_local(mc_header))
    return ((((sig) == (__int32) (uci->cpu_sig.sig)) && (((pf) & (__int32)(uci->cpu_sig.pf)) || (((pf) == 0) && ((uci->cpu_sig.pf) == 0)))) &&
            (mc_header->rev > uci->cpu_sig.rev));
}

static int microcode_sanity_check(void *mc)
{
    struct microcode_header_intel *mc_header = mc;
    struct extended_sigtable *ext_header = NULL;
    struct extended_signature *ext_sig;
    unsigned long total_size, data_size, ext_table_size;
    unsigned int ext_sigcount = 0, i;
    unsigned long sum, orig_sum;

    _(assume \thread_local(&((struct microcode_intel *)mc_header)->hdr))
    total_size = ((struct microcode_intel *)mc_header)->hdr.totalsize ?((struct microcode_intel *)mc_header)->hdr.totalsize :((2000) + (sizeof(struct microcode_header_intel)));
    data_size = (((struct microcode_intel *)mc_header)->hdr.datasize ?((struct microcode_intel *)mc_header)->hdr.datasize : (2000));
    if ( (_(unchecked)(data_size + (sizeof(struct microcode_header_intel)))) > total_size )
    {



        return -101;
    }

    if ( (mc_header->ldrver != 1) || (mc_header->hdrver != 1) )
    {



        return -101;
    }
    ext_table_size = _(unchecked)(total_size - ((sizeof(struct microcode_header_intel)) + data_size));
    if ( ext_table_size )
    {
        if ( (ext_table_size < (sizeof(struct extended_sigtable))) ||
             ((ext_table_size - (sizeof(struct extended_sigtable))) % (sizeof(struct extended_signature))) )
        {



            return -101;
        }
//        ext_header = mc + (sizeof(struct microcode_header_intel)) + data_size;
        _(assume \thread_local(ext_header))
        if ( ext_table_size != _(unchecked)((ext_header)->count * (sizeof(struct extended_signature)) + (sizeof(struct extended_sigtable))) )
        {



            return -14;
        }
        ext_sigcount = ext_header->count;
    }


    if ( ext_table_size )
    {
        unsigned long ext_table_sum = 0;
        unsigned long *ext_tablep = (unsigned long *)ext_header;

        i = ext_table_size / (sizeof(unsigned long));
        while ( i-- )
            ext_table_sum += ext_tablep[i];
        if ( ext_table_sum )
        {



            return -101;
        }
    }


    orig_sum = 0;
    i = _(unchecked)((sizeof(struct microcode_header_intel)) + data_size) / (sizeof(unsigned long));
    while ( _(unchecked)(i--) )
    {
        _(assume \thread_local(&((unsigned long *)mc)[i]))
        orig_sum = _(unchecked)(orig_sum + ((unsigned long *)mc)[i]);
    }
    if ( orig_sum )
    {


        return -101;
    }
    if ( !ext_table_size )
        return 0;

    for ( i = 0; i < ext_sigcount; i++ )
    {
        ext_sig = (struct extended_signature*)(ext_header + (sizeof(struct extended_sigtable)) +
            (sizeof(struct extended_signature)) * i);
        sum = orig_sum
            - (mc_header->sig + mc_header->pf + mc_header->cksum)
            + (ext_sig->sig + ext_sig->pf + ext_sig->cksum);
        if ( sum )
        {


            return -101;
        }
    }
    return 0;
}

static int get_matching_microcode(const void *mc, unsigned int cpu)
{
    struct ucode_cpu_info *uci = &ucode_cpu_info;
    const struct microcode_header_intel *mc_header = mc;
    const struct extended_sigtable *ext_header;
    _(assume \thread_local(&((struct microcode_intel *)mc_header)->hdr))
    unsigned long total_size = ((struct microcode_intel *)mc_header)->hdr.totalsize ?((struct microcode_intel *)mc_header)->hdr.totalsize :((2000) + (sizeof(struct microcode_header_intel)));
    int ext_sigcount, i;
    struct extended_signature *ext_sig;
    void *new_mc;

    _(assume (mc_header->sig) <= INT_MAX && (mc_header->sig) >= INT_MIN)
    _(assume (mc_header->pf) <= INT_MAX && (mc_header->pf) >= INT_MIN)
    if ( microcode_update_match(cpu, mc_header,
                                (int)mc_header->sig, (int)mc_header->pf) )
        goto find;

    if ( total_size <= _(unchecked)((((struct microcode_intel *)mc_header)->hdr.datasize ?((struct microcode_intel *)mc_header)->hdr.datasize : (2000)) + (sizeof(struct microcode_header_intel))) )
        return 0;

//    ext_header = mc + (((struct microcode_intel *)mc_header)->hdr.datasize ?((struct microcode_intel *)mc_header)->hdr.datasize : (2000)) + (sizeof(struct microcode_header_intel));
    _(assume (ext_header->count) <= INT_MAX && (ext_header->count) >= INT_MIN)
    _(assume \thread_local(ext_header))
    ext_sigcount =(int) ext_header->count;
    ext_sig =(struct extended_signature*)( (char *)ext_header + (sizeof(struct extended_sigtable)));
    for ( i = 0; i < ext_sigcount; i++ )
    {
        _(assume (ext_sig->sig) <= INT_MAX && (ext_sig->sig) >= INT_MIN)
        _(assume (ext_sig->pf) <= INT_MAX && (ext_sig->pf) >= INT_MIN)
        _(assume \thread_local(ext_sig))
        if ( microcode_update_match(cpu, mc_header,
                                    (int)ext_sig->sig, (int)ext_sig->pf) )
            goto find;
        ext_sig++;
    }
    return 0;
 find:

    if ( new_mc == NULL )
    {


        return -12;
    }

    char *pddtr1=(char*)new_mc,*pddtr2=(char*)mc;
    int iddtr=0;
    for(;iddtr<_(unchecked)((int)total_size);iddtr++){
      _(assume \writable(pddtr1))
      _(assume \thread_local(pddtr2))
      *pddtr1++=*pddtr2++;
    }
    //xfree(uci->mc.mc_intel);
    _(assume \writable(&(uci->mc.mc_intel)))
    uci->mc.mc_intel = new_mc;
    return 1;
}


static int apply_microcode(unsigned int cpu)
{
    unsigned long flags;
    unsigned long long msr_content;
    unsigned int val[2];
    unsigned int cpu_num = 0;//raw_smp_processor_id();
    struct ucode_cpu_info *uci = &ucode_cpu_info;


    do { if (cpu_num != cpu) __bug("ap.c", 371); } while (0);
    _(assume \thread_local(&(uci->mc)))
    if ( uci->mc.mc_intel == NULL )
        return -101;





    //wrmsrl(0x00000079, (unsigned long)uci->mc.mc_intel->bits);
    //wrmsrl(0x0000008b, 0x0ULL);


    //cpuid_eax(1);


    //rdmsrl(0x0000008b, msr_content);
    val[1] = (unsigned long)(msr_content >> 32);

    _(assume \thread_local(&(uci->mc.mc_intel->hdr.rev)))
    if ( val[1] != uci->mc.mc_intel->hdr.rev )
    {




        return -5;
    }






    _(assume \writable(&(uci->cpu_sig.rev)))
    uci->cpu_sig.rev = val[1];

    return 0;
}

static long get_next_ucode_from_buffer(void **mc, const unsigned char *buf,unsigned long size, long offset)
{
    struct microcode_header_intel *mc_header;
    unsigned long total_size;

    _(assume size <= INT_MAX && size >= INT_MIN)
    if ( offset >= (__int32) size )
        return 0;
    mc_header = (struct microcode_header_intel *)(buf + offset);
    _(assume \thread_local(&(((struct microcode_intel *)mc_header)->hdr)))
    total_size = ((struct microcode_intel *)mc_header)->hdr.totalsize ?((struct microcode_intel *)mc_header)->hdr.totalsize :((2000) + (sizeof(struct microcode_header_intel)));

    _(assume total_size <= INT_MAX && total_size >= INT_MIN)
    if ((_(unchecked)((int)total_size)) > (int)size )
    {
        return -101;
    }

//    *mc = xmalloc_bytes(total_size);
    _(assume \thread_local(mc))
    if ( *mc == NULL )
    {
        return -12;
    }
    //memcpy(*mc, (const void *)(buf + offset), total_size);
    char *pddtr1=(char*)(*mc),*pddtr2=(char*)(buf+offset);
    int iddtr=0;
    for(;iddtr<(int)total_size;iddtr++){
      _(assume \writable(pddtr1))
      _(assume \thread_local(pddtr2))
      *pddtr1++=*pddtr2++;
    }
    //xfree(uci->mc.mc_intel);
//    uci->mc.mc_intel = new_mc;
    return _(unchecked)((int)total_size);
}

static int cpu_request_microcode(unsigned int cpu, const void *buf,
                                 size_t size)
{
    long offset = 0;
    int error = 0;
    void *mc;
    unsigned int matching_count = 0;


    //do { if (cpu != raw_smp_processor_id()) __bug("ap.c", 449); } while (0);

    while ( (offset = get_next_ucode_from_buffer(&mc, (const unsigned char *)buf, _(unchecked)((unsigned long)size), (long)offset)) > 0 )
    {
        error = microcode_sanity_check(mc);
        if ( error )
            break;
        error = get_matching_microcode(mc, cpu);
        if ( error < 0 )
            break;




        if ( error == 1 )
        {
            _(unchecked)(matching_count++);
            error = 0;
        }
        //xfree(mc);
    }
    if ( offset > 0 )
        //xfree(mc);
    if ( offset < 0 )
        error = offset;

    if ( !error && matching_count )
        error = apply_microcode(cpu);

    return error;
}

static int microcode_resume_match(unsigned int cpu, const void *mc)
{
    return get_matching_microcode(mc, cpu);
}

//static const struct microcode_ops microcode_intel_ops = {
//    .microcode_resume_match = microcode_resume_match,
//    .cpu_request_microcode = cpu_request_microcode,
//    .collect_cpu_info = collect_cpu_info
//    .apply_microcode = apply_microcode
//};


extern struct cpuinfo_x86 boot_cpu_data;
//static int cpu_request_microcode(unsigned int cpu, const void *buf,
//                                 size_t size)
//{
//    long offset = 0;
//    int error = 0;
//    void *mc;
//    unsigned int matching_count = 0;
//
//
//    //do { if (cpu != raw_smp_processor_id()) __bug("ap.c", 504); } while (0);
//
//    while ( (offset = get_next_ucode_from_buffer(&mc, (const unsigned char *)buf, (unsigned long)size, (long)offset)) > 0 )
//    {
//        error = microcode_sanity_check(mc);
//        if ( error )
//            break;
//        error = get_matching_microcode(mc, cpu);
//        if ( error < 0 )
//            break;
//
//
//
//
//        if ( error == 1 )
//        {
//            matching_count++;
//            error = 0;
//        }
//        //xfree(mc);
//    }
//    if ( offset > 0 )
//        //xfree(mc);
//    if ( offset < 0 )
//        error = offset;
//
//    if ( !error && matching_count )
//        error = apply_microcode(cpu);
//
//    return error;
//}

//static int microcode_resume_match(unsigned int cpu, const void *mc)
//{
//    return get_matching_microcode(mc, cpu);
//}


//static int collect_cpu_info(unsigned int cpu, struct cpu_signature *csig)
//{
//    struct cpuinfo_x86 *c = (struct cpuinfo_x86*)&cpu_data[cpu];
//
//    char * ptrsd=(char*)csig;
//    int ptrid=0;
//    for(;ptrid < sizeof(*csig); ptrid++, ptrsd++)
//      *ptrsd=0;
//
//
//    if ( (c->x86_vendor != 2) || (c->x86 < 0x10) )
//    {
//
//
//
//        return -101;
//    }
//    //rdmsrl(0x0000008b, csig->rev);
//
//
//
//
//    return 0;
//}
//static int apply_microcode(unsigned int cpu)
//{
//    unsigned long flags;
//    unsigned long long msr_content;
//    unsigned int val[2];
//    unsigned int cpu_num = 0;//raw_smp_processor_id();
//    struct ucode_cpu_info *uci = &ucode_cpu_info;
//
//
//    do { if (cpu_num != cpu) __bug("ap.c", 575); } while (0);
//
//    if ( uci->mc.mc_intel == NULL )
//        return -101;
//
//
//
//
//
//    //wrmsrl(0x00000079, (unsigned long)uci->mc.mc_intel->bits);
//    //wrmsrl(0x0000008b, 0x0ULL);
//
//
//    //cpuid_eax(1);
//
//
//    //rdmsrl(0x0000008b, msr_content);
//    val[1] = (unsigned long)(msr_content >> 32);
//
//
//    if ( val[1] != uci->mc.mc_intel->hdr.rev )
//    {
//
//
//
//
//        return -5;
//    }
//
//
//
//
//
//
//
//    uci->cpu_sig.rev = val[1];
//
//    return 0;
//}
static int start_update(void)
{
    return 0;
}
struct microcode_ops {
    int (*microcode_resume_match)(unsigned int cpu, const void *mc);
    int (*cpu_request_microcode)(unsigned int cpu, const void *buf,
                                 size_t size);
    int (*collect_cpu_info)(unsigned int cpu, struct cpu_signature *csig);
    int (*apply_microcode)(unsigned int cpu);
    int (*start_update)(void);
};
extern const struct microcode_ops *microcode_ops;
int microcode_init_intel(void)
{
     _(assume \thread_local(&boot_cpu_data))
    if ( boot_cpu_data.x86_vendor == 1 )
//        microcode_ops = &microcode_intel_ops;
    return 0;
}
#include <stdlib.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef u8 uint8_t;
typedef u16 uint16_t;
typedef u32 uint32_t;
typedef u64 uint64_t;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long s64;

typedef unsigned char __u8;
typedef unsigned short __u16;
typedef unsigned int __u32;

struct equiv_cpu_entry
{
   uint32_t installed_cpu;
   uint32_t fixed_errata_mask;
   uint32_t fixed_errata_compare;
   uint16_t equiv_cpu;
   uint16_t reserved;
};

struct microcode_header_amd
{
   uint32_t data_code;
   uint32_t patch_id;
   uint8_t mc_patch_data_id[2];
   uint8_t mc_patch_data_len;
   uint8_t init_flag;
   uint32_t mc_patch_data_checksum;
   uint32_t nb_dev_id;
   uint32_t sb_dev_id;
   uint16_t processor_rev_id;
   uint8_t nb_rev_id;
   uint8_t sb_rev_id;
   uint8_t bios_api_rev;
   uint8_t reserved1[3];
   uint32_t match_reg[8];
};


void xfree(void * va){return;}


void *_xmalloc(unsigned long size, unsigned long align){return;}
static inline int wrmsr_safe(unsigned int msr, uint32_t val)
{
   int _rc;
   uint32_t lo, hi;
   lo = (uint32_t)val;
   hi = (uint32_t)(val >> 31);
   return _rc;
}

void __assert_failed(char *assertion, char *file, int line)
_(requires \true)
{
   return;
}

void __bug(char *file, int line)
_(requires \true)
{
   return;
}
struct microcode_amd;
struct microcode_amd
{
   void *mpb;
   unsigned int mpb_size;
   struct equiv_cpu_entry *equiv_cpu_table;
   unsigned int equiv_cpu_table_size;
};

struct mpbhdr
{
   uint32_t type;
   uint32_t len;
   uint8_t data[];
};

struct lock_profile
{
   struct lock_profile *next;
   char *name;
   struct spinlock *lock;
   u64 lock_cnt;
   u64 block_cnt;
   s64 time_hold;
   s64 time_block;
   s64 time_locked;
};

struct lock_debug
{
   s16 irq_safe;
};

void spin_debug_enable(void);
void spin_debug_disable(void);

struct A{
  u32 head_tail;
} ;

struct B{
       u16 head;
       u16 tail;
} ;

typedef union {
   struct A a;
   struct B b;
} spinlock_tickets_t;

typedef struct spinlock
{
   spinlock_tickets_t tickets;
   u16 recurse_cpu;

   u16 recurse_cnt;

   struct lock_debug debug;

} spinlock_t;
spinlock_t microcode_update_lock /*= {{0}, 0xfffu, 0, {-1}}*/;
void _spin_lock(spinlock_t *lock);
void _spin_lock_cb(spinlock_t *lock, void (*cond)(void *), void *data);
void _spin_lock_irq(spinlock_t *lock);
unsigned long _spin_lock_irqsave(spinlock_t *lock);

void _spin_unlock(spinlock_t *lock);
void _spin_unlock_irq(spinlock_t *lock);
void _spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags);

int _spin_is_locked(spinlock_t *lock);
int _spin_trylock(spinlock_t *lock);
void _spin_barrier(spinlock_t *lock);

int _spin_trylock_recursive(spinlock_t *lock);
void _spin_lock_recursive(spinlock_t *lock);
void _spin_unlock_recursive(spinlock_t *lock);

struct C{
  uint32_t bits;
};

struct D{
   unsigned long revision : 4;
   unsigned long part_number : 12;
   unsigned long architecture : 4;
   unsigned long variant : 4;
   unsigned long implementer : 8;
};

struct E{
  uint32_t bits[2];
};
struct F{
   unsigned long arm : 4;
   unsigned long thumb : 4;
   unsigned long jazelle : 4;
   unsigned long thumbee : 4;
   unsigned long __res0 : 16;

   unsigned long progmodel : 4;
   unsigned long security : 4;
   unsigned long mprofile : 4;
   unsigned long virt : 4;
   unsigned long gentimer : 4;
   unsigned long __res1 : 12;
};

struct cpuinfo_arm
{
   union {
      struct C c;
      struct D d;
   } midr;
   union {

       struct
       {
           unsigned long aff0 : 8;
           unsigned long aff1 : 8;
           unsigned long aff2 : 8;
           unsigned long mt : 1;
           unsigned long __res0 : 5;
           unsigned long up : 1;
           unsigned long mp : 1;
       };
   } mpidr;
   union {
       struct E e;
       struct F f;
   } pfr32;

   struct
   {
       uint32_t bits[1];
   } dbg32;

   struct
   {
       uint32_t bits[1];
   } aux32;

   struct
   {
       uint32_t bits[4];
   } mm32;

   struct
   {
       uint32_t bits[6];
   } isa32;
};
struct cpuinfo_arm cpu_data[100000];
struct cpuinfo_x86
{
   __u8 x86;
   __u8 x86_vendor;
   __u8 x86_model;
   __u8 x86_mask;
   int cpuid_level;
   __u32 extended_cpuid_level;
   unsigned int x86_capability[(50 + 1)];
   char x86_vendor_id[16];
   char x86_model_id[64];
   int x86_cache_size;
   int x86_cache_alignment;
   __u32 x86_max_cores;
   __u32 booted_cores;
   __u32 x86_num_siblings;
   __u32 apicid;
   __u32 phys_proc_id;
   __u32 cpu_core_id;
   __u32 compute_unit_id;
   unsigned short x86_clflush_size;
} __cacheline_aligned;

static unsigned int cpuid_eax(unsigned int op)
{
   unsigned int eax;
   return eax;
}

typedef int bool_t;

struct cpu_signature
{
   unsigned int sig;
   unsigned int pf;
   unsigned int rev;
};

struct G{
  void *mc_valid;
};

struct H{
  struct microcode_amd *mc_amd;
};

struct ucode_cpu_info
{
   struct cpu_signature cpu_sig;
   union {
      struct H h;
       struct G g;     
   } mc;
};

 int collect_cpu_info(signed int cpu, struct cpu_signature *csig)
 _(requires \true)
 _(ensures \result<=0)
 {
   struct cpuinfo_x86 *c = (struct cpuinfo_x86 *)&cpu_data[cpu];
   _(assume \thread_local(c));

   if ((c->x86_vendor != 2) || (c->x86 < 0x10))
   {
       return -101;
   }


   return 0;
}

struct cpuinfo_x86 boot_cpu_data;

static bool_t verify_patch_size(uint32_t patch_size)
_(requires \true)
_(ensures \result==0 || \result == 1)
 {

    _(assume \thread_local(&boot_cpu_data))
   uint32_t max_size;
   switch (boot_cpu_data.x86)
   {
   case 0x14:
       max_size = 1824;
       break;
   case 0x15:
       max_size = 4096;
       break;
   case 0x16:
       max_size = 3458;
       break;
   case 0x17:
       max_size = 3200;
       break;
   default:
       max_size = 2048;
       break;
   }

   return (patch_size <= max_size);
}

unsigned int raw_smp_processor_id() { return 0; }

static bool_t find_equiv_cpu_id(const struct equiv_cpu_entry *equiv_cpu_table,
                               unsigned int current_cpu_id,
                               unsigned int *equiv_cpu_id)
_(requires \true)
_(writes equiv_cpu_id)
_(ensures \result<=1)
{

   unsigned int i;

   if (!equiv_cpu_table)
       return 0;
  
   i=0;
   _(assert equiv_cpu_id!= NULL)
   
   for (; ; )

   {
        _(assume \thread_local(&equiv_cpu_table[i]));
        if(equiv_cpu_table[i].installed_cpu==0)
          break;     
        
       if (current_cpu_id == equiv_cpu_table[i].installed_cpu)
       {
           *equiv_cpu_id = equiv_cpu_table[i].equiv_cpu & 0xffff;
           return 1;
       }
        i =_(unchecked)( i+1);
        
   }

   return 0;
}

unsigned int ucode_cpu_data;

struct ucode_cpu_info *per_cpu(unsigned long ucode_cpu_info, unsigned int cpu)
_(requires \true)
{
   struct ucode_cpu_info *ucode_cpu_data;
   return ucode_cpu_data;
}

static bool_t microcode_fits(const struct microcode_amd *mc_amd,
                            unsigned int cpu)
_(requires \true)
_(ensures \result<=1)
{ 
    _(assume \thread_local(&ucode_cpu_data));
   struct ucode_cpu_info *uci = per_cpu(ucode_cpu_data, cpu);
   _(assume \thread_local(mc_amd));
   const struct microcode_header_amd *mc_header = mc_amd->mpb;
   const struct equiv_cpu_entry *equiv_cpu_table = mc_amd->equiv_cpu_table;
   unsigned int current_cpu_id;
   unsigned int equiv_cpu_id;

   do
   {
       if (cpu != raw_smp_processor_id())
           __bug("microcode_amd.c", 617);
   } while (0);

   current_cpu_id = cpuid_eax(0x00000001);

   if (!find_equiv_cpu_id(equiv_cpu_table, current_cpu_id, &equiv_cpu_id))
       return 0;

    _(assume \thread_local(mc_header)); 
   if ((mc_header->processor_rev_id) != equiv_cpu_id)
       return 0;

   if (!verify_patch_size(mc_amd->mpb_size))
   {
       return 0;
   }

   _(assume \thread_local(&uci->cpu_sig));
   if (mc_header->patch_id <= uci->cpu_sig.rev)
   {
       return 0;
   }

   return 1;
}



void _spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags)
_(requires \true)
{
   
   return;  
}

unsigned int flush_area_local(const void *va, unsigned int flags)
_(ensures \result==flags)
{
      return flags; 
}

static int apply_microcode(unsigned int cpu)
_(requires \true)
{
   unsigned long flags;
   _(assume \thread_local(&ucode_cpu_data));
   struct ucode_cpu_info *uci = per_cpu(ucode_cpu_data, cpu);
   uint32_t rev;
   _(assume \thread_local(&uci->mc.h));
   struct microcode_amd *mc_amd = uci->mc.h.mc_amd;
   struct microcode_header_amd *hdr;
   int hw_err;

   do
   {
       if (raw_smp_processor_id() != cpu)
           __bug("microcode_amd.c", 667);
   } while (0);

   if (mc_amd == ((void *)0))
       return -101;

     _(assume \thread_local(mc_amd));
   hdr = mc_amd->mpb;
   if (hdr == ((void *)0))
       return -101;

   hw_err = wrmsr_safe(0xc0010020, (unsigned long)hdr);

   do
   {
       unsigned long a__, b__;
   } while (0);

   _spin_unlock_irqrestore(&microcode_update_lock, flags);

   

   flush_area_local(hdr, 0x200 | ((0) + 1));

   _()
   _(assume \thread_local(hdr));
   if (hw_err || (rev != hdr->patch_id))
   {
       return -2;
   }
   _(assume \mutable(&uci->cpu_sig));
   _(assume \thread_local(&uci->cpu_sig));

   return 0;
}



static int get_ucode_from_buffer_amd(
   struct microcode_amd *mc_amd,
   const struct mpbhdr *buf,
   unsigned int bufsize,
   unsigned int *offset)
_(writes mc_amd)
_(writes &mc_amd->mpb_size)
_(writes &mc_amd->mpb)
_(writes offset)
_(requires \true)
{
   _(assume \thread_local(offset));
   const struct mpbhdr *mpbuf = buf + *offset;
   
   if (*offset >= bufsize)
   {
       return -101;
   }

   _(assume \thread_local(mpbuf));
   if (mpbuf->type != 0x00000001)
   {
       return -101;
   }

   if (( _(unchecked)((*offset + mpbuf->len)) ) > bufsize)
   {
       return -101;
   }

   _(assume \thread_local(mc_amd));
   if (mc_amd->mpb_size < mpbuf->len)
   {
       if (mc_amd->mpb)
       {
           xfree(mc_amd->mpb);
           mc_amd->mpb_size = 0;
       }
       mc_amd->mpb = _xmalloc(mpbuf->len, (1 << (7)));
       if (mc_amd->mpb == ((void *)0))
           return -1;
       mc_amd->mpb_size = mpbuf->len;
   }
   //__builtin___memcpy_chk(mc_amd->mpb, mpbuf->data, mpbuf->len, 100000);


   *offset = _(unchecked)((*offset+mpbuf->len + 8));

   return 0;
}

static int install_equiv_cpu_table(
   struct microcode_amd *mc_amd,
   const struct mpbhdr *data,
   unsigned int *offset)
_(requires \true)
_(writes offset)
_(writes &mc_amd->equiv_cpu_table_size)
_(ensures \result<=0)
 { 
  _(assume \thread_local(offset))

   const struct mpbhdr *mpbuf = data + *offset + 4;

   _(assume \thread_local(mpbuf))
   *offset = _(unchecked) (*offset+mpbuf->len + 12);

   if (mpbuf->type != 0x00000000)
   {
       return -101;
   }

   if (mpbuf->len == 0)
   {
       return -101;
   }
   _(assume \thread_local(mc_amd) )
   if (!mc_amd->equiv_cpu_table)
   {
       return -1;
   }

   mc_amd->equiv_cpu_table_size = mpbuf->len;

   return 0;
}

static int container_fast_forward(const uint32_t *data, unsigned int size_left, unsigned int *offset)
_(writes offset)
_(requires \true)
_(ensures \result<=0)
{
   for (;;)
   {
       unsigned int size;
       const uint32_t *header;

       if (size_left < 8)
           return -101;

       _(assume \thread_local(offset))  
       header = data + *offset;

       _(assume \thread_local(&header[0]))
       _(assume \thread_local(&header[1]))
       if (header[0] == 0x00414d44 &&
           header[1] == 0x00000000)
           break;

       if (header[0] != 0x00000001)
           return -101;
       size = _(unchecked)(header[1] + 8);
       if (size < 32 || size_left < size)
           return -101;

       size_left -= size;
       *offset = _(unchecked)(*offset+size);

       if (!size_left)
           return -4;
   }

   return 0;
}
static const unsigned int final_levels[] = {
   0x01000098,
   0x0100009f,
   0x010000af};

static bool_t check_final_patch_levels(unsigned int cpu)
_(requires \true)
_(ensures \result<=1)
{

    _(assume \thread_local(&ucode_cpu_data))
   struct ucode_cpu_info *uci = per_cpu(ucode_cpu_data, cpu);
   int i;
   _(assume \thread_local(&boot_cpu_data))
   if (boot_cpu_data.x86 != 0x10)
       return 0;

    _(assume \thread_local(&uci->cpu_sig))
   for (i = 0; i < (int)(sizeof(final_levels) / sizeof(final_levels[0])); i++)
       if (uci->cpu_sig.rev == final_levels[i])
           return 1;

   return 0;
}

static int cpu_request_microcode(unsigned int cpu, const void *buf,
                                unsigned int bufsize)

_(requires \true)
{
   struct microcode_amd *mc_amd, *mc_old;
   unsigned int offset = 0;
   unsigned int last_offset, applied_offset = 0;
   int error = 0, save_error = 1;
   _(assume \thread_local(&ucode_cpu_data))
   struct ucode_cpu_info *uci = per_cpu(ucode_cpu_data, cpu);
   unsigned int current_cpu_id;
   unsigned int equiv_cpu_id;

   do
   {
       if (cpu != raw_smp_processor_id())
           __bug("microcode_amd.c", 877);
   } while (0);

   current_cpu_id = cpuid_eax(0x00000001);

   _(assume \thread_local((const uint32_t *)buf))
   if (*(const uint32_t *)buf != 0x00414d44)
   {
       error = -101;
       goto out;
   }

   if (check_final_patch_levels(cpu))
   {
       error = -8;
       goto out;
   }

   mc_amd = ((struct microcode_amd *)malloc(sizeof(struct microcode_amd)));
   if (!mc_amd)
   {
       error = -1;
       goto out;
   }
   _(assume \thread_local(&offset))
   _(assume \thread_local(&uci->mc.h))

   mc_old = uci->mc.h.mc_amd;

   mc_amd->mpb = ((void *)0);
   mc_amd->mpb_size = 0;
   last_offset = offset;

   if (applied_offset)
   {
       save_error = get_ucode_from_buffer_amd(
           mc_amd, buf, bufsize, &applied_offset);

       if (save_error)
           error = save_error;
   }

   if (save_error)
   {
       xfree(mc_amd);
   }
   else
       xfree(mc_old);

out:
   return error;
}

static int microcode_resume_match(unsigned int cpu, const void *mc)
_(requires \true)
_(ensures \result<=1)
{ 
    _(assume \thread_local(&ucode_cpu_data))
   struct ucode_cpu_info *uci = per_cpu(ucode_cpu_data, cpu);
   _(assume \thread_local(&uci->mc.h))
   struct microcode_amd *mc_amd = uci->mc.h.mc_amd;
   const struct microcode_amd *src = mc;

   _(assume \thread_local(src));
   _(assume \thread_local(mc_amd))
   if (!microcode_fits(src, cpu))
       return 0;

   if (src != mc_amd)
   {
       if (mc_amd)
       {
           xfree(mc_amd->equiv_cpu_table);
           xfree(mc_amd->mpb);
           xfree(mc_amd);
       }

       mc_amd = ((struct microcode_amd *)malloc(sizeof(struct microcode_amd)));
       if (!mc_amd)
           return -1;

       mc_amd->equiv_cpu_table = _xmalloc(src->equiv_cpu_table_size, (1 << (7)));
       if (!mc_amd->equiv_cpu_table)
           goto err1;
       mc_amd->mpb = _xmalloc(src->mpb_size, (1 << (7)));
       if (!mc_amd->mpb)
           goto err2;

       mc_amd->equiv_cpu_table_size = src->equiv_cpu_table_size;
       mc_amd->mpb_size = src->mpb_size;
   }

   return 1;

err2:
   xfree(mc_amd->equiv_cpu_table);
err1:
   xfree(mc_amd);
   return -1;
}

static int start_update(void)
{
   return 0;
}

struct microcode_ops
{
   int (*microcode_resume_match)(unsigned int cpu, const void *mc);
   int (*cpu_request_microcode)(unsigned int cpu, const void *buf,
                                unsigned int size);
   int (*collect_cpu_info)(unsigned int cpu, struct cpu_signature *csig);
   int (*apply_microcode)(unsigned int cpu);
   int (*start_update)(void);
};
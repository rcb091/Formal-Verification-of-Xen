#include<vcc.h>
#include<stdio.h>
#include<stdlib.h>

typedef bool bool_t;
typedef struct { int counter; } atomic_t;
typedef unsigned long long    uint64_t;
typedef unsigned int    uint32_t;
typedef unsigned short      uint16_t;

#define false 0
#define VM_EVENT_REASON_MOV_TO_MSR              5
#define VM_EVENT_REASON_WRITE_CTRLREG           4
#define VM_EVENT_REASON_MEM_ACCESS              1
#define VM_EVENT_REASON_SOFTWARE_BREAKPOINT     6
#define VM_EVENT_REASON_DESCRIPTOR_ACCESS       13
#define VM_EVENT_X86_CR0    0
#define VM_EVENT_X86_CR3    1
#define VM_EVENT_X86_CR4    2

#define VM_EVENT_FLAG_EMULATE            (1 << 2)
#define VM_EVENT_FLAG_TOGGLE_SINGLESTEP  (1 << 4)
#define VM_EVENT_FLAG_SET_EMUL_READ_DATA (1 << 5)
#define VM_EVENT_FLAG_DENY               (1 << 6)
#define VM_EVENT_FLAG_SET_EMUL_INSN_DATA (1 << 9)

_(pure)
static inline struct cpu_info *get_cpu_info(void);


struct cpu_info {
    struct vcpu *current_vcpu;
};

struct segment_register {
    uint16_t   sel;
    union {
        _(backing_member)
        uint16_t attr;
        struct {
            uint16_t type:4;
            uint16_t s:   1;
            uint16_t dpl: 2;
            uint16_t p:   1;
            uint16_t avl: 1;
            uint16_t l:   1;
            uint16_t db:  1;
            uint16_t g:   1;
            uint16_t pad: 4;
        };
    };
    uint32_t   limit;
    uint64_t   base;
};

enum guest_type {
    guest_type_pv, guest_type_hvm
};

// Structure definitions, unused code is removed
struct domain
{
    struct vcpu **vcpu;
    struct arch_domain arch;
    enum guest_type guest_type;

};


struct vcpu
{
    int              vcpu_id;
    int              processor;
    struct domain   *domain;
    struct vcpu     *next_in_list;
    struct arch_vcpu arch;
    atomic_t         vm_event_pause_count;
    atomic_t         pause_count;
};

struct hvm_vcpu{
    bool                single_step;
};
struct arch_vcpu
{
    struct arch_vm_event *vm_event;
    struct {
        bool next_interrupt_enabled;
    } monitor;
    union {
        struct hvm_vcpu hvm;
    };
};

struct monitor_write_data {
    struct {
        unsigned int msr;
        unsigned int cr0;
        unsigned int cr3;
        unsigned int cr4;
    } do_write;
};

struct vm_event_emul_read_data{};
struct vm_event_emul_insn_data{};

struct arch_vm_event {
    uint32_t emulate_flags;
    union {
        struct vm_event_emul_read_data read;
        struct vm_event_emul_insn_data insn;
    } emul;
    struct vm_event_regs_x86 gprs;
    bool set_gprs;
    /* A sync vm_event has been sent and we're not done handling it. */
    bool sync_event;
    struct monitor_write_data write_data;
};
    
struct arch_domain {
    bool_t mem_access_emulate_each_rep;
}; 

struct vm_event_write_ctrlreg {
    uint32_t index;
};

typedef struct vm_event_st {
    
    uint32_t flags;
    uint32_t reason;
    union {
        struct vm_event_write_ctrlreg         write_ctrlreg;
    } u;
    
    union {
        union {
          struct vm_event_emul_read_data read;
          struct vm_event_emul_insn_data insn;
        } emul;
        union {
            struct vm_event_regs_x86 x86;
            struct vm_event_regs_arm arm;
        } regs;
    } data;
} vm_event_response_t;  

struct vm_event_regs_x86 {
    uint64_t rax;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rbx;
    uint64_t rsp;
    uint64_t rbp;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rflags;
    uint64_t dr6;
    uint64_t dr7;
    uint64_t rip;
    uint64_t cr0;
    uint64_t cr2;
    uint64_t cr3;
    uint64_t cr4;
    uint64_t sysenter_cs;
    uint64_t sysenter_esp;
    uint64_t sysenter_eip;
    uint64_t msr_efer;
    uint64_t msr_star;
    uint64_t msr_lstar;
    uint32_t cs_base;
    uint32_t ss_base;
    uint32_t ds_base;
    uint32_t es_base;
    uint64_t fs_base;
    uint64_t gs_base;
    struct vm_event_x86_selector_reg cs;
    struct vm_event_x86_selector_reg ss;
    struct vm_event_x86_selector_reg ds;
    struct vm_event_x86_selector_reg es;
    struct vm_event_x86_selector_reg fs;
    struct vm_event_x86_selector_reg gs;
    uint64_t shadow_gs;
    uint16_t cs_sel;
    uint16_t ss_sel;
    uint16_t ds_sel;
    uint16_t es_sel;
    uint16_t fs_sel;
    uint16_t gs_sel;
    uint32_t _pad;
};
struct vm_event_regs_arm {
};

enum x86_segment {
    /* General purpose.  Matches the SReg3 encoding in opcode/ModRM bytes. */
    x86_seg_es,
    x86_seg_cs,
    x86_seg_ss,
    x86_seg_ds,
    x86_seg_fs,
    x86_seg_gs,
    /* System: Valid to use for implicit table references. */
    x86_seg_tr,
    x86_seg_ldtr,
    x86_seg_gdtr,
    x86_seg_idtr,
    /* No Segment: For accesses which are already linear. */
    x86_seg_none
};

struct vm_event_x86_selector_reg {
    uint32_t limit  :    20;
    uint32_t ar     :    12;
};

struct hvm_function_table {
    void (*get_segment_register)(struct vcpu *v, enum x86_segment seg,
                                 struct segment_register *reg);
    bool_t (*is_singlestep_supported)(void);

};

extern struct hvm_function_table hvm_funcs;

static inline int atomic_read(const atomic_t *v)
_(decreases 0)
{
  _(assume \thread_local(v))
    return *(volatile int *)&v->counter;
}

static inline bool is_hvm_domain(const struct domain *d)
_(decreases 0)
{
    _(assume \thread_local(d))
    return d->guest_type == guest_type_hvm;
}

static inline bool is_x86_user_segment(enum x86_segment seg)
_(decreases 0)
{
    unsigned int idx = seg;
    return idx <= x86_seg_gs;
}

static inline bool_t hvm_is_singlestep_supported(void)
_(decreases 0)
{
    _(assume \thread_local(&hvm_funcs))
    return hvm_funcs.is_singlestep_supported;
}

static inline
bool p2m_mem_access_emulate_check(struct vcpu *v,
                                  const vm_event_response_t *rsp)
_(decreases 0)
{
    /* Not supported on ARM. */
    return false;
}

void hvm_toggle_singlestep(struct vcpu *v)
  _(requires \true)
  _(writes &v->arch.hvm.single_step)
  _(decreases 0) 
{
    _(assume \thread_local(v))
    _(assert v!=NULL)
    do { if ( 0 && (atomic_read(&v->pause_count)) ) {} } while (0);
    //ASSERT(atomic_read(&v->pause_count));

    if ( !hvm_is_singlestep_supported() )
        return;

    _(assume \thread_local(&v->arch.hvm.single_step))
    v->arch.hvm.single_step = !v->arch.hvm.single_step;
}


/* Implicitly serialized by the domctl lock. */
int vm_event_init_domain(struct domain *d)
_(requires \true)
_(requires \wrapped(d))
_(requires \thread_local(d))
_(writes \span(d))
_(ensures \result == -12 || \result == 0)
{
    struct vcpu *v;
    _(assume \thread_local(v))
    _(assume \thread_local(&d->vcpu[0]))
    _(assert d!=NULL)
    for( (v) = (d)->vcpu ? (d)->vcpu[0] : NULL; (v) != NULL; (v) = (v)->next_in_list )
    _(invariant v!=NULL)
    {
        if ( v->arch.vm_event )
            continue;
        
        _(assert v->arch.vm_event==NULL)
        //v->arch.vm_event = xzalloc(struct arch_vm_event);
        v->arch.vm_event = ((struct arch_vm_event *)malloc(sizeof(struct arch_vm_event)));

        if ( !v->arch.vm_event )
            return -12;
    }

    return 0;
}

/*
 * Implicitly serialized by the domctl lock,
 * or on domain cleanup paths only.
 */
void vm_event_cleanup_domain(struct domain *d)
_(requires \true)
_(requires \wrapped(d))
_(requires \thread_local(d))
_(writes \span(d))
_(ensures d->arch.mem_access_emulate_each_rep==0)
{
    struct vcpu *v;
    _(assume \thread_local(v))
    _(assume \thread_local(&d->vcpu[0]))
    _(assert v!=NULL)
    _(assert d!=NULL)
    for( (v) = (d)->vcpu ? (d)->vcpu[0] : NULL; (v) != NULL; (v) = (v)->next_in_list )
    _(invariant v!=NULL)
    {
        
        //xfree(v->arch.vm_event);
        free(v->arch.vm_event);
        v->arch.vm_event = NULL;
    }

    d->arch.mem_access_emulate_each_rep = 0;
}


void vm_event_toggle_singlestep(struct domain *d, struct vcpu *v,
                                vm_event_response_t *rsp)
_(requires \thread_local(rsp))
_(writes &v->arch.hvm.single_step)
_(decreases 0)
{
    _(assume \thread_local(v))
    _(assume \thread_local(d))
    _(assert v!=NULL)
    _(assert d!=NULL)
    _(assert rsp!=NULL)
    
    if ( !(rsp->flags & VM_EVENT_FLAG_TOGGLE_SINGLESTEP) )
        return;
     
     _(assert rsp->flags & VM_EVENT_FLAG_TOGGLE_SINGLESTEP)

    if ( !is_hvm_domain(d) )
        return;
    do { if ( 0 && (atomic_read(&v->vm_event_pause_count)) ) {} } while (0);
    //ASSERT(atomic_read(&v->vm_event_pause_count));

    hvm_toggle_singlestep(v);
}

void vm_event_register_write_resume(struct vcpu *v, vm_event_response_t *rsp)
_(requires \thread_local(rsp))
_(requires \thread_local(&v->arch))
_(requires \thread_local(& rsp->u.write_ctrlreg.index))
_(decreases 0)
{
    _(assume \thread_local(v))
    _(assert v!=NULL)
    _(assert rsp!=NULL)
    if ( rsp->flags & VM_EVENT_FLAG_DENY )
    {
        _(assert rsp->flags & VM_EVENT_FLAG_DENY)
        struct monitor_write_data *w;
        _(assume \writable(&w->do_write.msr))
        do { if ( 0 && (v->arch.vm_event) ) {} } while (0);
        //ASSERT(v->arch.vm_event);

        /* deny flag requires the vCPU to be paused */
        if ( !atomic_read(&v->vm_event_pause_count) )
            return;
        //_(assume \thread_local(w))
        w = &v->arch.vm_event->write_data;
        
        _(assert w!=NULL)
        
        switch ( rsp->reason )
        {
        case VM_EVENT_REASON_MOV_TO_MSR:
            _(assume \writable(&w->do_write.msr))
            w->do_write.msr = 0;
            break;
        case VM_EVENT_REASON_WRITE_CTRLREG:
            switch ( rsp->u.write_ctrlreg.index )
            {
            case VM_EVENT_X86_CR0:
                _(assume \writable(&w->do_write.cr0))
                w->do_write.cr0 = 0;
                break;
            case VM_EVENT_X86_CR3:
                _(assume \writable(&w->do_write.cr3))
                w->do_write.cr3 = 0;
                break;
            case VM_EVENT_X86_CR4:
                _(assume \writable(&w->do_write.cr4))
                w->do_write.cr4 = 0;
                break;
            }
            break;
        }
    }
}

void vm_event_set_registers(struct vcpu *v, vm_event_response_t *rsp)
_(writes &(v->arch.vm_event->set_gprs))
_(writes \extent(&v->arch.vm_event->gprs ))
_(requires \thread_local(v))
_(requires \extent_mutable(&rsp->data.regs.x86))
_(ensures v->arch.vm_event->set_gprs==1)
_(decreases 0)
{
    _(assert v!=NULL)
    _(assume \thread_local(rsp))
    _(assert rsp!=NULL)
    do { if ( 0 && (atomic_read(&v->vm_event_pause_count)) ) {} } while (0); 
    _(assume \thread_local(&v->arch))
    _(assume \thread_local(rsp))

    v->arch.vm_event->gprs = rsp->data.regs.x86;
    v->arch.vm_event->set_gprs = 1;
}

void vm_event_monitor_next_interrupt(struct vcpu *v)
_(writes &v->arch.monitor.next_interrupt_enabled)
_(ensures v->arch.monitor.next_interrupt_enabled == 1)
_(decreases 0)
{
    _(assume \thread_local(v))
    _(assert v!=NULL)
    v->arch.monitor.next_interrupt_enabled = 1;
}

void vm_event_sync_event(struct vcpu *v, bool value)
_(writes &v->arch.vm_event->sync_event)
_(requires \thread_local (&v->arch))
_(ensures v->arch.vm_event->sync_event==value)
_(decreases 0)
{
    _(assume \thread_local(v))
    _(assert v!=NULL)
    _(assert v->arch.vm_event!=NULL)
    v->arch.vm_event->sync_event = value;
}

void vm_event_emulate_check(struct vcpu *v, vm_event_response_t *rsp)
_(writes &v->arch.vm_event->emulate_flags)
_(requires \thread_local (&v->arch))
_(requires \thread_local (rsp))
_(requires \extent_mutable(&rsp->data.emul.read))
_(requires \extent_mutable(&rsp->data.emul.insn))
_(writes \extent(&v->arch.vm_event->emul.read))
_(writes \extent(&v->arch.vm_event->emul.insn))
_(decreases 0)
{
    _(assert v!=NULL)
    _(assert rsp!=NULL)
    if ( !(rsp->flags & VM_EVENT_FLAG_EMULATE) )
    {
        v->arch.vm_event->emulate_flags = 0;
        return;
    }

    switch ( rsp->reason )
    {
    case VM_EVENT_REASON_MEM_ACCESS:
        /*
         * Emulate iff this is a response to a mem_access violation and there
         * are still conflicting mem_access permissions in-place.
         */
        if ( p2m_mem_access_emulate_check(v, rsp) )
        {
            if ( rsp->flags & VM_EVENT_FLAG_SET_EMUL_READ_DATA )
                v->arch.vm_event->emul.read = rsp->data.emul.read;

            v->arch.vm_event->emulate_flags = rsp->flags;
        }
        break;

    case VM_EVENT_REASON_SOFTWARE_BREAKPOINT:
        if ( rsp->flags & VM_EVENT_FLAG_SET_EMUL_INSN_DATA )
        {
            v->arch.vm_event->emul.insn = rsp->data.emul.insn;
            v->arch.vm_event->emulate_flags = rsp->flags;
        }
        break;

    case VM_EVENT_REASON_DESCRIPTOR_ACCESS:
        if ( rsp->flags & VM_EVENT_FLAG_SET_EMUL_READ_DATA )
            v->arch.vm_event->emul.read = rsp->data.emul.read;
        v->arch.vm_event->emulate_flags = rsp->flags;
        break;

    default:
        break;
    };
}


void hvm_get_segment_register(struct vcpu *v, enum x86_segment seg,
                              struct segment_register *reg)
_(requires \wrapped(reg))
_(requires \wrapped(v))
_(requires \mutable(reg))
_(decreases 0)
{
    _(assume \thread_local(&hvm_funcs))
    _(assert v!=NULL)
    _(assert reg!=NULL)
    hvm_funcs.get_segment_register(v, seg, reg);

    switch ( seg )
    {
    case x86_seg_ss:
        /* SVM may retain %ss.DB when %ss is loaded with a NULL selector. */
        
        if ( !reg->p )
            reg->db = 0;
        break;

    case x86_seg_tr:
        /*
         * SVM doesn't track %tr.B. Architecturally, a loaded TSS segment will
         * always be busy.
         */
        reg->type |= 0x2;

        /*
         * %cs and %tr are unconditionally present.  SVM ignores these present
         * bits and will happily run without them set.
         */
    case x86_seg_cs:
        reg->p = 1;
        break;

    case x86_seg_gdtr:
    case x86_seg_idtr:
        /*
         * Treat GDTR/IDTR as being present system segments.  This avoids them
         * needing special casing for segmentation checks.
         */
        reg->attr = 0x80;
        break;

    default: /* Avoid triggering -Werror=switch */
        break;
    }

    if ( reg->p )
    {
        /*
         * For segments which are present/usable, cook the system flag.  SVM
         * ignores the S bit on all segments and will happily run with them in
         * any state.
         */
        reg->s = is_x86_user_segment(seg);

        /*
         * SVM discards %cs.G on #VMEXIT.  Other user segments do have .G
         * tracked, but Linux commit 80112c89ed87 "KVM: Synthesize G bit for
         * all segments." indicates that this isn't necessarily the case when
         * nested under ESXi.
         *
         * Unconditionally recalculate G.
         */
        reg->g = !!(reg->limit >> 20);

        /*
         * SVM doesn't track the Accessed flag.  It will always be set for
         * usable user segments loaded into the descriptor cache.
         */
        if ( is_x86_user_segment(seg) )
            reg->type |= 0x1;
    }
}

#ifdef CONFIG_HVM
static void vm_event_pack_segment_register(enum x86_segment segment,
                                           struct vm_event_regs_x86 *reg)
_(writes \span(reg))
_(requires \wrapped(get_cpu_info()))
{
    struct segment_register seg; 
       
    _(assume \thread_local(&seg))
    _(assume \wrapped(&seg))
    
    _(assert reg!=NULL)
    hvm_get_segment_register(get_cpu_info()->current_vcpu, segment, &seg);

    switch ( segment )
    {
    case x86_seg_ss:
        reg->ss_base = (uint32_t) seg.base;
        reg->ss.limit = seg.g ? seg.limit >> 12 : seg.limit;
        reg->ss.ar = seg.attr;
        reg->ss_sel = seg.sel;
        break;

    case x86_seg_fs:
        reg->fs_base = seg.base;
        reg->fs.limit = seg.g ? seg.limit >> 12 : seg.limit;
        reg->fs.ar = seg.attr;
        reg->fs_sel = seg.sel;
        break;

    case x86_seg_gs:
        reg->gs_base = seg.base;
        reg->gs.limit = seg.g ? seg.limit >> 12 : seg.limit;
        reg->gs.ar = seg.attr;
        reg->gs_sel = seg.sel;
        break;

    case x86_seg_cs:
        reg->cs_base = (uint32_t) seg.base;
        reg->cs.limit = seg.g ? seg.limit >> 12 : seg.limit;
        reg->cs.ar = seg.attr;
        reg->cs_sel = seg.sel;
        break;

    case x86_seg_ds:
        reg->ds_base = (uint32_t) seg.base;
        reg->ds.limit = seg.g ? seg.limit >> 12 : seg.limit;
        reg->ds.ar = seg.attr;
        reg->ds_sel = seg.sel;
        break;

    case x86_seg_es:
        reg->es_base = (uint32_t) seg.base;
        reg->es.limit = seg.g ? seg.limit >> 12 : seg.limit;
        reg->es.ar = seg.attr;
        reg->es_sel = seg.sel;
        break;

    default:
        //ASSERT_UNREACHABLE();
        do { } while (0);
    }
}
#endif
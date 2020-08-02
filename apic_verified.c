unsigned int apic_read(unsigned int lvr);
unsigned int GET_APIC_VERSION(unsigned int);
struct node{
  unsigned int x86_vendor;
  unsigned int x86;
  }boot_cpu_data;
  
static int modern_apic(void)
_(requires \thread_local(&boot_cpu_data))
_(writes &boot_cpu_data.x86_vendor)
 _(writes &boot_cpu_data.x86)
{
    unsigned int lvr, version;
    //struct node boot_cpu_data;
   /* AMD systems use old APIC versions, so check the CPU */
    unsigned int X86_VENDOR_AMD;
    
    if (boot_cpu_data.x86_vendor == X86_VENDOR_AMD &&
        boot_cpu_data.x86 >= 0xf) 
        return 1;  
        _(ghost unsigned int APIC_LVR)
    lvr = apic_read(APIC_LVR);
    version = GET_APIC_VERSION(lvr);
    return version >= 0x14;
}

/*
 * Bootstrap processor local APIC boot mode - so we can undo our changes
 * to the APIC state.
 */
 
 

void printk(void*,unsigned int);
void ack_APIC_irq();
void ack_bad_irq(unsigned int irq)
_(requires \true)
_(requires irq>0)
_(ensures irq>0) 
{
    printk("unexpected IRQ trap at irq %02x\n", irq);
    
    /*
     * Currently unexpected vectors happen only on SMP and APIC.
     * We _must_ ack these because every local APIC has only N
     * irq slots per priority level, and a 'hanging, unacked' IRQ
     * holds up an irq slot - in excessive cases (when multiple
     * unexpected vectors occur) that might lock up the APIC
     * completely.
     * But only ack when the APIC is enabled -AK
     */
     
    _(ghost unsigned int cpu_has_apic);
    if (cpu_has_apic)
        ack_APIC_irq();
        
      _(assert \mutable(&irq))
}

/*
 * 'what should we do if we get a hw irq event on an illegal vector'.
 * each architecture has to answer this themselves.
 */


int GET_APIC_MAXLVT(unsigned int );
//unsigned int apic_read(unsigned int);

/*
     * Careful: we have to set masks only first to deassert
     * any level-triggered sources.
     */
int get_maxlvt(void)
_(requires \true)
{
  
    _(ghost unsigned int APIC_LVR);
    unsigned int v = apic_read(APIC_LVR);
  
    return GET_APIC_MAXLVT(v);
}

/*
     * Do not trust the local APIC being empty at bootup.
     */
  /*
         * Put the board back into PIC mode (has an effect
         * only on certain older boards).  Note that APIC
         * interrupts, including IPIs, won't work beyond
         * this point!  The only exception are INIT IPIs.
         */
bool modern_apicc(void)
_(requires \true)
{
  return 1;
}         
         
int get_maxlvt(void);
void apic_write(int,unsigned int);
unsigned long apic_read(int);
void clear_local_APIC(void)
{
    int maxlvt;
    unsigned long v;
/*
     * Masking an LVT entry on a P6 can trigger a local APIC error
     * if the vector is zero. Mask LVTERR first to prevent this.
     */
    maxlvt = get_maxlvt();
    _(ghost int APIC_TMICT);
    apic_write(APIC_TMICT, 0);
    
    if (maxlvt >= 3) {
      _(ghost unsigned int ERROR_APIC_VECTOR);
        v = ERROR_APIC_VECTOR; 
        _(ghost  int APIC_LVTERR);
        _(ghost  unsigned int APIC_LVT_MASKED);
        apic_write(APIC_LVTERR, v | APIC_LVT_MASKED);
    }
    
    /*
     * Careful: we have to set masks only first to deassert
     * any level-triggered sources.
     */
    
    _(ghost int APIC_LVTT);
    v = apic_read(APIC_LVTT);
    _(ghost unsigned int APIC_LVT_MASKED)
    apic_write(APIC_LVTT, v | APIC_LVT_MASKED);
    _(ghost int APIC_LVT0);
    v = apic_read(APIC_LVT0);
    apic_write(APIC_LVT0, v | APIC_LVT_MASKED);
    _(ghost int APIC_LVT1);
    v = apic_read(APIC_LVT1);
    apic_write(APIC_LVT1, v | APIC_LVT_MASKED);
    if (maxlvt >= 4) {
      _(ghost int APIC_LVTPC);
        v = apic_read(APIC_LVTPC);
        apic_write(APIC_LVTPC, v | APIC_LVT_MASKED);
    }
}



/*
         * Put the board back into PIC mode (has an effect
         * only on certain older boards).  Note that APIC
         * interrupts, including IPIs, won't work beyond
         * this point!  The only exception are INIT IPIs.
        
         */



void apic_wait_icr_idle(void);
 __init sync_Arb_IDs(void);
bool modern_apic(void);
void apic_printk(unsigned int, void*);
void apic_write(unsigned int ,unsigned int);
void sync_Arb_IDs(void)
{
    /* Unsupported on P4 - see Intel Dev. Manual Vol. 3, Ch. 8.6.1
       And not needed on AMD */
    if (modern_apicc())
        return;
    /*
     * Wait for idle.
     */
    apic_wait_icr_idle();

    _(ghost unsigned int APIC_DEBUG);
    _(ghost unsigned int APIC_ICR);
    _(ghost unsigned int APIC_DEST_ALLINC);
    _(ghost unsigned int APIC_INT_LEVELTRIG);
    _(ghost unsigned int APIC_DM_INIT);
    apic_printk(APIC_DEBUG, "Synchronizing Arb IDs.\n");
    apic_write(APIC_ICR, APIC_DEST_ALLINC | APIC_INT_LEVELTRIG | APIC_DM_INIT);
}


 /*
     * Detecting directed EOI on BSP:
     * If having directed EOI support in lapic, force to use ioapic_ack_old,
     * and enable the directed EOI for intr handling.
     */



void x2apic_ap_setup(void);

void __enable_x2apic(void);
void x2apic_ap_setup(void)
{
  _(ghost bool x2apic_enabled)
    if ( x2apic_enabled )
        __enable_x2apic();
}






static void __enable_x2apic(void);
void rdmsrl(unsigned int,unsigned int);
void wrmsrl(unsigned int ,unsigned int);

static void __enable_x2apic(void)
{
    unsigned int msr_content;
    _(ghost unsigned int MSR_APIC_BASE);
    rdmsrl(MSR_APIC_BASE, msr_content);
    _(ghost unsigned int APIC_BASE_EXTD);
    _(ghost unsigned int APIC_BASE_ENABLE);
    if ( !(msr_content & APIC_BASE_EXTD) )
    {
        msr_content |= APIC_BASE_ENABLE | APIC_BASE_EXTD;
        msr_content = (unsigned int)msr_content;
        wrmsrl(MSR_APIC_BASE, msr_content);
    }
}


int strcmp(char *, const char *);

static int apic_set_verbosity(const char *str)
_(requires \thread_local(str))
_(requires str!=NULL)
_(ensures str!=NULL)
{
  _(ghost unsigned int apic_verbosity);
  _(ghost unsigned int APIC_DEBUG);
  _(ghost unsigned int APIC_VERBOSE);
  _(ghost int EINVAL);
    if (strcmp("debug", str) == 0)
        apic_verbosity = APIC_DEBUG;
    else if (strcmp("verbose", str) == 0)
        apic_verbosity = APIC_VERBOSE;
    else if(EINVAL<100000)
        return EINVAL;

    return 0;
}





int lapic_disable(const char *);
void setup_clear_cpu_cap(unsigned int);
int lapic_disable(const char *str)
_(requires \thread_local(str))
_(requires str!=NULL)
_(ensures str!=NULL) 
{
  _(ghost int enable_local_apic);
  _(ghost unsigned int X86_FEATURE_APIC);
    enable_local_apic = -1;
    setup_clear_cpu_cap(X86_FEATURE_APIC);
    return 0;
}


    /*
     * The next two are just to see if we have sane values.
     * They're only really relevant if we're in Virtual Wire
     * compatibility mode, but most boxes are anymore.
     */



struct node
{
  unsigned int active;
};

static void apic_pm_activate(void)
{
  struct node apic_pm_state;
_(writes &apic_pm_state.active)
_(ensures apic_pm_state.active == 1)
    apic_pm_state.active = 1;
}


/*
     * After a crash, we no longer service the interrupts and a pending
     * interrupt from previous kernel might still have ISR bit set.
     *
     * Most probably by now CPU has serviced that pending interrupt and
     * it might not have done the ack_APIC_irq() because it thought,
     * interrupt came from i8259 as ExtInt. LAPIC did not get EOI so it
     * does not clear the ISR bit and cpu thinks it has already serivced
     * the interrupt. Hence a vector might get locked. It was noticed
     * for timer irq (vector 0x31). Issue an extra EOI to clear ISR.
     */


int  get_8254_timer_count(void);
unsigned int inb_p(unsigned short int );
void outb_p(unsigned char , unsigned short int );
unsigned int  get_8254_timer_count(void)
_(requires \true)
{
    /*extern spinlock_t i8253_lock;*/
    /*unsigned long flags;*/

    unsigned int count;

    /*spin_lock_irqsave(&i8253_lock, flags);*/
  _(ghost unsigned short int PIT_MODE);
  _(ghost unsigned short int PIT_CH0);
    outb_p(0x00, PIT_MODE);
    count = inb_p(PIT_CH0);
    count |= inb_p(PIT_CH0) << 8;
    
    /*spin_unlock_irqrestore(&i8253_lock, flags);*/
    _(assert count>=0);

    return count;
}


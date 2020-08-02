#include <vcc.h>
#include<stdio.h>
#include <stdbool.h>
#include <asm/spinlock.h>
#include <xen/init.h>
#include <xen/types.h>
#include <asm/regs.h>
#include <xen/errno.h>
#include <xen/sched.h>
#include <xen/irq.h>
#include <asm/atomic.h>
#include <asm/system.h>
#include <asm/io.h>
#include <asm/desc.h>
#include <xen/bitops.h>
#include <xen/delay.h>
#include <asm/apic.h>
#include <asm/asm_defns.h>
#include <io_ports.h>
#include <asm/atomic.h>
#include <asm/system.h>
#include <asm/timer.h>
#include <asm/hw_irq.h>
#include <asm/pgtable.h>
#include <asm/desc.h>
#include <asm/apic.h>
#include <asm/i8259.h>
#include <asm/irq_vector.h>
/*
* This is the 'legacy' 8259A Programmable Interrupt Controller,
* present in the majority of PC/AT boxes.
* plus some generic x86 specific things if generic specifics makes
* any sense at all.
* this file should become arch/i386/kernel/irq.c when the old irq.c
* moves to arch independent land

*/
_(ghost static struct dummy{int cpu_mask;int vector;})
_(ghost struct irq_desc{ unsigned int irq; int status;static struct hw_interrupt_type__read_mostly
*handler;static struct dummy arch;})
_(ghost static struct hw_interrupt_type__read_mostly{void *startup;void *shutdown;void *enable;void
*disable;void *ack;void *end;})
//_(ghost unsigned int i8259A_lock)
_(def struct irq_desc * irq_to_desc(int a))
_(def void outb(int a,int b))
_(def void outb_p(int a,int b))
_(def int inb(int a))
_(def void apic_intr_init(void))
_(def void init_bsp_APIC(void))
_(def void udelay(int))
_(def int platform_legacy_irq(int))
_(def void setup_irq(int,int,static struct irqaction__read_mostly *))
//_(def int cpumask_of(int))
_(def void cpumask_copy(int,int))
_(def unsigned LEGACY_VECTOR(unsigned int))
//_(def int per_cpu(int*,int)[])
_(def int smp_processor_id(void))
_(ghost int* vector_irq)
_(ghost )
//_(ghost spinlock_t i8259A_lock = SPIN_LOCK_UNLOCK)
//_(def void printk(char *a,int b))
// static DEFINE_SPINLOCK( i8259A_lock);

int cpumask_of(int a){
  return a;
}

//int per_cpu(int* arr,int len)[unsigned a]{
//  return (int)len;
//}



static bool _mask_and_ack_8259A_irq(unsigned int irq);

bool bogus_8259A_irq(unsigned int irq)
_(requires \true)
{
return _mask_and_ack_8259A_irq(irq);
}

static void mask_and_ack_8259A_irq(struct irq_desc *desc)
_(requires \thread_local(desc))
{

_mask_and_ack_8259A_irq(desc->irq);
}

static unsigned int startup_8259A_irq(struct irq_desc *desc)
_(requires \mutable(desc))
_(writes &cached_irq_mask)
{
enable_8259A_irq(desc);
return 0; /* never anything pending */
}
_(ghost bool IRQ_DISABLED)
_(ghost bool IRQ_INPROGRESS)
static void end_8259A_irq(struct irq_desc *desc, unsigned short vector)
_(requires \thread_local(desc))
_(requires \thread_local(&IRQ_DISABLED))
_(requires \thread_local(&IRQ_INPROGRESS))
_(writes &cached_irq_mask)
{
if (!(desc->status & (IRQ_DISABLED|IRQ_INPROGRESS)))
enable_8259A_irq(desc);
}

static struct hw_interrupt_type__read_mostly i8259A_irq_type = {
//.typename = "XT-PIC",
//.startup = startup_8259A_irq,
//.shutdown = disable_8259A_irq,
//.enable = enable_8259A_irq,
//.disable = disable_8259A_irq,
//.ack = mask_and_ack_8259A_irq,
//.end = end_8259A_irq
};

/*
* 8259A PIC functions to handle ISA devices:
*/

#define aeoi_mode (i8259A_irq_type.ack == disable_8259A_irq)

/*
* This contains the irq mask for both 8259A irq controllers,
*/

static unsigned int cached_irq_mask = 0xffff;
_(def int __byte(int,unsigned int))
#define __byte(x,y) (((unsigned char *)&(y))[x])
#define cached_21 (__byte(0,cached_irq_mask))
#define cached_A1 (__byte(1,cached_irq_mask))

/*
* Not all IRQs can be routed through the IO-APIC, eg. on certain (older)
* boards the timer interrupt is not really connected to any IO-APIC pin,
* it's fed to the master 8259A's IR0 line only.
*
* Any '1' bit in this mask means the IRQ is routed through the IO-APIC.
* this 'mixed mode' IRQ handling costs nothing because it's only used
* at IRQ setup time.
*/
unsigned int io_apic_irqs;
_(ghost int i8259A_lock)

void spin_lock_irqsave(int^ i82,unsigned long flags){}
void spin_unlock_irqrestore(int^ i82,unsigned long flags){}
static void _disable_8259A_irq(unsigned int irq)
_(writes &cached_irq_mask)
{
  _(assume irq<32 && irq>=0)
unsigned int mask = 1 << irq;
unsigned long flags;

spin_lock_irqsave(&i8259A_lock, flags);
cached_irq_mask |= mask;
if (irq & 8)
outb(cached_A1,0xA1);
else
outb(cached_21,0x21);
//per_cpu(vector_irq, 0)[LEGACY_VECTOR(irq)] = (int)~irq;
spin_unlock_irqrestore(&i8259A_lock, flags);
}

void disable_8259A_irq(struct irq_desc *desc)
_(requires \thread_local(desc))
_(writes &cached_irq_mask)
{
_disable_8259A_irq(desc->irq);
}

void enable_8259A_irq(struct irq_desc *desc)
_(requires \thread_local(desc))
_(writes &cached_irq_mask)
{
_(assume desc->irq<32 && desc->irq>=0)
unsigned int mask = ~(1 << desc->irq);
unsigned long flags;

spin_lock_irqsave(&i8259A_lock, flags);
cached_irq_mask &= mask;
//per_cpu(vector_irq, 0)[LEGACY_VECTOR(desc->irq)] = (int)desc->irq;
if (desc->irq & 8)
outb(cached_A1,0xA1);
else
outb(cached_21,0x21);
spin_unlock_irqrestore(&i8259A_lock, flags);
}

int i8259A_irq_pending(unsigned int irq)
_(requires \true)
{
  _(assume irq<32 && irq>=0)
unsigned int mask = 1<<irq;
unsigned long flags;
int ret;

spin_lock_irqsave(&i8259A_lock, flags);
if (irq < 8)
ret = inb(0x20) & (int)mask;
else
ret = inb(0xA0) & (int)(mask >> 8);

spin_unlock_irqrestore(&i8259A_lock, flags);

return ret;
}

void mask_8259A(void)
{
unsigned long flags;

spin_lock_irqsave(&i8259A_lock, flags);
outb(0xff, 0xA1);
outb(0xff, 0x21);
spin_unlock_irqrestore(&i8259A_lock, flags);
}

void unmask_8259A(void)
{
unsigned long flags;

spin_lock_irqsave(&i8259A_lock, flags);
outb(cached_A1, 0xA1);
outb(cached_21, 0x21);
spin_unlock_irqrestore(&i8259A_lock, flags);
}

/*
* This function assumes to be called rarely. Switching between
* 8259A registers is slow.
* This has to be protected by the irq controller spinlock
* before being called.
*/
static inline int i8259A_irq_real(unsigned int irq)
{
int value;
_(assume irq<32 && irq>0)
int irqmask = _(unchecked)((int)(1<<irq));

if (irq < 8) {
outb(0x0B,0x20); /* ISR register */
value = inb(0x20) & irqmask;
outb(0x0A,0x20); /* back to the IRR register */
return value;
}
outb(0x0B,0xA0); /* ISR register */
value = inb(0xA0) & (irqmask >> 8);
outb(0x0A,0xA0); /* back to the IRR register */
return value;
}

/*
* Careful! The 8259A is a fragile beast, it pretty
* much _has_ to be done exactly like this (mask it
* first, _then_ send the EOI, and the order of EOI
* to the two 8259s is important! Return a boolean
* indicating whether the irq was genuine or spurious.
*/
static bool _mask_and_ack_8259A_irq(unsigned int irq)

{
  _(assume irq<32 && irq>=0)
unsigned int irqmask = _(unchecked)(1 << irq);
 
unsigned long flags;
bool is_real_irq = 1; /* Assume real unless spurious */ //gundo

spin_lock_irqsave(&i8259A_lock, flags);
 
 
if ((cached_irq_mask & irqmask) && !i8259A_irq_real(irq)) {
static int spurious_irq_mask;
is_real_irq = 0; //gundo
/* Report spurious IRQ, once per IRQ line. */
if (!(spurious_irq_mask & _(unchecked)((int)irqmask))) {
// printk("spurious 8259A interrupt: IRQ%d.\n", irq);
spurious_irq_mask |= _(unchecked)(int)irqmask;
}
/*
* Theoretically we do not have to handle this IRQ,
* but in Linux this does not cause problems and is
* simpler for us.
*/
}

cached_irq_mask |= irqmask;

//1 if (irq & 8) {
//2 inb(0xA1); /* DUMMY - (do we need this?) */
//3 outb(cached_A1,0xA1);
//1 if (!aeoi_mode) {
//2 outb(0x60 + (irq & 7), 0xA0);/* 'Specific EOI' to slave */
//2 outb(0x62,0x20); /* 'Specific EOI' to master-IRQ2 */
//2 }
//2 } else {
//2 inb(0x21); /* DUMMY - (do we need this?) */
//2 outb(cached_21,0x21);

//2 if (!aeoi_mode)
//3 outb(0x60 + irq, 0x20);/* 'Specific EOI' to master */
// }

spin_unlock_irqrestore(&i8259A_lock, flags);

return is_real_irq;
}

static char irq_trigger[2];
/**
* ELCR registers (0x4d0, 0x4d1) control edge/level of IRQ
*/
static void restore_ELCR(char *trigger)
_(requires \thread_local_array(trigger,2))
{
outb(trigger[0], 0x4d0);
outb(trigger[1], 0x4d1);
}

static void save_ELCR(char *trigger)
_(writes trigger,trigger+1)
{
/* IRQ 0,1,2,8,13 are marked as reserved */
trigger[0] = _(unchecked)(char)(inb(0x4d0) & 0xF8);
trigger[1] = _(unchecked)(char)(inb(0x4d1) & 0xDE);
}

int i8259A_resume(void)
_(requires \true)
_(ensures \result==0)
_(writes &irq_trigger,irq_trigger+1)
{
// init_8259A(aeoi_mode);
restore_ELCR(irq_trigger);
return 0;
}

int i8259A_suspend(void)
_(writes &irq_trigger,irq_trigger+1)
{
save_ELCR(irq_trigger);
return 0;
}

void init_8259A(int auto_eoi)
_(requires \true)
{
unsigned long flags;
_(ghost int FIRST_LEGACY_VECTOR)
spin_lock_irqsave(&i8259A_lock, flags);

outb(0xff, 0x21); /* mask all of 8259A-1 */
outb(0xff, 0xA1); /* mask all of 8259A-2 */

/*
* outb_p - this has to work on a wide range of PC hardware.
*/
outb_p(0x11, 0x20); /* ICW1: select 8259A-1 init */
outb_p(FIRST_LEGACY_VECTOR + 0, 0x21); /* ICW2: 8259A-1 IR0-7 */
outb_p(0x04, 0x21); /* 8259A-1 (the master) has a slave on IR2 */
if (auto_eoi)
outb_p(0x03, 0x21); /* master does Auto EOI */
else
outb_p(0x01, 0x21); /* master expects normal EOI */

outb_p(0x11, 0xA0); /* ICW1: select 8259A-2 init */
outb_p(_(unchecked)(FIRST_LEGACY_VECTOR + 8), 0xA1); /* ICW2: 8259A-2 IR0-7 */
outb_p(0x02, 0xA1); /* 8259A-2 is a slave on master's IR2 */
outb_p(0x01, 0xA1); /* (slave's support for AEOI in flat mode

is to be investigated) */

//1 if (auto_eoi)
/*

* in AEOI mode we just have to mask the interrupt
* when acking.
*/
//2 i8259A_irq_type.ack = disable_8259A_irq;
//3 else
//4 i8259A_irq_type.ack = mask_and_ack_8259A_irq;

udelay(100); /* wait for 8259A to initialize */

outb(cached_21, 0x21); /* restore master IRQ mask */
outb(cached_A1, 0xA1); /* restore slave IRQ mask */

spin_unlock_irqrestore(&i8259A_lock, flags);
}

void make_8259A_irq(unsigned int irq)
_(requires \true)
_(writes &io_apic_irqs,&irq_to_desc((int)irq)->handler)
{
  _(assume irq<32 && irq>=0)
io_apic_irqs &= ~(1 << irq);
irq_to_desc((int)irq)->handler = &i8259A_irq_type;
}

static struct irqaction__read_mostly cascade = { 
  //no_action, "cascade", NULL
};

void init_IRQ(void)
_(requires \true)
 

{
int irq, cpu = smp_processor_id();

_(ghost int FIRST_LEGACY_VECTOR)
init_bsp_APIC();

init_8259A(0);
for (irq = 0; platform_legacy_irq(irq);irq++)
 {
//_(assert irq<irq+1)
struct irq_desc *desc = irq_to_desc(irq);
_(assert \wrapped(desc))
_(assert \writable(desc))
if ( irq == 2 ) /* IRQ2 doesn't exist */
continue;
desc->handler = &i8259A_irq_type;
//per_cpu(vector_irq, cpu)[(unsigned)(FIRST_LEGACY_VECTOR + irq)] = irq;
cpumask_copy(desc->arch.cpu_mask, cpumask_of(cpu));
desc->arch.vector = (int)FIRST_LEGACY_VECTOR + irq;
}
_(ghost unsigned IRQ0_VECTOR)
//per_cpu(vector_irq, cpu)[IRQ0_VECTOR] = 0;

apic_intr_init();

setup_irq(2, 0, &cascade);
}
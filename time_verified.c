 //* arch/x86/time.c
 
 //* Per-CPU time calibration and management.
 
 /* opt_clocksource: Force clocksource to one of: pit, hpet, acpi. */
 
 #include <xen/errno.h>
#include <xen/event.h>
#include <xen/sched.h>
#include <xen/lib.h>
#include <xen/init.h>
#include <xen/time.h>
#include <xen/timer.h>
#include <xen/smp.h>
#include <xen/irq.h>
#include <xen/softirq.h>
#include <xen/efi.h>
#include <xen/cpuidle.h>
#include <xen/symbols.h>
#include <xen/keyhandler.h>
#include <xen/guest_access.h>
#include <asm/io.h>
#include <asm/msr.h>
#include <asm/mpspec.h>
#include <asm/processor.h>
#include <asm/fixmap.h>
#include <asm/guest.h>
#include <asm/mc146818rtc.h>
#include <asm/div64.h>
#include <asm/acpi.h>
#include <asm/hpet.h>
#include <io_ports.h>
#include <asm/setup.h> /* for early_time_init */
#include <public/arch-x86/cpuid.h>

#include <vcc.h>

typedef struct cpumask{ } cpumask_t;
extern cpumask_t cpu_online_map;
extern void pit_broadcast_mask;
static inline void cpumask_and(cpumask_t *dstp, const cpumask_t *src1p,
			       const cpumask_t *src2p);
int cpumask_test_cpu(int cpu,bool  mask);
int __cpumask_clear_cpu(int cpu,bool mask);
int cpumask_empty(bool mask);
void raise_softirq(bool TIMER_SOFTIRQ);
enum {
  TIMER_SOFTIRQ=0
};
int cpumask_raise_softirq(bool mask,bool TIMER_SOFTIRQ);

/* Calibrate all CPUs to platform timer every EPOCH. */

static void smp_send_timer_broadcast_ipi(void)
_(requires \true)
{
    int cpu ;
    cpumask_t mask;

    cpumask_and(&mask, &cpu_online_map, &pit_broadcast_mask);

    if ( cpumask_test_cpu(cpu, &mask) )
    {
        __cpumask_clear_cpu(cpu, &mask);
        raise_softirq(TIMER_SOFTIRQ);
    }

    if ( !cpumask_empty(&mask) )
    {
        cpumask_raise_softirq(&mask, TIMER_SOFTIRQ);
    }
}

//

/*
 * We simulate a 32-bit platform timer from the 16-bit PIT ch2 counter.
 * Otherwise overflow happens too quickly (~50ms) for us to guarantee that
 * softirq handling will happen in time.
 * 
 * The pit_lock protects the 16- and 32-bit stamp fields as well as the 
 */
 
#define PIT_MODE		0x43
#define PIT_CH2			0x42
void spin_lock_irqsave(int * ,unsigned int flags);
void spin_unlock_irqrestore(int *,unsigned int flags);
void outb(int a,int b);
int inb(int a);

static int read_pit_count(void)
_(requires \true)
_(ensures PIT_CH2!=0)
{
    int count16;
    int count32;
    int pit_stamp32,pit_stamp16;
    unsigned int flags;
     int pit_lock;
    spin_lock_irqsave(&pit_lock, flags);

    count16  = inb(PIT_CH2);
    

    count32 = _(unchecked)(pit_stamp32 + _(unchecked)((int)(pit_stamp16 - count16)));

    spin_unlock_irqrestore(&pit_lock, flags);

    return count32;
}



//
/* Per-socket TSC_ADJUST values, for secondary cores/threads to sync to. */

/*
 * Scale a 64-bit delta by scaling and multiplying by a 32-bit fraction,
 * yielding a 64-bit result.
 */


static void resume_pit(struct platform_timesource *pts)
_(requires \thread_local(pts))
_(requires \true)
_(ensures PIT_MODE!=0)
{
    /* Set CTC channel 2 to mode 0 again; initial value does not matter. */
    outb(0xb0, PIT_MODE); /* binary, mode 0, LSB/MSB, Ch 2 */
    outb(0, PIT_CH2);     /* LSB of count */
    outb(0, PIT_CH2);     /* MSB of count */
}

//

/*
 * cpu_mask that denotes the CPUs that needs timer interrupt coming in as
 * IPIs in place of local APIC timers
 */

void spin_lock(int *);
int __read_platform_stime(int a);
void spin_unlock(int *);
int unlikely(int);
static int read_platform_stime(int *stamp)
_(requires stamp!=NULL)
_(ensures stamp!=NULL)
{
    int plt_counter, count;
    int stime;
    int platform_timer_lock,plt_stamp64,plt_stamp,plt_mask;

  

    spin_lock(&platform_timer_lock);;
    count = _(unchecked)(plt_stamp64 + (plt_counter - plt_stamp) & plt_mask);
    stime = __read_platform_stime(count);
    spin_unlock(&platform_timer_lock);

 

    return stime;
}

///* Rough hack to allow accurate timers to sort-of-work with no APIC. */

/************************************************************
 * PLATFORM TIMER 1: PROGRAMMABLE INTERVAL TIMER (LEGACY PIT)
 */
 
 int domlist_read_lock;
 
static void update_domain_rtc(void)
_(requires \true)
{
    struct domain *d;
  int domlist_read_lock;
    rcu_read_lock(&domlist_read_lock);

    for_each_domain ( d );
        if ( is_hvm_domain(d) )
            rtc_update_clock(d);

    rcu_read_unlock(&domlist_read_lock);
}

/*
     * Now let's take care of CTC channel 2: mode 0, (interrupt on
     * terminal count mode), binary count, load CALIBRATE_LATCH count,
     * (LSB and MSB) to begin countdown.
     */



//
unsigned int read_tsc(void);
unsigned int rdtsc_ordered();
unsigned int read_tsc(void)
_(requires \true)
{
    return rdtsc_ordered();
}


/************************************************************
 * PLATFORM TIMER 2: HIGH PRECISION EVENT TIMER (HPET)
 */
 
 /*
 * Called in verify_tsc_reliability() under reliable TSC conditions
 * thus reusing all the checks already performed there.
 */



//




_(ghost unsigned int s_time_t);
unsigned int get_s_time();
unsigned int get_s_time_fixed(int);
unsigned int  get_s_time()
_(requires \true)
{
    return get_s_time_fixed(0);
}

/*
     * Calculations for platform timer overflow assume u64 boundary.
     * Hence we set to less than 64, such that the TSC wraparound is
     * correctly checked and handled.
     */

//

 /* clocksource=tsc is initialized via __initcalls (when CPUs are up). */


void rcu_read_lock(int* );
void rcu_read_unlock(int *);
struct domain
{
  int x;
};
bool is_hvm_domain(struct domain *);
void rtc_update_clock(struct domain *);
void for_each_domain(struct domain *);
static void update_domain_rtc2(void)
_(requires \true)
{
    struct domain *d;
int domlist_read_lock;
    rcu_read_lock(& domlist_read_lock);

    for_each_domain ( d );
        if ( is_hvm_domain(d) )
            rtc_update_clock(d);

    rcu_read_unlock(& domlist_read_lock);
}

//
/***************************************************************************
 * CMOS Timer functions
 ***************************************************************************/

/* Converts Gregorian date to seconds since 1970-01-01 00:00:00.
 * Assumes input in normal date format, i.e. 1980-12-31 23:59:59
 * => year=1980, mon=12, day=31, hour=23, min=59, sec=59.
 *
 * [For the Julian calendar (which was used in Russia before 1917,
 * Britain & colonies before 1752, anywhere else before 1582,
 * and is still in use by some communities) leave out the
 * -year/100+year/400 terms, and add 10.]
 *
 * This algorithm was first published by Gauss (I think).
 *
 * WARNING: this function will overflow on 2106-02-07 06:28:16 on
 * machines were long is 32-bit! (However, as time_t is signed, we
 * will already get problems at other places on 2038-01-19 03:14:08)
 */


void local_irq_disable(void);
void check_tsc_warp(int,int *);
void cpumask_clear_cpu(unsigned int,int *);
int cpumask_test_cpu(unsigned int ,int *);
void local_irq_enable();
unsigned int smp_processor_id();
void cpu_relax();
static void tsc_check_slave(void *unused)
_(requires unused!=NULL)
_(ensures unused!=NULL)
{
    unsigned int cpu = smp_processor_id();
    local_irq_disable();
    int tsc_check_cpumask;
    while ( !cpumask_test_cpu(cpu, &tsc_check_cpumask) )
        cpu_relax();
        int tsc_max_warp;
 
       int cpu_khz;
   check_tsc_warp(cpu_khz, &tsc_max_warp);
    cpumask_clear_cpu(cpu, &tsc_check_cpumask);
    local_irq_enable();
}

/*
     * It's expected that domains cope with this bit changing on every
     * pvclock read to check whether they can resort solely on this tuple
     * or if it further requires monotonicity checks with other vcpus.
     */

void DEFINE_SPINLOCK(int);
void cpumask_andnot(int *,int *,int);
int cpumask_of(unsigned int);
int cpumask_empty(int *);

static void tsc_check_reliability(void)
_(requires \true)
_(ensures PIT_MODE>0)
{
    unsigned int cpu = smp_processor_id();
    int lock;
     DEFINE_SPINLOCK(lock);

    spin_lock(&lock);
 int tsc_check_count=0;
 int tsc_check_cpumask;
 int cpu_online_map;
 int cpu_khz;
 int tsc_max_warp;
    tsc_check_count++;
   
    cpumask_andnot(&tsc_check_cpumask, &cpu_online_map, cpumask_of(cpu));
    local_irq_disable();
    check_tsc_warp(cpu_khz, &tsc_max_warp);
    local_irq_enable();
    while ( !cpumask_empty(&tsc_check_cpumask) )
        cpu_relax();

    spin_unlock(&lock);
}
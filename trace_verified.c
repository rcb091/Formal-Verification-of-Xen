#include <xen/init.h>
#include <xen/kernel.h>
#include <xen/lib.h>
#include <xen/domain.h>
#include <xen/sched.h>
#include <xen/trace.h>
#include <vcc.h>
#include <limits.h>

#define UNSIGNED_MAX 65536
#define LONG_MAX 9223372036854775807

unsigned int is_pv_32bit_vcpu(unsigned int);// It provides declaration of is_pv_32bit_vcpu 

 void __trace_var(unsigned int , bool ,unsigned int ,const void* );//declaration of _trace_var function
 

 _(ghost unsigned long intpte_t) //intpte_t defined in other file so we used ghost here

 
_(ghost struct l1_pgentry_t{ unsigned long intpte_t;})
void __trace_pv_trap(unsigned short trapnr, unsigned long eip,
                     int use_error_code, unsigned short error_code)
                     _(requires \true)
                     _(ensures trapnr>=0 && eip>=0 && error_code>=0)
                            
{
  _(ghost unsigned int current)
    if ( is_pv_32bit_vcpu(current) )
    {
        struct __packed {
           unsigned long eip;
            unsigned trapnr:16,//changed size to 16 bit as given funtion parameter is of 16 bit
                use_error_code:1,
                error_code:16;
				_(invariant  \this->trapnr <= UNSIGNED_MAX)
        _(invariant  \this->error_code <= UNSIGNED_MAX)
        _(invariant  \this->eip <=LONG_MAX )
           
        } d;
       
              
        d.eip = eip;
        _(assert d.eip == eip)
        d.trapnr =(trapnr);
        _(assert d.trapnr == trapnr)
        d.error_code = error_code;
       
       _(assert d.error_code == error_code)
        d.use_error_code=!!use_error_code;
        //_(assert d.use_error_code != (int)use_error_code)
          _(ghost unsigned int TRC_PV_TRAP)       
        __trace_var(TRC_PV_TRAP, 1, sizeof(d), &d);
        
    }
    else
    {
        struct __packed {
            unsigned long eip;
            unsigned trapnr:16,//comment here
                use_error_code:1,
                error_code:16;
        _(invariant  \this->trapnr <= UNSIGNED_MAX)
        _(invariant  \this->error_code <= UNSIGNED_MAX)
        _(invariant  \this->eip <=LONG_MAX )
        } d;
        unsigned event;

        d.eip = eip;
        _(assert d.eip == eip)
        d.trapnr = _(unchecked)(trapnr);
        _(assert  d.trapnr == trapnr)
        d.error_code = error_code;
        _(assert  d.error_code == error_code)
        d.use_error_code=!!use_error_code;
         _(ghost unsigned int TRC_PV_TRAP)       
        event = TRC_PV_TRAP;
        _(assert event == TRC_PV_TRAP)
        _(ghost unsigned long TRC_64_FLAG)
        event |= TRC_64_FLAG;
        _(assert event == (TRC_PV_TRAP | TRC_64_FLAG))
        __trace_var(event, 1, sizeof(d), &d);
    }
}

void __trace_pv_page_fault(unsigned long addr, unsigned error_code)
  _(requires \true)
  _(ensures addr>=0 && error_code>=0)
{
     
    
   //unsigned long eip = guest_cpu_user_regs()->rip;
    _(ghost unsigned int current)
    if ( is_pv_32bit_vcpu(current) )
    {
        struct __packed {
            unsigned eip, addr, error_code;
        } d;
   _(ghost unsigned long eip)
        d.eip = eip;
        _(assert d.eip == eip)
        d.addr = addr;
        _(assert d.addr == addr)
        d.error_code = error_code;
         _(assert  d.error_code == error_code)
         _(ghost unsigned int TRC_PV_PAGE_FAULT)       
        __trace_var(TRC_PV_PAGE_FAULT, 1, sizeof(d), &d);
    }
    else
    {
        struct __packed {
            unsigned long eip, addr;
            unsigned error_code;
        _(invariant  \this->error_code <= UNSIGNED_MAX)
        _(invariant  \this->eip <=LONG_MAX )
        _(invariant  \this->addr <=LONG_MAX )
        } d;
        unsigned event;
 _(ghost unsigned long eip)
        d.eip = eip;
        _(assert d.eip == eip)
        d.addr = addr;
        _(assert d.addr == addr)
        d.error_code = error_code;
         _(assert d.error_code == error_code)
        _(ghost unsigned int TRC_PV_PAGE_FAULT)     
        event = TRC_PV_PAGE_FAULT;
        _(assert event == TRC_PV_PAGE_FAULT)
        _(ghost unsigned long TRC_64_FLAG)     
        event |= TRC_64_FLAG;
        _(assert event == (TRC_PV_PAGE_FAULT| TRC_64_FLAG))
          
        __trace_var(event, 1, sizeof(d), &d);
    }
}

void __trace_trap_one_addr(unsigned event, unsigned long va)
  _(requires \true)
  _(ensures event>=0 && va>=0)
{
    _(ghost unsigned int current)
    if ( is_pv_32bit_vcpu(current) )
    {
       unsigned d = va;
       _(assert d == va)
        __trace_var(event, 1, sizeof(d), &d);
    }
    else
    {
      _(ghost unsigned long TRC_64_FLAG)
        event |= TRC_64_FLAG;
        _(assert event == \old(event)| TRC_64_FLAG)
        __trace_var(event, 1, sizeof(va), &va);
    }
}

void __trace_trap_two_addr(unsigned event, unsigned long va1,
                           unsigned long va2)
       _(requires \true)
       _(ensures event>=0 && va1>=0 && va2>=0)
{
    _(ghost unsigned int current)
    if ( is_pv_32bit_vcpu(current) )
    {
        struct __packed {
            unsigned va1, va2;
            _(invariant  \this->va1 <=UNSIGNED_MAX )
            _(invariant  \this->va2 <=UNSIGNED_MAX )
        } d;
        d.va1=va1;
        _(assert d.va1 == va1)
        d.va2=va2;
        _(assert d.va2 == va2)
        __trace_var(event, 1, sizeof(d), &d);
    }
    else
    {
        struct __packed {
            unsigned long va1, va2;
            _(invariant  \this->va1 <=UNSIGNED_MAX )
            _(invariant  \this->va2 <=UNSIGNED_MAX )
        } d;
        d.va1=va1;
        _(assert d.va1 == va1)
        d.va2=va2;
        _(assert d.va2 == va2)
        _(ghost unsigned long TRC_64_FLAG)
        event |= TRC_64_FLAG;
        _(assert event == \old(event)|TRC_64_FLAG)
        __trace_var(event, 1, sizeof(d), &d);
    }
}

void __trace_ptwr_emulation(unsigned long addr, l1_pgentry_t npte)
  _(requires \true)
  _(ensures addr>=0)

{
    
    //unsigned long eip = guest_cpu_user_regs()->rip;

    /* We have a couple of different modes to worry about:
     * - 32-on-32: 32-bit pte, 32-bit virtual addresses
     * - pae-on-pae, pae-on-64: 64-bit pte, 32-bit virtual addresses
     * - 64-on-64: 64-bit pte, 64-bit virtual addresses
     * pae-on-64 is the only one that requires extra code; in all other
     * cases, "unsigned long" is the size of a guest virtual address.
     */
    _(ghost unsigned int current)
    if ( is_pv_32bit_vcpu(current) )
    {
        struct __packed {
            l1_pgentry_t pte;
            unsigned addr, eip;
            
            
        } d;
        d.addr = addr;
        _(assert d.addr == addr)
         _(ghost unsigned long eip)
        d.eip = eip;
        _(assert d.eip == eip)
        d.pte = npte;
        
        _(ghost unsigned int TRC_PV_PTWR_EMULATION_PAE)
        __trace_var(TRC_PV_PTWR_EMULATION_PAE, 1, sizeof(d), &d);
    }
    else
    {
        struct {
            l1_pgentry_t pte;
            unsigned long addr, eip;
            _(invariant  \this->addr <=LONG_MAX )
            _(invariant  \this->eip <=LONG_MAX )
        } d;
        unsigned event;

        d.addr = addr;
        _(assert d.addr == addr)
         _(ghost unsigned long eip)
        d.eip = eip;
        _(assert d.eip == eip)
        d.pte = npte;
        
        _(ghost unsigned int TRC_PV_PTWR_EMULATION)
        event = TRC_PV_PTWR_EMULATION;
        _(assert event == TRC_PV_PTWR_EMULATION)
        _(ghost unsigned long TRC_64_FLAG)
        event |= TRC_64_FLAG;
         _(assert event == TRC_PV_PTWR_EMULATION | TRC_64_FLAG);
        __trace_var(event, 1/*tsc*/, sizeof(d), &d);
    }
}

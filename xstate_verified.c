#include <vcc.h>
#include <assert.h>
#include <threads.h>
#include <stdlib.h>
#include <stdio.h>
int XCR_XFEATURE_ENABLED_MASK;
int xcr0, xfeature_mask, processor_id;
long X86_CR0_TS;

struct xsave_struct{
  int xsave_hdr;
}*xstate;

struct vcpu 
{
  struct archi arch;
 }*v;
 struct xsave_struct idle_vcpu[];
  
 struct archi
 {
   unsigned xsave_area;
 };

void assert(int expression); 

void write_cr0(long cr0)
{
  long p;
   p = cr0 ;
}

long read_cr0()
{
  return;
}

 void *memset(void *str, int c, size_t n);

void xrstor(struct vcpu *v,long mask)
{
    long hmask = mask;
    long lmask = mask;
    //struct xsave_struct *ptr = v->arch.xsave_area;
    unsigned int faults, prev_faults;
}

static bool xsetbv(int index, long xfeatures)
{
  _(ensures \result==0||\result==1)
    int hi = xfeatures >> 31;
    int lo = (int)xfeatures;

  /*  asm volatile ( "1: .byte 0x0f,0x01,0xd1\n"
                   "3:                     \n"
                   ".section .fixup,\"ax\" \n"
                   "2: xor %0,%0           \n"
                   "   jmp 3b              \n"
                   ".previous              \n"
                   _ASM_EXTABLE(1b, 2b)
                   : "+a" (lo)
                   : "c" (index), "d" (hi));*/
    
    return lo != 0;
}
bool set_xcr0(long xfeatures)
_(writes &xcr0)
_(writes &XCR_XFEATURE_ENABLED_MASK)
_(ensures \result==0||\result==1)
{ //_(requires \thread_local(&XCR_XFEATURE_ENABLED_MASK))

    if ( !xsetbv(XCR_XFEATURE_ENABLED_MASK, xfeatures) )
        return 0;
       // _(requires \writable(&xcr0))
    xcr0 = xfeatures;
    return 1;
}

void xstate_set_init(long uint64_t, long mask)
_(writes &xcr0)
_(writes &XCR_XFEATURE_ENABLED_MASK)
{
    long cr0 = read_cr0();
     
    long xcr0;
    //struct vcpu *v = idle_vcpu[processor_id];
    //struct xsave_struct *xstate = v->arch.xsave_area;
    
  _(assume \thread_local(&xfeature_mask))
    if ( ~xfeature_mask & mask )
        _(assert(~xfeature_mask & mask))
    {
        _(assume \false);
        return;
    }

    if ( (~xcr0 & mask) && !set_xcr0(xcr0 | mask) )
        return;

   // clts();
   _(assume \thread_local(&xstate))
    memset(&xstate->xsave_hdr, 0, sizeof(xstate->xsave_hdr));
    _(assume \thread_local(&v))
    xrstor(v,mask);

     _(assume \thread_local(&X86_CR0_TS))
    if ( cr0 & X86_CR0_TS )
        write_cr0(cr0);

    if ( (~xcr0 & mask) && !set_xcr0(xcr0) )
        //BUG();
        return;
}


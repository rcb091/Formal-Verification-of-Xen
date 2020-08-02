#include <vcc.h>
#define XSTATE_FP_SSE  1
#define XSTATE_ALL 1
#define XSTATE_LAZY 0
#define XSTATE_NONLAZY 1
typedef int uint64_t ;
struct vcpu
{

//    struct arch_vcpu arch;
    
    int xsave_area ;
    int xcr0_accum;
    int xcr0;
    int nonlazy_xstate_used;
    int fully_eager_fpu;
    int fpu_initialised;
    int fpu_dirtied;
    int fpu_ctxt;
};


int  get_8254_timer_count(void);
unsigned int inb_p(unsigned short int );
void outb_p(unsigned char , unsigned short int );


//int ASSERT(int x)
//{
//  return 1;
//}

int ASSERT(int  p)
{ return 1; }


void xstate_free_save_area(struct vcpu *v)
_(requires \thread_local(v))
_(requires \true)
{}


void xfree(int a)
_(requires \true)
{}



void vcpu_destroy_fpu(struct vcpu *v)
_(requires \thread_local(v))
_(requires \true)
{
    if ( v->xsave_area )
        xstate_free_save_area(v);
    else
        xfree(v->fpu_ctxt);


}









int is_idle_vcpu(struct vcpu *v){
  return 1;
}


void clts(){
  return;
}




int set_xcr0(int  x)
_(requires \true)
_(ensures \result >= 1 && \result <= 10)
{
  return 1;
}




void fpu_fxrstor(struct vcpu *v){
  
}







int xstate_all(struct vcpu *v)
_(requires \thread_local(v))
{
  return 1;
}


int  xrstor(struct vcpu *v,int x)
{
  return 1;
}




static inline void fpu_xrstor(struct vcpu *v, uint64_t mask)
_(requires \thread_local(v))
{
    bool ok;

    ASSERT(v->xsave_area);
    /*
     * XCR0 normally represents what guest OS set. In case of Xen itself, 
     * we set the accumulated feature mask before doing save/restore.
     */
     
    ok = set_xcr0( XSTATE_FP_SSE);
    ASSERT(ok);
    xrstor(v, mask);
    ok = set_xcr0( XSTATE_FP_SSE);
    ASSERT(ok);
}




static inline uint64_t vcpu_xsave_mask(const struct vcpu *v)
_(requires \thread_local(v))
{
  
    if ( v->fpu_dirtied )
        return v->nonlazy_xstate_used ? XSTATE_ALL : XSTATE_LAZY;

    ASSERT(v->nonlazy_xstate_used);

    /*
     * The offsets of components which live in the extended region of
     * compact xsave area are not fixed. Xsave area may be overwritten
     * when a xsave with v->fpu_dirtied set is followed by one with
     * v->fpu_dirtied clear.
     * In such case, if hypervisor uses compact xsave area and guest
     * has ever used lazy states (checking xcr0_accum excluding
     * XSTATE_FP_SSE), vcpu_xsave_mask will return XSTATE_ALL. Otherwise
     * return XSTATE_NONLAZY.
     */
    return xstate_all(v) ? XSTATE_ALL : XSTATE_NONLAZY;
}



void vcpu_restore_fpu_lazy(struct vcpu *v)
_(requires \thread_local(v))
_(writes &v->fpu_initialised)
_(writes &v->fpu_dirtied)
{
    ASSERT(!is_idle_vcpu(v));
    int cpu_has_xsave = 1;
    /* Avoid recursion. */
    clts();

    if ( v->fpu_dirtied )
        return;

    ASSERT(!v->fully_eager_fpu);

    if ( cpu_has_xsave )
        fpu_xrstor(v, XSTATE_LAZY);
    else
        fpu_fxrstor(v);

    v->fpu_initialised = 1;
    v->fpu_dirtied = 1;
}






int xsave
(struct vcpu *v,int x)_(requires \true)
{}
//int is_idle_vcpu(struct vcpu *v)_(requires \true){}

int fpu_fxsave(struct vcpu *v)_(requires \true){}


//int clts()_(requires \true)
//{}
static inline void fpu_xsave(struct vcpu *v)
_(requires \true)
_(requires \thread_local(v))

{
    int ok;
    uint64_t mask = vcpu_xsave_mask(v);

    ASSERT(mask);
    ASSERT(v->xsave_area);
    /*
     * XCR0 normally represents what guest OS set. In case of Xen itself,
     * we set the accumulated feature mask before doing save/restore.
     */
//     _(ghost int XSTATE_FP_SSE );
    ok = set_xcr0(v->xcr0_accum | XSTATE_FP_SSE);
    ASSERT(ok);
    xsave(v, mask);
    ok = set_xcr0(v->xcr0 ?XSTATE_FP_SSE: XSTATE_FP_SSE);
    ASSERT(ok);
}



static int _vcpu_save_fpu(struct vcpu *v)
_(requires \true)
_(requires \thread_local(v))
_(writes &v->fpu_dirtied)
{
    if ( !v->fpu_dirtied && !v->nonlazy_xstate_used )
        return 0;

    ASSERT(!is_idle_vcpu(v));

    /* This can happen, if a paravirtualised guest OS has set its CR0.TS. */
     clts();
    _(ghost int cpu_has_xsave)
    if ( cpu_has_xsave )
        fpu_xsave(v);
    else
        fpu_fxsave(v);

    v->fpu_dirtied = 0;

    return 1;
}


int stts()_(requires \true)
{}

void vcpu_save_fpu(struct vcpu *v)
_(requires \thread_local(v))
_(writes &v->fpu_dirtied)
_(requires \true)
{
    _vcpu_save_fpu(v);
    stts();
}




void save_fpu_enable(struct vcpu *current)
_(requires \thread_local(current))
_(writes &current->fpu_dirtied)
{
    if ( !_vcpu_save_fpu(current) )
        {
          clts();}
}




























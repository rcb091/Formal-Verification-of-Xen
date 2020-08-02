#include <vcc.h>
#include <asm/debugger.h>

//_(ghost struct cpu_user_regs{ unsigned long rip; unsigned long eflags;})
typedef struct cpu_user_regs{ unsigned long rip; unsigned long eflags;};

unsigned short gdb_arch_signal_num(struct cpu_user_regs *regs, unsigned long cookie)
_(requires \true)
_(ensures \result == 5) 
{
    return 5;   /* TRAP signal.  see include/gdb/signals.h */
}

/*
 * Use __copy_*_user to make us page-fault safe, but not otherwise restrict
 * our access to the full virtual address space.
 */
 
unsigned int __copy_from_user(int  *dest, const int *src, unsigned len)
_(requires \true)
_(writes \array_range(dest, len))
_(requires \thread_local_array(src, len)) 
_(requires \arrays_disjoint(src, len, dest, len))
_(ensures \forall unsigned i; i < len==> dest[i] == \old(src[i])) 
{
  if (len > 0) {
   dest[len - 1] = src[len - 1];
   __copy_from_user(dest, src, len - 1);
}
  
}
 
 
unsigned int gdb_arch_copy_from_user(int *dest, const int *src, unsigned len)
_(requires \true)
_(writes \array_range(dest, len))
_(requires \thread_local_array(src, len)) 
_(requires \arrays_disjoint(src, len, dest, len))
_(ensures \forall unsigned i; i < len==> dest[i] == \old(src[i])) 
_(ensures \result >=0)
{
    return __copy_from_user(dest, src, len);
}



unsigned int __copy_to_user(int *dest, const int *src, unsigned len)
_(requires \true)
_(writes \array_range(dest, len))
_(requires \thread_local_array(src, len)) 
_(requires \arrays_disjoint(src, len, dest, len)) 
_(ensures \forall unsigned i; i < len==> dest[i] == \old(src[i])) 
{
  if (len > 0) {
   dest[len - 1] = src[len - 1];
   __copy_to_user(dest, src, len - 1);
}

}



unsigned int gdb_arch_copy_to_user(int *dest, const int *src, unsigned len)
_(requires \true)
_(writes \array_range(dest, len))
_(requires \thread_local_array(src, len)) 
_(requires \arrays_disjoint(src, len, dest, len)) 
_(ensures \result >= 0)
{
  
     return  __copy_to_user(dest, src, len);
   
}



void gdb_arch_print_state(struct cpu_user_regs *regs)
_(requires \true)
{
    /* XXX */
}

void gdb_arch_enter(struct cpu_user_regs *regs)
_(requires \true)
{
    /* nothing */
}

void gdb_arch_exit(struct cpu_user_regs *regs)
_(requires \true)
{
    /* nothing */
}



void gdb_arch_resume(struct cpu_user_regs *regs, unsigned long addr, unsigned long type, struct gdb_context *ctx)
_(requires \true)
_(requires \wrapped(regs))
_(writes &regs->rip)
_(writes &regs->eflags)
_(ensures \wrapped(regs))
{
    
    if ( addr != 1UL )
        regs->rip = addr;
    _(assert regs->rip == addr)
    
   // _(ghost unsigned long X86_EFLAGS_RF)
    //_(ghost unsigned long X86_EFLAGS_TF)
     unsigned long X86_EFLAGS_RF;
     unsigned long X86_EFLAGS_TF;

    regs->eflags &= ~X86_EFLAGS_TF;
    _(assert regs->eflags != \old(regs->eflags) && regs->eflags == (\old(regs->eflags) & (~X86_EFLAGS_TF)))

    /* Set eflags.RF to ensure we do not re-enter. */
    regs->eflags |= X86_EFLAGS_RF;
    _(assert regs->eflags != \old(regs->eflags) && regs->eflags == (\old(regs->eflags) | (~X86_EFLAGS_RF)))
    
    //_(ghost unsigned int GDB_STEP)
    unsigned int GDB_STEP;
    /* Set the trap flag if we are single stepping. */
    if ( type == GDB_STEP)
        regs->eflags |= X86_EFLAGS_TF;
        _(assert regs->eflags != \old(regs->eflags) && regs->eflags == (\old(regs->eflags) | (~X86_EFLAGS_TF)))
}

void printk(const char *fmt) {}
void printk(const char *fmt, const char *fmt1) {}
void printk(const char *fmt, unsigned int* fmt2, unsigned __int32 fmt3) {}
void printk(const char *fmt, const char *fmt1, struct thread* fmt2, const char *fmt3) {}

#ifdef SCHED_DEBUG
#define DEBUG(_f, _a...) \
    //printk("MINI_OS(file=sched.c, line=%d) " _f "\n", __LINE__, ## _a)
#else
#define DEBUG(_f, _a...)    ((void)0)
#endif

typedef unsigned int uint32_t;
typedef   signed long long int64_t;
typedef int64_t s_time_t;

#define STACK_SIZE_PAGE_ORDER 4
#define STACK_SIZE ((1UL << 12) * (1 << 4))

struct thread
{
    char *name;
    char *stack;
    /* keep in that order */
    unsigned long sp;  /* Stack pointer */
    unsigned long ip;  /* Instruction pointer */
    //MINIOS_TAILQ_ENTRY(struct thread) thread_list;
    uint32_t flags;
    s_time_t wakeup_time;
#ifdef HAVE_LIBC
    //struct _reent reent;
#endif
};

unsigned long alloc_pages(int order) { return 0; }

void dump_stack(struct thread *thread, unsigned long *pointer = NULL)
_(requires \thread_local(thread))
{
    unsigned long *bottom = (unsigned long *)(thread->stack + STACK_SIZE); 
    pointer = (unsigned long *)thread->sp;
    int count;
    struct thread* current = 0;
    if(thread == current)
    {
#ifdef __i386__    
        //asm("movl %%esp,%0"
          //  : "=r"(pointer));
#else
       // asm("movq %%rsp,%0"
         //   : "=r"(pointer));
#endif
    }
    printk("The stack for \"%s\"\n", thread->name);
    for(count = 0; count < 25 && pointer < bottom; count ++)
    {
        //printk("[0x%p] 0x%lx\n", pointer, *pointer);
        pointer++;
    }
    
    if(pointer < bottom) printk(" ... continues.\n");
}

/* Gets run when a new thread is scheduled the first time ever, 
   defined in x86_[32/64].S */
void thread_starter(void) {}

/* Pushes the specified value onto the stack of the specified thread */
static void stack_push(struct thread *thread, unsigned long value)
_(requires \wrapped(thread))
_(requires \mutable(thread))
_(ensures \wrapped(thread))
_(writes thread)
{
    _(unwrap thread)
    thread->sp = _(unchecked)(thread->sp - sizeof(unsigned long));
    *((unsigned long *)thread->sp) = value;
    _(wrap thread)
}

/* Architecture specific setup of thread creation */
struct thread* arch_create_thread(char *name, void (*function)(void *),
                                  void *data, struct thread *thread = NULL)
_(requires \wrapped(thread))
_(requires \mutable(thread))
_(ensures \wrapped(thread))
_(writes thread)
{
    //thread = xmalloc(struct thread);
    /* We can't use lazy allocation here since the trap handler runs on the stack */
    _(unwrap thread)
    thread->stack = (char *)alloc_pages(STACK_SIZE_PAGE_ORDER);
    thread->name = name;
    printk("Thread \"%s\": pointer: 0x%p, stack: 0x%p\n", name, thread, 
            thread->stack);
    
    thread->sp = (unsigned long)thread->stack + STACK_SIZE;
    /* Save pointer to the thread on the stack, used by current macro */
    *((unsigned long *)thread->stack) = (unsigned long)thread;

    /* Must ensure that (%rsp + 8) is 16-byte aligned at the start of thread_starter. */
    thread->sp -= sizeof(unsigned long);
    
    stack_push(thread, (unsigned long) function);
    stack_push(thread, (unsigned long) data);
    thread->ip = (unsigned long) thread_starter;
    _(wrap thread)
    return thread;
}

#define NULL 0
struct thread *idle_thread = NULL;

void run_idle_thread(void)
{
    /* Switch stacks and run the thread */ 
#if defined(__i386__)
    //__asm__ __volatile__("mov %0,%%esp\n\t"
      //                   "push %1\n\t" 
        //                 "ret"                                            
          //               :"=m" (idle_thread->sp)
            //             :"m" (idle_thread->ip));                          
#elif defined(__x86_64__)
    //__asm__ __volatile__("mov %0,%%rsp\n\t"
      //                   "push %1\n\t" 
        //                 "ret"                                            
          //               :"=m" (idle_thread->sp)
            //             :"m" (idle_thread->ip));                                                    
#endif
}

void local_irq_save(unsigned long) {}
void local_irq_restore(unsigned long) {}
void local_save_flags(unsigned long) {}
void local_irq_disable() {}
void local_irq_enable() {}

unsigned long __local_irq_save(void)
{
    unsigned long flags;

    local_irq_save(flags);
    return flags;
}

void __local_irq_restore(unsigned long flags)
{
    local_irq_restore(flags);
}

unsigned long __local_save_flags(void)
{
    unsigned long flags;

    local_save_flags(flags);
    return flags;
}

void __local_irq_disable(void)
{
    local_irq_disable();
}

void __local_irq_enable(void)
{
    local_irq_enable();
}

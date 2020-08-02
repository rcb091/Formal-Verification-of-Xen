

#ifdef HAVE_LIBC

#include <sys/lock.h>
#include <sched.h>
#include <wait.h>
#include <mini-os/lib.h>
#include <limits.h>

_(ghost struct _lock_t{ int busy; struct wait_queue_head wait;}) //ghost
typedef struct _lock_t _LOCK_T;

_(ghost struct _lock_recursive_t{ struct thread *owner; int count; struct wait_queue_head wait;}) //ghost
typedef struct _lock_recursive_t _LOCK_RECURSIVE_T;


//Following are the dummy functions which we have added as they are defined in different header files


static inline void init_waitqueue_head(struct wait_queue_head *h) {}
void local_irq_save(unsigned long t){}
void local_irq_restore(unsigned long t){}
void wake_up(struct wait_queue_head *h){}
struct _LOCK_RECURSIVE_T* get_current(void){}
void BUG_ON(bool b){}


//Following are the function declarations that were in the original file lock.c


int ___lock_init(_LOCK_T *lock);
int ___lock_acquire(_LOCK_T *lock);
int ___lock_try_acquire(_LOCK_T *lock);
int ___lock_release(_LOCK_T *lock);
int ___lock_init_recursive(_LOCK_RECURSIVE_T *lock);
int ___lock_acquire_recursive(_LOCK_RECURSIVE_T *lock);
int ___lock_try_acquire_recursive(_LOCK_RECURSIVE_T *lock);
int ___lock_release_recursive(_LOCK_RECURSIVE_T *lock);


//Following are the function definitions which we have verified with VCC using appropriate annotations 


//Function 1: ___lock_init is being used to legally initialize the value of lock variable of structure _LOCK_T

int ___lock_init(_LOCK_T *lock)
_(requires \mutable(lock))   //requires          
_(ensures lock->busy==0)     //ensures
{
    _(assume \writable(lock))//assume writable
     
    lock->busy = 0;
    
    _(assert lock->busy == 0) //assert
    
    init_waitqueue_head(&lock->wait);
    return 0;
}


/*Function 2: ___lock_acquire is being used to legally access the lock variable if its not busy or being used by other process
It basically blocks and only returns when it has the lock*/

int ___lock_acquire(_LOCK_T *lock)

_(requires \mutable(lock))  //requires
_(ensures lock->busy==1)    //ensures

{
    unsigned long flags;
    while(1)
    {
        local_irq_save(flags);
        
        _(assume \thread_local(lock)) //assume thread local
        
        if (!lock->busy)
        break;
        
        local_irq_restore(flags);
    }
    
    _(assume \writable(lock))        //assume writable
    
    lock->busy = 1;
    
    _(assert lock->busy == 1)       //assert
    
    local_irq_restore(flags);
    
    return 0;
}


/*Function 3: ___lock_try_acquire is being used to legally access the lock variable if its not busy or being used by other process
It is different from ___lock_acquire in the manner that it does not block but returns immediately and can either succeed or fail
to obtain the lock*/

int ___lock_try_acquire(_LOCK_T *lock)

_(requires \mutable(lock))                    //requires
_(ensures (lock->busy==0 ==> lock->busy==1))  //ensures

{
   unsigned long flags;
   int ret = -1;
   
   local_irq_save(flags);
    
   _(assume \thread_local(lock))  //assume thread local
   _(assume \writable(lock))      //assume writable
    
   if (!lock->busy)
   {
      lock->busy = 1;
      
      _(assert lock->busy == 1)  //assert
      
      ret = 0;
   }
   
   local_irq_restore(flags);
   
   return ret;
}


/*Function 4: ___lock_release is being used to properly release the  lock variable
so that it can be legally acquired by other processes*/

int ___lock_release(_LOCK_T *lock)

_(requires \mutable(lock))    //requires
_(ensures lock->busy==0)      //ensures

{
    unsigned long flags;
    local_irq_save(flags);
    
    _(assume \writable(lock)) //assume writable
    
    lock->busy = 0;
    
    _(assert lock->busy == 0) //assert
    
    wake_up(&lock->wait);
    local_irq_restore(flags);
    
    return 0;
}


//Function 5: ___lock_init_recursive is being used to legally initialize the lock variable if its not owned by any concurrent thread

int ___lock_init_recursive(_LOCK_RECURSIVE_T *lock)

_(requires \mutable(lock))       //requires
_(ensures lock->owner ==> NULL)  //ensures

{
     _(assume \writable(lock))   //assume writable
     
    lock->owner = NULL;
    init_waitqueue_head(&lock->wait);
    
    return 0;
}


//Function 6: ___lock_acquire_recursive is being used to acquire lock variable in a multi-threaded environment 

int ___lock_acquire_recursive(_LOCK_RECURSIVE_T *lock)

_(requires \mutable(lock))                       //requires
_(ensures (lock->count)==(\old(lock->count)+1))  //ensures

{
    unsigned long flags;
    
    _(assume \thread_local(lock))               //assume thread local

    if (lock->owner != get_current())
    {
        while (1)
        {
            local_irq_save(flags);
            if (!lock->owner)
            break;
            local_irq_restore(flags);
        }
        
        _(assume \writable(lock))              //assume writable
        
        lock->owner = get_current();
        local_irq_restore(flags);
    }
    
    _(assume \writable(lock))                 //assume writable
    _(assume lock->count<INT_MAX)             //assume
    
    lock->count++;
    
    return 0;
}


/*Function 7: ___lock_try_acquire_recursive is being used to acquire lock variable in a multi-threaded environment but
it functions similar to ___lock_try_acquire*/

int ___lock_try_acquire_recursive(_LOCK_RECURSIVE_T *lock)

_(requires \mutable(lock))                                         //requires
_(ensures ((lock->count)==(\old(lock->count)+1)||(lock->owner)))   //ensures

{
    unsigned long flags;
    int ret = -1;
    local_irq_save(flags);
    
    _(assume \thread_local(lock))                                 //assume thread local
     
    if (!lock->owner)
    {
        ret = 0;
        
        _(assume \writable(lock))                                //assume writable
          
        lock->owner = get_current();
        
        _(assume \writable(lock))                                //assume writable
        _(assume lock->count<INT_MAX)                            //assume
        
        lock->count++;
    }
    
    local_irq_restore(flags);
    
    return ret;
}


//Function 8: ___lock_release_recursive is being used to release lock variable in multi-threaded environment 

int ___lock_release_recursive(_LOCK_RECURSIVE_T *lock)

_(requires \mutable(lock))                  //requires
_(ensures \true)                            //ensures

{
    unsigned long flags;
    BUG_ON(lock->owner != get_current());
    
    _(assume \writable(lock))               //assume writable
    _(assume lock->count>0)                 //assume
    
    if (--lock->count)
    return 0;
    
    local_irq_save(flags);
    lock->owner = NULL;
    wake_up(&lock->wait);
    local_irq_restore(flags);
    
    return 0;
}


#endif
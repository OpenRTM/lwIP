/**
 * @file sys_arch.c
 * @brief lwIP OS abstraction layer for TOPPERS/ASP3 using acre_ APIs and DTQ for mailbox.
 */
#include "kernel.h"
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/err.h"
#include "arch/sys_arch.h"
#include <stdlib.h>
#include <string.h>
#include <kernel_cfg.h>

u32_t xInsideISR;
typedef struct {
    ID id;
    bool_t use;
} asp_sys_mtx_t;

 asp_sys_mtx_t asp_sys_mtx_list[] = {
    {LWIP_SYS_MTX1,false},{LWIP_SYS_MTX2,false},
    {LWIP_SYS_MTX3,false},{LWIP_SYS_MTX4,false},
    {LWIP_SYS_MTX5,false},{LWIP_SYS_MTX6,false},
    {LWIP_SYS_MTX7,false},{LWIP_SYS_MTX8,false}
};

typedef struct {
    ID id;
    bool_t use;
} asp_sys_sem_t;

asp_sys_sem_t asp_sys_sem_list[] = {
    {LWIP_SYS_SEM1,false},{LWIP_SYS_SEM2,false},
    {LWIP_SYS_SEM3,false},{LWIP_SYS_SEM4,false},
    {LWIP_SYS_SEM5,false},{LWIP_SYS_SEM6,false},
    {LWIP_SYS_SEM7,false},{LWIP_SYS_SEM8,false}
};

typedef struct{
    ID id;
    bool_t use;
    void (*thread_func)(void *thread_arg);
    void *thread_arg;
} asp_sys_thread_t;

asp_sys_thread_t asp_sys_thread_list[] = {
    {LWIP_SYS_TASK1,false,NULL,NULL},{LWIP_SYS_TASK2,false,NULL,NULL},
    {LWIP_SYS_TASK3,false,NULL,NULL},{LWIP_SYS_TASK4,false,NULL,NULL},
    {LWIP_SYS_TASK5,false,NULL,NULL},{LWIP_SYS_TASK6,false,NULL,NULL},
    {LWIP_SYS_TASK7,false,NULL,NULL},{LWIP_SYS_TASK8,false,NULL,NULL},
    {LWIP_SYS_TASK9,false,NULL,NULL},{LWIP_SYS_TASK10,false,NULL,NULL}
};

typedef struct {
    ID id;
    bool_t use;
} asp_sys_dtq_t;

asp_sys_dtq_t asp_sys_dtq_list[] = {
    {LWIP_SYS_DTQ1,false},{LWIP_SYS_DTQ2,false},
    {LWIP_SYS_DTQ3,false},{LWIP_SYS_DTQ4,false},
    {LWIP_SYS_DTQ5,false},{LWIP_SYS_DTQ6,false},
    {LWIP_SYS_DTQ7,false},{LWIP_SYS_DTQ8,false}
};

/**
 * @brief System Initialize.
 *
 */
void sys_init(void)
{

}
/**
 * @brief Get System uptime. unit ms
 */
u32_t sys_now()
{
    SYSTIM ctime;
    get_tim(&ctime);
    return (u32_t)(ctime/1000);
}

/**
 * @brief Create a new mutex.
 *
 * @param[out] mutex Pointer to the created mutex handle.
 * @return ERR_OK on success, ERR_MEM if allocation failed.
 */
err_t sys_mutex_new(sys_mutex_t *mutex)
{
    int i;
    *mutex = 0;
    for(  i = 0 ; i < sizeof(asp_sys_mtx_list)/sizeof(asp_sys_mtx_list[0]);i++)
    {
        asp_sys_mtx_t *elm = &asp_sys_mtx_list[i];
        if (!elm->use)
        {
            elm->use = true;
            *mutex = elm->id;
            break;
        }
    }
    if (*mutex <= 0){
        return ERR_MEM;
    }
    return ERR_OK;
  }

/**
 * @brief Lock the specified mutex.
 *
 * @param mutex The mutex to lock.
 */
void sys_mutex_lock(sys_mutex_t *mutex)
{
    loc_mtx(*mutex);
}

/**
 * @brief Unlock the specified mutex.
 *
 * @param mutex The mutex to unlock.
 */
void sys_mutex_unlock(sys_mutex_t *mutex)
{
    unl_mtx(*mutex);
}

/**
 * @brief Delete the specified mutex.
 *
 * @param mutex The mutex to delete.
 */
void sys_mutex_free(sys_mutex_t *mutex)
{
    int idx = *mutex - LWIP_SYS_MTX1;
    if ( 0 <= idx && idx < sizeof(asp_sys_mtx_list)/sizeof(asp_sys_mtx_list[0]))
    {
        unl_mtx(*mutex);
        asp_sys_mtx_t *elm = &asp_sys_mtx_list[idx];
        elm->use = false;
    }
    *mutex = 0;
}

/**
 * @brief Create a new semaphore.
 * @param count Initial count.
 * @return Semaphore ID.
 */
err_t sys_sem_new(sys_sem_t *sem,u8_t count) {
    int i;
    *sem = 0;
    for(  i = 0 ; i < sizeof(asp_sys_sem_list)/sizeof(asp_sys_sem_list[0]);i++)
    {
        asp_sys_sem_t *elm = &asp_sys_sem_list[i];
        if (!elm->use)
        {
            elm->use = true;
            *sem = elm->id;
            break;
        }
    }
    if (*sem <= 0){
        return ERR_MEM;
    }
    if(count == 0)
    {
       wai_sem(*sem);
    }
    return ERR_OK;
}


/**
 * @brief Delete a semaphore.
 * @param sem Semaphore ID.
 */
void sys_sem_free(sys_sem_t *sem) {
    int idx = *sem - LWIP_SYS_SEM1;

    if ( 0 <= idx && idx < sizeof(asp_sys_sem_list)/sizeof(asp_sys_sem_list[0]))
    {
        asp_sys_sem_t *elm = &asp_sys_sem_list[idx];
        if(pol_sem(*sem)!=E_OK)
        {
           sig_sem(*sem);
        }
        elm->use = false;
    }
    *sem = 0;
}

/**
 * @brief Signal a semaphore.
 * @param sem Semaphore ID.
 */
void sys_sem_signal(sys_sem_t *sem) {
    if(sns_ctx())
    {
        isig_sem(*sem);
    }else{
        sig_sem(*sem);
    }
}

/**
 * @brief Wait for a semaphore with timeout.
 * @param sem Semaphore ID.
 * @param timeout Timeout in milliseconds.
 * @return Time waited or SYS_ARCH_TIMEOUT.
 */
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout) {
    ER ret;
    SYSTIM start, end;
    get_tim(&start);

    if (timeout == 0) {
        ret = wai_sem(*sem);
    } else {
        ret = twai_sem(*sem, timeout);
    }

    get_tim(&end);

    if (ret == E_TMOUT) {
        return SYS_ARCH_TIMEOUT;
    }

    return (u32_t)(end - start);
}


/**
 * @brief Create a new mailbox using DTQ.
 * @param size Number of messages (used for buffer size).
 * @return DTQ ID or NULL.
 */
err_t sys_mbox_new(sys_mbox_t *mbox,int size) {
    int i;
    *mbox = 0;
    if (size > DEFAULT_MBOX_SIZE) return ERR_MEM;
    for(  i = 0 ; i < sizeof(asp_sys_dtq_list)/sizeof(asp_sys_dtq_list[0]);i++)
    {
        asp_sys_dtq_t *elm = &asp_sys_dtq_list[i];
        if (!elm->use)
        {
            elm->use = true;
            *mbox = elm->id;
            break;
        }
    }
    if (*mbox <= 0){
        return ERR_MEM;
    }
    return ERR_OK;
}

/**
 * @brief Delete a mailbox.
 * @param mbox DTQ ID.
 */
void sys_mbox_free(sys_mbox_t *mbox) {
    int idx = *mbox - LWIP_SYS_DTQ1;
    if( 0 <= idx  && idx < sizeof(asp_sys_dtq_list)/sizeof(asp_sys_dtq_list[0]))
    {
        asp_sys_dtq_t *elm = &asp_sys_dtq_list[idx];
        elm->use = false;
    }
}

/**
 * @brief Post a message to a mailbox (blocking).
 * @param mbox DTQ ID.
 * @param msg Message pointer.
 */
void sys_mbox_post(sys_mbox_t *mbox, void *msg) {
    snd_dtq(*mbox, (intptr_t)msg);
}

/**
 * @brief Try to post a message to a mailbox (non-blocking).
 * @param mbox DTQ ID.
 * @param msg Message pointer.
 * @return ERR_OK or ERR_MEM.
 */
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg) {
    ER ret = psnd_dtq(*mbox, (intptr_t)msg);
    return (ret == E_OK) ? ERR_OK : ERR_MEM;
}


/**
 * @brief Try to post a message to a mailbox (non-blocking). from isr
 * @param mbox DTQ ID.
 * @param msg Message pointer.
 * @return ERR_OK or ERR_MEM.
 */

err_t sys_mbox_trypost_fromisr(sys_mbox_t *mbox, void *msg) {
    return sys_mbox_trypost(mbox,msg);
}

/**
 * @brief Fetch a message from a mailbox with timeout.
 * @param mbox DTQ ID.
 * @param msg Pointer to store message.
 * @param timeout Timeout in milliseconds.
 * @return Time waited or SYS_ARCH_TIMEOUT.
 */
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout) {
    SYSTIM start, end;
    intptr_t data;
    get_tim(&start);

    ER ret = (timeout == 0) ? rcv_dtq(*mbox, &data) : trcv_dtq(*mbox, &data, timeout);
    get_tim(&end);

    if (ret == E_TMOUT) {
        *msg = NULL;
        return SYS_ARCH_TIMEOUT;
    }

    *msg = (void *)data;
    return (u32_t)(end - start);
}

/**
 * @brief Try to fetch a message from a mailbox (non-blocking).
 * @param mbox DTQ ID.
 * @param msg Pointer to store message.
 * @return 0 if success, SYS_MBOX_EMPTY if empty.
 */
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg) {
    intptr_t data;
    ER ret = prcv_dtq(*mbox, &data);
    if (ret == E_OK) {
        *msg = (void *)data;
        return 0;
    } else {
        *msg = NULL;
        return SYS_MBOX_EMPTY;
    }
}


void lwip_task(EXINF exinf)
{
    asp_sys_thread_t *thread;
    thread = &asp_sys_thread_list[exinf];
    while(1){
        slp_tsk();
        if(thread->use){
            if(thread->thread_func != NULL)
            {
                thread->thread_func(thread->thread_arg);
            }
        }
        thread->use=false;
    }
}

/**
 * @brief Create a new thread.
 * @param name Thread name (unused).
 * @param thread Thread function.
 * @param arg Argument to thread.
 * @param stacksize Stack size.
 * @param prio Priority.
 * @return Thread ID.
 */
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio) {
    sys_thread_t id;
    int i;

    id = 0;
    for(  i = 0 ; i < sizeof(asp_sys_thread_list)/sizeof(asp_sys_thread_list[0]);i++)
    {
        asp_sys_thread_t *elm = &asp_sys_thread_list[i];
        if (!elm->use)
        {

            elm->thread_func = thread;
            elm->thread_arg = arg;
            elm->use = true;
            id = elm->id;
            wup_tsk(id);
            break;
        }
    }
    return id;
}

/**
 * @brief Protect critical section.
 * @return Protection level.
 */
sys_prot_t sys_arch_protect(void) {
    loc_cpu();
    return 1;
}

/**
 * @brief Unprotect critical section.
 * @param pval Protection level.
 */
void sys_arch_unprotect(sys_prot_t pval) {
    if (pval) {
        unl_cpu();
    }
}

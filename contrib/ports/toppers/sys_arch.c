/**
 * @file sys_arch.c
 * @brief lwIP OS abstraction layer for TOPPERS/ASP3 using acre_ APIs and DTQ for mailbox.
 */
#include "kernel.h"
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/err.h"
#include "lwip/netif.h"
#include "arch/sys_arch.h"
#include <stdlib.h>
#include <string.h>
#include <kernel_cfg.h>
typedef void (*thread_func)(void *thread_arg);
extern void lwip_task(EXINF exinf);
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
    {LWIP_SYS_SEM1,false},    {LWIP_SYS_SEM2,false},
    {LWIP_SYS_SEM3,false},    {LWIP_SYS_SEM4,false},
    {LWIP_SYS_SEM5,false},    {LWIP_SYS_SEM6,false},
    {LWIP_SYS_SEM7,false},    {LWIP_SYS_SEM8,false},
    {LWIP_SYS_SEM9,false},    {LWIP_SYS_SEM10,false},
    {LWIP_SYS_SEM11,false},    {LWIP_SYS_SEM12,false},
    {LWIP_SYS_SEM13,false},    {LWIP_SYS_SEM14,false},
    {LWIP_SYS_SEM15,false},    {LWIP_SYS_SEM16,false},
    {LWIP_SYS_SEM17,false},    {LWIP_SYS_SEM18,false},
    {LWIP_SYS_SEM19,false},    {LWIP_SYS_SEM20,false},
    {LWIP_SYS_SEM21,false},    {LWIP_SYS_SEM22,false},
    {LWIP_SYS_SEM23,false},    {LWIP_SYS_SEM24,false},
    {LWIP_SYS_SEM25,false},    {LWIP_SYS_SEM26,false},
    {LWIP_SYS_SEM27,false},    {LWIP_SYS_SEM28,false},
    {LWIP_SYS_SEM29,false},    {LWIP_SYS_SEM30,false},
    {LWIP_SYS_SEM31,false},    {LWIP_SYS_SEM32,false},
    {LWIP_SYS_SEM33,false},    {LWIP_SYS_SEM34,false},
    {LWIP_SYS_SEM35,false},    {LWIP_SYS_SEM36,false},
    {LWIP_SYS_SEM37,false},    {LWIP_SYS_SEM38,false},
    {LWIP_SYS_SEM39,false},    {LWIP_SYS_SEM40,false},
    {LWIP_SYS_SEM41,false},    {LWIP_SYS_SEM42,false},
    {LWIP_SYS_SEM43,false},    {LWIP_SYS_SEM44,false},
    {LWIP_SYS_SEM45,false},    {LWIP_SYS_SEM46,false},
    {LWIP_SYS_SEM47,false},    {LWIP_SYS_SEM48,false},
    {LWIP_SYS_SEM49,false},    {LWIP_SYS_SEM50,false},
    {LWIP_SYS_SEM51,false},    {LWIP_SYS_SEM52,false},
    {LWIP_SYS_SEM53,false},    {LWIP_SYS_SEM54,false},
    {LWIP_SYS_SEM55,false},    {LWIP_SYS_SEM56,false},
    {LWIP_SYS_SEM57,false},    {LWIP_SYS_SEM58,false},
    {LWIP_SYS_SEM59,false},    {LWIP_SYS_SEM60,false},
    {LWIP_SYS_SEM61,false},    {LWIP_SYS_SEM62,false},
    {LWIP_SYS_SEM63,false},    {LWIP_SYS_SEM64,false},
    {LWIP_SYS_SEM65,false},    {LWIP_SYS_SEM66,false},
    {LWIP_SYS_SEM67,false},    {LWIP_SYS_SEM68,false},
    {LWIP_SYS_SEM69,false},    {LWIP_SYS_SEM70,false},
    {LWIP_SYS_SEM71,false},    {LWIP_SYS_SEM72,false},
    {LWIP_SYS_SEM73,false},    {LWIP_SYS_SEM74,false},
    {LWIP_SYS_SEM75,false},    {LWIP_SYS_SEM76,false},
    {LWIP_SYS_SEM77,false},    {LWIP_SYS_SEM78,false},
    {LWIP_SYS_SEM79,false},    {LWIP_SYS_SEM80,false},
    {LWIP_SYS_SEM81,false},    {LWIP_SYS_SEM82,false},
    {LWIP_SYS_SEM83,false},    {LWIP_SYS_SEM84,false},
    {LWIP_SYS_SEM85,false},    {LWIP_SYS_SEM86,false},
    {LWIP_SYS_SEM87,false},    {LWIP_SYS_SEM88,false},
    {LWIP_SYS_SEM89,false},    {LWIP_SYS_SEM90,false},
    {LWIP_SYS_SEM91,false},    {LWIP_SYS_SEM92,false},
    {LWIP_SYS_SEM93,false},    {LWIP_SYS_SEM94,false},
    {LWIP_SYS_SEM95,false},    {LWIP_SYS_SEM96,false},
    {LWIP_SYS_SEM97,false},    {LWIP_SYS_SEM98,false},
    {LWIP_SYS_SEM99,false},    {LWIP_SYS_SEM100,false},
    {LWIP_SYS_SEM101,false},    {LWIP_SYS_SEM102,false},
    {LWIP_SYS_SEM103,false},    {LWIP_SYS_SEM104,false},
    {LWIP_SYS_SEM105,false},    {LWIP_SYS_SEM106,false},
    {LWIP_SYS_SEM107,false},    {LWIP_SYS_SEM108,false},
    {LWIP_SYS_SEM109,false},    {LWIP_SYS_SEM110,false},
    {LWIP_SYS_SEM111,false},    {LWIP_SYS_SEM112,false},
    {LWIP_SYS_SEM113,false},    {LWIP_SYS_SEM114,false},
    {LWIP_SYS_SEM115,false},    {LWIP_SYS_SEM116,false},
    {LWIP_SYS_SEM117,false},    {LWIP_SYS_SEM118,false},
    {LWIP_SYS_SEM119,false},    {LWIP_SYS_SEM120,false},
    {LWIP_SYS_SEM121,false},    {LWIP_SYS_SEM122,false},
    {LWIP_SYS_SEM123,false},    {LWIP_SYS_SEM124,false},
    {LWIP_SYS_SEM125,false},    {LWIP_SYS_SEM126,false},
    {LWIP_SYS_SEM127,false},    {LWIP_SYS_SEM128,false},
    {LWIP_SYS_SEM129,false},    {LWIP_SYS_SEM130,false},
    {LWIP_SYS_SEM131,false},    {LWIP_SYS_SEM132,false},
    {LWIP_SYS_SEM133,false},    {LWIP_SYS_SEM134,false},
    {LWIP_SYS_SEM135,false},    {LWIP_SYS_SEM136,false},
    {LWIP_SYS_SEM137,false},    {LWIP_SYS_SEM138,false},
    {LWIP_SYS_SEM139,false},    {LWIP_SYS_SEM140,false},
    {LWIP_SYS_SEM141,false},    {LWIP_SYS_SEM142,false},
    {LWIP_SYS_SEM143,false},    {LWIP_SYS_SEM144,false},
    {LWIP_SYS_SEM145,false},    {LWIP_SYS_SEM146,false},
    {LWIP_SYS_SEM147,false},    {LWIP_SYS_SEM148,false},
    {LWIP_SYS_SEM149,false},    {LWIP_SYS_SEM150,false},
    {LWIP_SYS_SEM151,false},    {LWIP_SYS_SEM152,false},
    {LWIP_SYS_SEM153,false},    {LWIP_SYS_SEM154,false},
    {LWIP_SYS_SEM155,false},    {LWIP_SYS_SEM156,false},
    {LWIP_SYS_SEM157,false},    {LWIP_SYS_SEM158,false},
    {LWIP_SYS_SEM159,false},    {LWIP_SYS_SEM160,false},
    {LWIP_SYS_SEM161,false},    {LWIP_SYS_SEM162,false},
    {LWIP_SYS_SEM163,false},    {LWIP_SYS_SEM164,false},
    {LWIP_SYS_SEM165,false},    {LWIP_SYS_SEM166,false},
    {LWIP_SYS_SEM167,false},    {LWIP_SYS_SEM168,false},
    {LWIP_SYS_SEM169,false},    {LWIP_SYS_SEM170,false},
    {LWIP_SYS_SEM171,false},    {LWIP_SYS_SEM172,false},
    {LWIP_SYS_SEM173,false},    {LWIP_SYS_SEM174,false},
    {LWIP_SYS_SEM175,false},    {LWIP_SYS_SEM176,false},
    {LWIP_SYS_SEM177,false},    {LWIP_SYS_SEM178,false},
    {LWIP_SYS_SEM179,false},    {LWIP_SYS_SEM180,false},
    {LWIP_SYS_SEM181,false},    {LWIP_SYS_SEM182,false},
    {LWIP_SYS_SEM183,false},    {LWIP_SYS_SEM184,false},
    {LWIP_SYS_SEM185,false},    {LWIP_SYS_SEM186,false},
    {LWIP_SYS_SEM187,false},    {LWIP_SYS_SEM188,false},
    {LWIP_SYS_SEM189,false},    {LWIP_SYS_SEM190,false},
    {LWIP_SYS_SEM191,false},    {LWIP_SYS_SEM192,false},
    {LWIP_SYS_SEM193,false},    {LWIP_SYS_SEM194,false},
    {LWIP_SYS_SEM195,false},    {LWIP_SYS_SEM196,false},
    {LWIP_SYS_SEM197,false},    {LWIP_SYS_SEM198,false},
    {LWIP_SYS_SEM199,false},    {LWIP_SYS_SEM200,false},
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
    loc_mtx(LWIP_SYS_MTX_SYSTEM);
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
    unl_mtx(LWIP_SYS_MTX_SYSTEM);
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
    loc_mtx(LWIP_SYS_MTX_SYSTEM);

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
    unl_mtx(LWIP_SYS_MTX_SYSTEM);

    if (*sem <= 0){
        return ERR_MEM;
    }
    sig_sem(*sem);
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
        ret = twai_sem(*sem, timeout * 1000);
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

void sys_arch_mutex_lock()
{    
    loc_mtx(OPEN_RTM_THREAD_MTX);

}
void sys_arch_mutex_unlock()
{
    unl_mtx(OPEN_RTM_THREAD_MTX);
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
    loc_mtx(LWIP_SYS_MTX_SYSTEM);

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
    unl_mtx(LWIP_SYS_MTX_SYSTEM);

    return id;
}

int sys_thread_done(sys_thread_t *thread)
{
    int idx = *thread - LWIP_SYS_TASK1;
    if( 0 <= idx  && idx < sizeof(asp_sys_thread_list)/sizeof(asp_sys_thread_list[0]))
    {
        asp_sys_thread_t *elm=&asp_sys_thread_list[idx];
        return ~elm->use;
    }
 }

 uint64_t sys_arch_system_clock()
 {
    uint64_t tim;
    get_tim(&tim);
    return tim;
 }
 
 void sys_arch_version(int *major,int *minor, int* patch)
 {
    *major = (TKERNEL_PRVER >> 12) & 0x000F;
    *minor = (TKERNEL_PRVER >> 4) & 0x00FF;
    *patch = (TKERNEL_PRVER) & 0x0F;
 }

 void sys_arch_currentid(sys_thread_t *thread)
 {
    get_tid(thread);
 }
 
 int sys_arch_delay(unsigned int miliseconds)
 {
    return dly_tsk(miliseconds*1000); //sec->us;
 }

 int sys_arch_usleep( uint64_t us)
 {
    return  dly_tsk( us);
 }

 void sys_thread_sleep()
 {
    slp_tsk();
 }

 void sys_thread_wakeup(sys_thread_t *thread)
 {
    wup_tsk(*thread);
 }

 static char nonemac[6]={0};
 char *get_ifaddr()
 {
    struct netif *netif_ptr = netif_list;
    while (netif_ptr != NULL) {
    if(netif_ptr->ip_addr.addr!=0)
    {
      /* IP4 有効*/
      break;
    }
    netif_ptr = netif_ptr->next;
    
    return netif_ptr->hwaddr;
  }
  return nonemac;
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

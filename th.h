
#ifndef __th_h__
#define __th_h__    1

#include <pthread.h>
#include <ucontext.h>
#include <time.h>

struct lh {
    struct  lh          *next; 
    struct  lh          *prev;
};

static inline void l_init(struct lh * lh)
{ 
    lh->next = lh; 
    lh->prev = lh; 
}

static inline void l_add_raw(struct lh *item, struct lh *prev, struct lh *next)
{
    next->prev = item;
    item->next = next;
    item->prev = prev;
    prev->next = item;
}

static inline void l_add(struct lh *item, struct lh *lh)
{
    l_add_raw(item, lh, lh->next);
}

static inline void l_add_tail(struct lh *item, struct lh *lh)
{
    l_add_raw(item, lh->prev, lh);
}

static inline void l_del(struct lh *item)
{
    struct lh   *prev = item->prev;
    struct lh   *next = item->next;

    prev->next = next;
    next->prev = prev;
}

static inline int l_empty(struct lh *lh)
{
    return lh->next == lh;
}

#define l_first(lh)                 ((lh)->next)
#define l_entry(lh, type, member)   ((type *)((char *)lh - (char *)(&((type *)0)->member)))
 

struct cpu {
    ucontext_t          uc;
    pthread_t           pth;
    struct th           *run;
    struct lh           ready;
    struct lh           sleep;
    pthread_mutex_t     mutex;
    pthread_cond_t      cond;
    int                 id;
};

struct th {
    ucontext_t          uc;
    void                *(*func)(void *);
    void                *arg;
    int                 done;
    int                 cpu;
    struct timespec     sleep;
    struct lh           node;
};

struct lk {
    int                 locked;
    struct lh           wait;
    pthread_mutex_t     mutex;
};

struct cd {
    int                 val;
    struct lh           wait;
    pthread_mutex_t     mutex;
};

struct th *th_create(int cpu_id, void (*func)(struct th *), void *arg);
int th_join(struct th *th);
int th_lk_init(struct lk *lk);
int th_lk_lock(struct lk *lk);
int th_lk_unlock(struct lk *lk);
int th_cd_init(struct cd *cd);
int th_cd_wait(struct cd *cd);
int th_cd_signal(struct cd *cd);
int th_cd_bcast(struct cd *cd);

#endif /* __th_h__ */


// ml2 ft=c
#ifndef RTHREADS_H_
#define RTHREADS_H_

#include <stdlib.h>
#include <stdbool.h>
struct rthrd;

struct rthrd *rthrd_create(void *(*start_function)(void *), void *data);
struct rthrd *rthrd_current(void);
// Frees rthrd too
int rthrd_deatach(struct rthrd *rthrd);
void rthrd_exit(int res);
bool rthrd_equal(struct rthrd* first, struct rthrd* second);

#ifndef RTHREADS_IMPLEMENTATION


#if defined (__unix__) || defined (__APPLE__) && defined (__MACH__)
#define RTHREADS_POSIX
#define _POSIX_C_SOURCE 200809L

#include <pthread.h>

#else
#error "Unsupported operating system"
#endif


struct rthrd {
#ifdef RTHREADS_POSIX
    pthread_t ptrhead;
#endif // RTHREADS_POSIX
};

struct rthrd *rthrd_create(void *(*start_function)(void *), void *data) {
    struct rthrd thread;
#ifdef RTHREADS_POSIX
    if (pthread_create(&thread.ptrhead, NULL, start_function, data) != 0) {
        return NULL;
    }

#endif // RTHREADS_POSIX
    struct rthrd *ptr =(struct rthrd *) malloc(sizeof(thread));
    if (ptr == NULL) {
        return NULL;
    }

    *ptr = thread;
    return ptr;
}


bool rthrd_equal(struct rthrd* first, struct rthrd* second) {
#ifdef RTHREADS_POSIX
    return pthread_equal(first->ptrhead, second->ptrhead);
#endif // RTHREADS_POSIX
}

struct rthrd *rthrd_current(void) {

    struct rthrd *ptr = malloc(sizeof(struct rthrd));
    if (ptr == NULL) {
        return NULL;
    }
#ifdef RTHREADS_POSIX
    pthread_t pthrd = pthread_self();
    ptr->ptrhead = pthrd;
#endif // RTHREADS_POSIX
    return ptr;
}

void rthrd_exit(int res) {
#ifdef RTHREADS_POSIX
    int *ret = malloc(sizeof(int));
    *ret = res;
    pthread_exit(ret);
#endif // RTHREADS_POSIX
}

#endif // RTHREADS_IMPLEMENTATION
#endif // RTHREADS_H_

#include "threads.h"


typedef struct Semaphore_t {
     TCB_t *queue;
     int count;
} Semaphore_t;

Semaphore_t CreateSem(int InputValue) {
    Semaphore_t* newSem = (Semaphore_t*) malloc(sizeof(Semaphore_t));
    newSem->queue = newQueue();
    newSem->count = InputValue;
    return *newSem;
}

void P(Semaphore_t * sem) {
    sem->count--;
    if (sem->count < 0) {
        // block : add it to the queue
    
		addQueue(&(sem->queue),curr_Thread);
		TCB_t *prev_Thread;
		prev_Thread = curr_Thread;
		curr_Thread = delQueue(&readyQ);
    	swapcontext( &(prev_Thread->context), &(curr_Thread->context));
    }

}

void V(Semaphore_t * sem) {
    sem->count++;
    if (sem->count <= 0) {
        TCB_t *deleted_item;
        deleted_item = delQueue(&(sem->queue));
        addQueue(&readyQ,deleted_item);
    }
    yield();
}

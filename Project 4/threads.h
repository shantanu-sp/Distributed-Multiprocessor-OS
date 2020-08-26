#include "q.h"
#include<stdio.h>

TCB_t *readyQ;
TCB_t *curr_Thread;
int counter=0;

void start_thread(void (*func)(void)){
	void *stack=(void*)malloc(8192);
	TCB_t *tcb=(TCB_t*)malloc(sizeof(TCB_t));
	tcb=newItem();
	init_TCB(tcb, func, stack, 8192);
	tcb->thread_id=++counter;
	addQueue(&readyQ,tcb);
}

void run(){
	curr_Thread=delQueue(&readyQ);
	ucontext_t parent;
	getcontext(&parent);
	swapcontext(&parent,&(curr_Thread->context));	
}

void yield(){
	TCB_t *prev_Thread;
	addQueue(&readyQ,curr_Thread);
	prev_Thread=curr_Thread;
	curr_Thread=delQueue(&readyQ);
	swapcontext(&(prev_Thread->context),&(curr_Thread->context));
}

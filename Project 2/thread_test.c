// Shantanu Purandare 1217160516
// Faisal Alatawi 1215360666

#include<stdio.h>
#include "threads.h"
#include<unistd.h>

int glo=1;

void func1(){
	int loc=0;
	while(1){
		printf("Thread %d:\tin func1\nGlobal: %d, Local: %d\n\n",curr_Thread->thread_id,glo++,loc++);
		sleep(1);
		yield();
	}
}

void func2(){
	int local=10;
	while(1){
		printf("Thread %d:\tin func2\nGlobal: %d, Local: %d\n\n",curr_Thread->thread_id,glo++,local++);
		sleep(1);
		yield();
	}
}

void main(){
	readyQ=newQueue();
	start_thread(func1);
	start_thread(func2);
	start_thread(func1);
	start_thread(func2);
	run();
}

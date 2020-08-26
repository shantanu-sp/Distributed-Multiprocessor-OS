/*
Project teammates-

Shantanu Purandare, ASU ID- 1217160516
Girish Kumar Ethirajan, ASU ID- 1216305688

*/

#include <pthread.h> 
#include <stdio.h>
#include <stdlib.h>
#include "sem.h"
#include <unistd.h>


semaphore_t mutex,w_sem,r_sem;

int done = 0,arr[3]; 

void childwork(int *n) 
{ 
// This is the child thread function

	while(1) { 
				
				 
		P(&mutex); 
		if (done != *n) { 	
			V(&mutex);		
			P(&w_sem);
		} 
 
		
		if (done == 0) { 
			arr[*n]++;
			done = 1; 
			V(&w_sem);
		} else if(done == 1) {
			arr[*n]++; 
			done = 2; 
			V(&w_sem); 
		} else if(done == 2) { 
			arr[*n]++;
			done = 3;
			V(&r_sem); 
		}
				

		V(&mutex); 
		sleep(2);
	} 

} 


 
int main() 
{ 

// This is the parent(main) thread

	init_sem(&mutex, 1);
	init_sem(&w_sem, 0);
	init_sem(&r_sem, 0);
	

	arr[0]=0; arr[1]=0;arr[2]=0;		
	int n1 = 0, n2 = 1, n3 = 2; 
	
 	start_thread(childwork, &n1);
	start_thread(childwork, &n2);
	start_thread(childwork, &n3);

	while(1){
		P(&mutex); 
		if(done!=3){
			V(&mutex);
			P(&r_sem);
		}
		done=0;
		printf("Values %d %d %d\n",arr[0],arr[1],arr[2]);
		V(&w_sem);
		V(&mutex);
		sleep(1);
	}

	return 0; 
} 


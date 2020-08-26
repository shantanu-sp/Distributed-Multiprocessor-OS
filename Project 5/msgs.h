#include "sem.h"
#include <string.h> // TODO:


#define N 10 // number of messages
#define BUFF_SIZE 100 // Buffer size 



typedef struct port 
{
	int message[N];
    int reply_port[N];
    char buffer[N][BUFF_SIZE];

    int in,out;
	Semaphore_t mutex, empty, full;
}Port_t;

Port_t p[100];

void init_ports(int num){

   for (int i = 0; i<num; i++){
        p[i].mutex = CreateSem(1);
        p[i].full = CreateSem(0);
        p[i].empty = CreateSem(N);
		p[i].in=-1;
		p[i].out=0;
    }

}

void Send(int port_num, int reply_port_num, int m, char in_buff[]){
    P(&p[port_num].empty);
    P(&p[port_num].mutex);

    // printf("sending from %d to %d : m = %d \n", reply_port_num, port_num, m );

    p[port_num].in=(p[port_num].in+1)%N;
    int i = p[port_num].in; 

    p[port_num].message[i] = m;
    p[port_num].reply_port[i] = reply_port_num;
    p[port_num].buffer[i][0] = '\0'; 
    strcpy(p[port_num].buffer[i], in_buff);
    

    V(&p[port_num].mutex);
    V(&p[port_num].full);
}

void Receive(int port_num,int *reply_port_num,  int *m, char out_buff[]){
    P(&p[port_num].full);
    P(&p[port_num].mutex);



    int i = p[port_num].out; 
    *m = p[port_num].message[i];
    *reply_port_num = p[port_num].reply_port[i];
    strcpy(out_buff, p[port_num].buffer[i]);
    p[port_num].message[i] = -1;


    p[port_num].out=(p[port_num].out+1)%N;

    // printf("Receive from %d to %d : m = %d \n", *reply_port_num, port_num, *m );


    V(&p[port_num].mutex);
    V(&p[port_num].empty);
}


void printData(int port_num){
    printf("Port: %d { \n",p[port_num].in);
    for(int i = 0; i < p[port_num].in+1; i++ ){
        printf("\t\t message = %d, reply_port = %d \n ", p[port_num].message[i], p[port_num].reply_port[i] );
    }
    printf("}\n");
}

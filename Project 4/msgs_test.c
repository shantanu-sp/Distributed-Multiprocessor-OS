// Faisal Alatawi & Shantanu Purandare

#include <stdio.h>
#include "msgs.h"
#include <unistd.h>

Semaphore_t mutex; 
int count = 0;
int s_id = 1; 
int sp = 10; // server port 



void server(){

    
    while(1){
    
        int client_reply_port; 
        int client_message; 
        Receive(sp, &client_reply_port, &client_message );
        printf("> Server received a messages : (message: %d , reply port: %d) \n", client_message, client_reply_port);

        


        int new_message = client_message * 10; 

        Send(client_reply_port, sp, new_message);
        printf("> Server sent a messages : (message: %d , destination port: %d) \n", new_message, client_reply_port);

		sleep(1);
        printf("==============================\n");
    }

}

void client(){
    int index;

    P(&mutex);
    index = count; 
    count++; 
    V(&mutex); 

    int c_id;
    int rp; 
    int *messages;
    int size; 
	
	while(1){
    
		switch (index)
		{
		case 0:
		    c_id = 0;
		    rp = 20; 
		    int messages_1[9] = {1,2,3,4,5,6,7,8,9};
		    messages = messages_1;
		    size = 9; 
		    break;

		case 1:
		    c_id = 1;
		    rp = 30; 
		    int messages_2[9] = {11,12,13,14,15,16,17,18,19};
		    messages = messages_2;
		    size = 9; 
		    break;
	   
		default:
		    c_id = 2;
		    rp = 40; 
		    int messages_3[9] = {21,22,23,24,25,26,27,28,29};
		    messages = messages_3;
		    size = 9; 
		    break;
		}



		for(int i = 0; i < size; i++){
		    int msg = *(messages+i);
			printf("> Client %d sent a messages : (message: %d , destination port: %d) \n", c_id ,msg, sp);
		    Send(sp, rp, msg);
		    // printData(sp);
			
			sleep(1);

			int received_reply_port; 
	        int received_message; 
        
			Receive(rp, &received_reply_port, &received_message);
			printf("> Client %d received a messages : (message: %d , reply port: %d) \n", c_id ,received_message, received_reply_port);		

		}
        
    }

}


int main(){
    /* code */
    init_ports(100); 

    readyQ=newQueue();

	mutex = CreateSem(1);

	start_thread(client);
	start_thread(client);
	start_thread(client);
	start_thread(server);
	run();
	


    return 0;
}

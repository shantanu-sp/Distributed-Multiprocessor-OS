// Faisal Alatawi & Shantanu Purandare

#include "msgs.h"
#include <stdio.h>
#include <unistd.h>

#define MAX_FILE_NAME_SIZE 50
#define MAX_NUMBER_OF_FILES 50

//------------------------------------
//Modes 
#define MODE_CLIENT_NEW_FILE 10 
#define MODE_CLIENT_SEND_DATA 11
#define MODE_CLIENT_SEND_END_OF_FILE 12 


#define MODE_SERVER_BUSY 100
#define MODE_SERVER_NAME_TOO_LONG 101
#define MODE_SERVER_FILE_TOO_BIG 102
#define MODE_SERVER_START_SENDING 103
//------------------------------------


Semaphore_t mutex, terminate;
char file_names[MAX_NUMBER_OF_FILES][MAX_FILE_NAME_SIZE]; // 50 files with names that has 50 or less chars

int count = 1; // clients counter
int server_port = 0;

void server() {
  printf("Server %d started - port = %d\n", 1, server_port);

  int MAX_CLIENTS = 3; // The max number of clients a time 
  int current_clinets_num = 0; 
  
  FILE* fptr[MAX_CLIENTS];
  int fptr_idx[100];
  int fptr_i = 0; 

  /*
  Note: we make the assumption that 1 Megabyte = 1,000,000 byte
  Based on the International System of Units (SI) 

  - 1 char = 1 byte 
  */
  int file_size[MAX_CLIENTS];
  for(int i; i < MAX_CLIENTS; i++){
    file_size[i] = 0; 
  } 
  
  while (1) {
    int client_reply_port; 
    int mode;
    char buff[100];

    Receive(server_port, &client_reply_port, &mode, buff);
    // printf("server: client = %d , mode = %d, buff[0] = %c \n", client_reply_port, mode, buff[0] );


    switch (mode) {
    
      case MODE_CLIENT_NEW_FILE:  // new file

        // printf("New file request from : client %d\n", client_reply_port);
        
        if(current_clinets_num > MAX_CLIENTS){
          strcpy(buff, "The server is busy");
          Send(client_reply_port, server_port, MODE_SERVER_BUSY, buff);
        
          break;
        }

        
        char file_name[50];
        if(strlen(buff) > 15){ //TODO: test
          // Error: the name of the file is too long
          strcpy(buff, "The file name is too long (more than 15 char)");
          Send(client_reply_port, server_port, MODE_SERVER_NAME_TOO_LONG, buff);
          break;
        }

        current_clinets_num++; 
        strcpy(file_name, buff);
        strcat(file_name, ".server");


        fptr[fptr_i] = fopen(file_name, "w"); // get index
        fptr_idx[client_reply_port] = fptr_i;
        fptr_i++;


        strcpy(buff, "start sending the file");
        Send(client_reply_port, server_port, MODE_SERVER_START_SENDING, buff);

          
        break;

      case MODE_CLIENT_SEND_DATA: ;
        // new buffer
        int i_send = fptr_idx[client_reply_port];

        for(int j = 0; j < strlen(buff); j++){ 
            fputc(buff[j], fptr[i_send]);
            file_size[i_send]++;
        }
        
        if(file_size[i_send] <= 1000000 ){ // size >= 1MB
          strcpy(buff, "Send next data block");
          Send(client_reply_port, server_port, MODE_SERVER_START_SENDING, buff);
        } 
        else
        {
          strcpy(buff, "File is too big");
          Send(client_reply_port, server_port, MODE_SERVER_FILE_TOO_BIG, buff);

          //close file
          fclose(fptr[i_send]);

          //delete file TODO: implement
          
          fptr_i--; // free space for new file
          current_clinets_num--; // less clients to serve 
        }
        
        

        break;
      
      case MODE_CLIENT_SEND_END_OF_FILE: ;
        // done with the file 
        // close file
        // printf("yes we are here\n"); 
        int i_end = fptr_idx[client_reply_port];
        fclose(fptr[i_end]);
        
        
        fptr_i--; // free space for new file
        current_clinets_num--; // less clients to serve

        strcpy(buff, "DONE writing the file");
        int TEMP_MODE = 7; //TODO
        Send(client_reply_port, server_port, TEMP_MODE, buff);
        break;

      default:
        printf("ERROR <****************\n");
        break;
    }
  }
}

void client() {
  int client_id;
  int client_port;
  char client_file[MAX_FILE_NAME_SIZE];
  
  P(&mutex);
  client_id = count++;
  V(&mutex);

  client_port = client_id;
  strcpy(client_file, file_names[client_id - 1]); // -1 because count start at 1

  printf("Client %d started - port = %d\n", client_id, client_port);

  FILE *fptr=fopen(client_file,"r");
  //Step 1 : send a request for the to send a file
  char buff[100];
  strcpy(buff,client_file);

  printf("clinet %d want to send file\n", client_id);
  Send(server_port, client_port, MODE_CLIENT_NEW_FILE, buff);

  int received_reply_port; 
  int received_mode;
  

  while(1){
	  Receive(client_port, &received_reply_port, &received_mode, buff); 

	  printf("client %d: %s \n", client_id, buff);
	  int read_loc;

	  switch (received_mode)
	  {
	  case MODE_SERVER_START_SENDING:
        // start sending the file
        read_loc=fread(buff,1,100,fptr);
        // printf("read_loc = %d \n", read_loc);

        if (read_loc<100){
            // printf("read_loc = %d \n", read_loc);
            buff[read_loc]='\0';
        }
        Send(server_port, client_port, MODE_CLIENT_SEND_DATA, buff);
        if (read_loc<100){
            Send(server_port, client_port, MODE_CLIENT_SEND_END_OF_FILE, buff);
            // P(&terminate);
        }
          
		break;

	  default:
		    // P(&terminate);
        // exit(0);
        while(1){
          yield();
        }
		break;
	  }
  }
}

int main(int argc, char const *argv[]) {
  init_ports(100); 

  readyQ = newQueue();

  mutex = CreateSem(1);
  terminate=CreateSem(0);

  int num_clients = atoi(argv[1]); 

  if (num_clients <= 0) {
    printf("Please use provide at least one file name \n");
    return 1;
  }

  // copy files names to array
  for (int i = 0; i < num_clients; i++) {
    int file_idx = i + 2; 

    strcpy(file_names[i], argv[file_idx]);
    start_thread(client);

    
    // printf("%s \n", file_names[i]);
  }

  start_thread(server);
	
  run();

  return 0;
}

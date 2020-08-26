// Faisal Alatawi & Shantanu Purandare

#include "msgs.h"
#include <stdio.h>
#include <unistd.h>

#define MAX_FILE_NAME_SIZE 50
#define MAX_NUMBER_OF_FILES 50

//------------------------------------
//Modes 
#define MODE_CLIENT_NEW_FILE 0 
#define MODE_CLIENT_SEND_DATA 1
#define MODE_CLIENT_SEND_END_OF_FILE 2 


#define MODE_SERVER_BUSY 100
#define MODE_SERVER_NAME_TOO_LONG 101
#define MODE_SERVER_FILE_TOO_BIG 102
#define MODE_SERVER_START_SENDING 103
//------------------------------------


Semaphore_t mutex;
char file_names[MAX_NUMBER_OF_FILES][MAX_FILE_NAME_SIZE]; // 50 files with names that has 50 or less chars

int count = 1; // clients counter
int server_port = 0;

void server() {
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


    switch (mode) {
    
      case MODE_CLIENT_NEW_FILE:  // new file
        if(current_clinets_num >= MAX_CLIENTS){
          strcpy(buff, "The server is busy");
          Send(client_reply_port, server_port, MODE_SERVER_BUSY, buff);
        
          break;
        }
        current_clinets_num++; 
        
        char file_name[50];
        if(strlen(buff) > 15){ //TODO: test
          // Error: the name of the file is too long
          strcpy(buff, "The file name is too long (more than 15 char)");
          Send(client_reply_port, server_port, MODE_SERVER_NAME_TOO_LONG, buff);
        }

        strcpy(file_name, buff);
        strcat(file_name, ".server");


        fptr[fptr_i] = fopen(file_name, "w"); // get index
        fptr_idx[client_reply_port] = fptr_i;
        fptr_i++;


        strcpy(buff, "You can start sending");
        Send(client_reply_port, server_port, MODE_SERVER_START_SENDING, buff);

          
        break;

      case MODE_CLIENT_SEND_DATA: ;
        // new buffer
        int i_send = fptr_idx[client_reply_port];

        for(int j = 0; j < strlen(buff)-1; j++){ // TODO: -1????????????????????
            fputc(buff[j], fptr[i_send]);
            file_size[i_send]++;
        }
        
        if(file_size[i_send] >= 1000000 ){ // size >= 1MB
          strcpy(buff, "Next data block");
          Send(client_reply_port, server_port, MODE_SERVER_START_SENDING, buff);
        } else
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
        //close file
        int i_end = fptr_idx[client_reply_port];
        fclose(fptr[i_end]);
        
        
        fptr_i--; // free space for new file
        current_clinets_num--; // less clients to serve

        strcpy(buff, "DONE");
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
  client_id = counter;
  counter++;
  V(&mutex);

  client_port = client_id;
  strcpy(client_file, file_names[client_id - 1]); // -1 because count start at 1

  printf("Client %d started - port = %d\n", client_id, client_port);


  //Step 1 : send a request for the to send a file
  char buff[100] = "faisal";
  Send(server_port, client_port, MODE_CLIENT_NEW_FILE, buff);

  int received_reply_port; 
	int received_mode;
  printf("Before receive \n");
  Receive(client_port, &received_reply_port, &received_mode, buff); 

  printf("the mode = %d , buff = %s \n", received_mode, buff);


  switch (received_mode)
  {
  case MODE_SERVER_BUSY:
     // can not send at this time 
    break;
  
  case MODE_SERVER_FILE_TOO_BIG:
     // file is too big : error message 
    break;
  
  case MODE_SERVER_NAME_TOO_LONG:
     // name is too long : error message 
    break;
  
  case MODE_SERVER_START_SENDING:
     // start sending the file 
    break;

  default:
    break;
  }

}

int main(int argc, char const *argv[]) {
  init_ports(100); 

  readyQ = newQueue();

  mutex = CreateSem(1);

  int num_clients = argc - 2; 

  if (num_clients <= 0) {
    printf("Please use provide at least one file name \n");
    return 1;
  }

  // copy files names to array
  for (int i = 0; i < num_clients; i++) {
    int file_idx = i + 2; 

    strcpy(file_names[i], argv[file_idx]);

    printf("%s \n", file_names[i]);
  }

  start_thread(client); // 1
  start_thread(client); // 2
  start_thread(client); // 3
  start_thread(client); // 4
  start_thread(client); // 5

	start_thread(server);
	
  run();

  return 0;
}

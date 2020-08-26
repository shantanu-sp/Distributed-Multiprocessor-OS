#include "TCB.h"
#include <stdlib.h> 
#include <stdio.h>



TCB_t* newItem(){
    TCB_t* new_item = (TCB_t*) malloc(sizeof(TCB_t));
    new_item->next = NULL;
    new_item->prev = NULL;
    return new_item; 
}

TCB_t* newQueue(){
	TCB_t* head = NULL;
	return head;
}

void addQueue(TCB_t** head, TCB_t* item){

    if( (*head) == NULL){
        (*head) = item;
        item->next = item; 
        item->prev = item;  
    } else {
        TCB_t* tail = (*head)->prev;

        tail->next = item;

        (*head)->prev = item;

        item->next = (*head); 
        item->prev = tail;
    }
}

TCB_t* delQueue(TCB_t **head){
    TCB_t* deleted_item = NULL;

	if((*head)==NULL){
		printf("\nEmpty queue, cannot delete\n");	
	}
	else{

        deleted_item = (*head);

        if((*head)->next == deleted_item ){
            // There is only one item in the queue
            (*head) = NULL;
        }
        else{
            (*head) = deleted_item->next;
            TCB_t* tail = deleted_item->prev;

            (*head)->prev = tail;
            tail->next =  (*head);
        } 

    }

    return deleted_item;
}

void displayQueue(TCB_t** head){

    if((*head) == NULL){
        printf("The queue is empty \n");
    } else {
        TCB_t* next = (*head); 
        printf("The queue: \n");
        do
        {
            printf("item = %d \n", next->thread_id);
            next = next->next; 
        } while (next != (*head));
    }

}







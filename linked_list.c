#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "linked_list.h"

 
Node * add_newNode(Node* head, pid_t new_pid, char * new_path){
    Node* new_process = (Node*) malloc(sizeof(Node));
    char* tmp_path = (char*)malloc(sizeof(char*));
    new_process->pid = new_pid;
    strcpy(tmp_path, new_path);
    
    //remove the . for the command and store the new path
    char* token = strtok(tmp_path, ".");
    new_process->path = token;
    new_process->next = NULL;
    Node* temp = head;
        //empty list
        if (head == NULL) {
            head = new_process;
            return head;
        } else {
            while(temp->next != NULL){
                    temp = temp->next;
            }
            temp->next = new_process;
            return temp;
        }
    
}


Node * deleteNode(Node* head, pid_t pid){
    Node* temp = head;
    Node* prev = NULL;
    while(temp != NULL){
        if(temp->pid == pid){
            if(temp == head){
                head = head->next;
                printf("Process with pid %d was deleted.\n", pid);
                return head;
            }
            else{
                prev->next = temp->next;
            }
        }
        prev = temp;
        temp = temp->next;
    }
    printf("Error: Process %d does not exist or has already been deleted.\n", pid);
    return head;
}

void printList(Node *node){
    int count = 0;
    char cwd[256];
    getcwd(cwd, sizeof(cwd));
    while(node != NULL){
        printf("%d: %s%s \n", node->pid, cwd,node->path);
        node = node->next;
        count++;
    }
    printf("Total background jobs:  %d\n", count);
}


int PifExist(Node *node, pid_t pid){
    while(node != NULL){
        if (node->pid == pid){
            return 1;
        }
        node = node->next;
    }
      return 0;
}


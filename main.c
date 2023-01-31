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


Node* head = NULL;


void func_BG(char **cmd){
    pid_t pid = fork();
        if (pid == 0) {    
            char* command = cmd[1];
            execvp(command, &cmd[1]);
            printf("Error: Failed to execute command %s\n", command);
            exit(1);
        } else if (pid > 0) {
            printf("Started background process %d\n", pid);
            head = add_newNode(head, pid, cmd[1]);
        } else {
            printf("Error: Fork Failed");
        }
}

void func_BGlist(){
    printList(head);
}


void func_BGkill(char * str_pid){
    pid_t pid = atoi(str_pid);
    if (PifExist(head,pid) == 0){
        printf("Error: Process %d does not exist or has already been terminated.\n", pid);
        return;
    }
    if(kill(pid, SIGTERM) == 0){
        printf("Process %d terminated.\n", pid);
        head = deleteNode(head,pid);
    }
    else{
        printf("Error: Failed to execute bgkill.\n");
    }
}


void func_BGstop(char * str_pid){
    pid_t pid = atoi(str_pid);
    if (PifExist(head,pid) == 0){
        printf("Error: Process %d does not exist.\n", pid);
        return;
    }
    if(kill(pid, SIGSTOP) == 0){
        printf("Process %d stopped.\n", pid);
    }
    else{
        printf("Error: Failed to execute bgstop.\n");
    }
}


void func_BGstart(char * str_pid){
    pid_t pid = atoi(str_pid);
    if (PifExist(head,pid) == 0){
        printf("Error: Process %d does not exist.\n", pid);
        return;
    }
    if(kill(pid, SIGCONT) == 0){
        printf("Process %d resumed.\n", pid);
    }
    else{
        printf("Error: Failed to execute bgstart.\n");
    }
}


void func_pstat(char * str_pid){
    pid_t pid = atoi(str_pid);
    if (PifExist(head,pid) == 0){
        printf("Error: Process %d does not exist.\n", pid);
        return;
    }
    char statPath[256];
    char statusPath[256];
    sprintf(statPath, "/proc/%d/stat", pid);
    sprintf(statusPath, "/proc/%d/status", pid);
    FILE *statFile = fopen(statPath, "r");
    char stat_file_contents[256];
    if (statFile ==NULL){
        printf("Error: Cannot read stat file \n");
        return;
    }
    fgets(stat_file_contents, 256, statFile);
    
    char *stat_elements[256];
    char *stat_token;
    stat_token = strtok(stat_file_contents, " ");

    int i = 0;
    while (stat_token != NULL) {
        stat_elements[i] = stat_token;
        stat_token = strtok(NULL, " ");
        i++;
    }
    
    //print all stat elements and divide utime and stime
    float utime = (atof(stat_elements[13])/sysconf(_SC_CLK_TCK))*1000;
    float stime = (atof(stat_elements[14])/sysconf(_SC_CLK_TCK))*1000;
    printf("Status of process %d:\n", pid);
    printf("comm:\t%s\n", stat_elements[1]);
    printf("state:\t%s\n", stat_elements[2]);
    printf("utime:\t%f ms\n", utime);
    printf("stime:\t%f ms\n", stime);
    printf("rss:\t%s\n", stat_elements[23]);
    
    
    FILE *statusFile = fopen(statusPath, "r");
    char status_elements[256][256];
    
    //check if file is null or exists
    if (statusFile ==NULL){
        printf("Error: Cannot read status file \n");
        return;
    }
    int j;
    for(j=0; fgets(status_elements[j], 256, statusFile) != NULL;j++);
    
    //print context switches
    printf("%s", status_elements[54]);
    printf("%s", status_elements[55]);
   
}

void outsideKill(){
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    if (pid > 0){
        if(WIFEXITED(status)){
            printf("Background process %d has been terminated.\n",pid);
            head = deleteNode(head,pid);
        } else if (WIFSIGNALED(status)){
            printf("Background process %d has been killed. \n", pid);
            head = deleteNode(head,pid);
        }
    }
}

// given by instructor for input
int main(){
    char user_input_str[50];
    while (true) {
      printf("Pman: > ");
      fgets(user_input_str, 50, stdin);
      printf("User input: %s \n", user_input_str);
      char * ptr = strtok(user_input_str, " \n");
      if(ptr == NULL){
        continue;
      }
      char * lst[50];
      int index = 0;
      lst[index] = ptr;
      index++;
      while(ptr != NULL){
        ptr = strtok(NULL, " \n");
        lst[index]=ptr;
        index++;
      }
      if (strcmp("bg",lst[0]) == 0){
        func_BG(lst);
      } else if (strcmp("bglist",lst[0]) == 0) {
        func_BGlist(lst);
      } else if (strcmp("bgkill",lst[0]) == 0) {
        func_BGkill(lst[1]);
      } else if (strcmp("bgstop",lst[0]) == 0) {
        func_BGstop(lst[1]);
      } else if (strcmp("bgstart",lst[0]) == 0) {
        func_BGstart(lst[1]);
      } else if (strcmp("pstat",lst[0]) == 0) {
        func_pstat(lst[1]);
      } else if (strcmp("q",lst[0]) == 0) {
        printf("Bye Bye \n");
        exit(0);
      } else {
        printf("Invalid input\n");
      }
      outsideKill();
    }

  return 0;
}


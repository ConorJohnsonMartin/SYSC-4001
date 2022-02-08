#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include "shm_com.h"
#include <sys/types.h>

int main(){
    pid_t pid;
    int running = 1;

    void *shared_memory = (void *)0;
    struct shared_use_st *shared_stuff;
    char buffer[BUFSIZ];
    int shmid;
    shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);
    if (shmid == -1) {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }
    shared_memory = shmat(shmid, (void *)0, 0);
    if (shared_memory == (void *)-1) {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Memory attached at %X\n", (int)shared_memory);
    shared_stuff = (struct shared_use_st *)shared_memory;

    pid = fork();

    switch(pid)
    {
    case -1:
        perror("fork failed");
        exit(1);
    case 0:
        printf("This is the child\n");
        while(running){
            while(shared_stuff->new_int) {
                sleep(1);
                printf("waiting for client...\n");
            }
            shared_stuff->new_int = 1;
            shared_stuff->random_int = rand();
        }
    default:
        printf("This is the parent\n");
        while(running){
            while(!shared_stuff->new_int) {
                sleep(1);
                printf("waiting for new number...\n");
            }
            shared_stuff->new_int = 0;
            printf("Number: %d\n", shared_stuff->random_int);
        }
    }
}
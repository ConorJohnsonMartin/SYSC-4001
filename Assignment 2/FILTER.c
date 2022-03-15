#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <semaphore.h>
#include <sys/wait.h>

#define NUM_SEMS 7

//SYSC 4001 - Assignment 2 
//Author: Conor Johnson Martin 101106217
//Version: Version 1 - Submitted March 15th, 2022

sem_t bin_sem[7]; //7 semaphores for each character
int status; //put into waitpid()

int main()
{
	//Creates 7 semaphores
	int res;
	int lots_of_semaphores;
	for(lots_of_semaphores = 0; lots_of_semaphores < NUM_SEMS; lots_of_semaphores++) {
		res = sem_init(&bin_sem[lots_of_semaphores], 1, 1); //int sem_init: int _pshared == 1, int _value ==1
        if (res != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }

	pid_t pid1, pid2, pid3; //Process ID for child processes
	char debug[3]; //char array that is for the debug mode expected to be "yes" or "no"
	struct shared_use_st {char B[7];}; // struct with an int array of length 7 to be put into shared memory and store the array

	printf("\n-----------------------CONCURRENT FILTERING-----------------------\n");
	printf("\nEnter Debug Mode? (Insert yes or no): ");
	scanf("%s", &debug); //value from user that gets stored in char array debug
	printf("\n");

	//From Lab 3
	void *shared_memory = (void *)0;
	struct shared_use_st *shared_s;
	int shmid;
	srand((unsigned int)getpid());
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
	shared_s = (struct shared_use_st *)shared_memory;

	printf("Enter A String Of 7 Alpha-Numeric Characters (No Spaces): ");
	scanf("%s", &shared_s->B); //value from user that gets stored in shared memory to be accessed by all processes

	pid1 = fork(); //Child1 takes inputs 4,5,6 
	if (pid1 == 0) {
		int elements[3] = {4, 5, 6};
		while (!determineOrder(shared_s->B, bin_sem)) {
			if (swapOrder(shared_s->B, elements, bin_sem)) {
				printDebugMode("\nProcess P3: Performed Swapping", debug);
			}
			else {
				printDebugMode("\nProcess P3: No Swapping", debug);
			}
			sleep(1);
		}
		exit(EXIT_SUCCESS);
	}
	else {
		pid2 = fork(); //Child2 takes inputs 2,3,4
		if (pid2 == 0) {
			int elements[3] = {2, 3, 4};
			while (!determineOrder(shared_s->B, bin_sem)) {
				if (swapOrder(shared_s->B, elements, bin_sem)) {
					printDebugMode("\nProcess P2: Performed Swapping", debug);
				}
				else {
					printDebugMode("\nProcess P2: No Swapping", debug);
				}
				sleep(1);
			}
			exit(EXIT_SUCCESS);
		}
		else {
			pid3 = fork(); //Child3 takes inputs 0,1,2
			if (pid3 == 0) {
				int elements[3] = {0, 1, 2};
				while (!determineOrder(shared_s->B, bin_sem)) {
					if (swapOrder(shared_s->B, elements, bin_sem)) {
						printDebugMode("\nProcess P1: Performed Swapping", debug);
					}
					else {
						printDebugMode("\nProcess P1: No Swapping", debug);
					}
					sleep(1);
				}
				exit(EXIT_SUCCESS);
			}
		}
	}

	//Ensures all processes complete before program completes
	waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);
    waitpid(pid3, &status, 0);
	sleep(1);
	printf("\nFILTERED RESULT IS: %s\n", shared_s->B);
	printf("\n------------------------FILTERING COMPLETE------------------------\n");

	//From Lab 3
	if (shmdt(shared_memory) == -1) {
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}
	if (shmctl(shmid, IPC_RMID, 0) == -1) {
		fprintf(stderr, "shmctl(IPC_RMID) failed\n");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}

//A function that sorts through all 7 elements will set numberFound to 1 if the element is a numeral else element is a letter
int determineOrder(char B[7], sem_t sem[7]) {
    
	for (int n = 0; n < 7; n++){
        sem_wait(&bin_sem[n]); //semaphore waits
    }
    int numberFound = 0; //letter

    for (int n = 0; n < 7; n++) {
        if (isdigit(B[n])){
            numberFound = 1; //numeral
        }
        else {
            if (numberFound) {
                for (int n = 0; n < 7; n++) {
                    sem_post(&bin_sem[n]); //release semaphore
                }
                return 0;
            }
        }
    }

    for (int n = 0; n < 7; n++) {
        sem_post(&bin_sem[n]); //release semaphore
    }
    return 1;
}

//A function that swaps the order of the three elements depending if a numeral comes before a letter
int swapOrder(char B[7], int elements[3], sem_t sem[7]) {
	for (int n = 0; n < 3; n++){
        sem_wait(&bin_sem[n]); //semaphore waits
    }

	int test = 0;
	if (isdigit(B[elements[0]]) && !isdigit(B[elements[1]])) {
		char temp = B[elements[0]];
		B[elements[0]] = B[elements[1]];
		B[elements[1]] = temp;
		test = 1;
	}

	if (isdigit(B[elements[1]]) && !isdigit(B[elements[2]])) {
		char temp = B[elements[1]];
		B[elements[1]] = B[elements[2]];
		B[elements[2]] = temp;
		test = 1;
	}
	for (int n = 0; n < 3; n++) {
        sem_post(&bin_sem[n]); //release semaphore
    }
	return test;
}

//A funtion that prints the messages in main if debug has been set to "yes" and the start of main.
void printDebugMode(char string[], char debug[]) {
	if (strcmp(debug, "yes") == 0) {
		printf("%s\n", string);
	}
	return;
}
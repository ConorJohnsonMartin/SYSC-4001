#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/time.h>

#define MICRO_SEC_IN_SEC 1000000 //From Lab 2

//SYSC 4001 - Assignment 1 
//Author: Conor Johnson Martin 101106217
//Version: Version 1 - Submitted February 18th, 2022

//The main method for the COLLATZ.c class
int main(){
	struct shared_use_st{int S[3];}; //Struct that holds the results from the collatz function (i.e. # of steps)
	struct timeval start, end; //From Lab 2
	pid_t pid1, pid2, pid3; //Process ID for child processes
	// x is the int entered by a user (used to calculate 3 cases x, 2*x, 3*x)
	// minimum represents the lowest # steps between the 3 cases
	// maximum represents the greatest # of steps betwene 3 cases
	// average represents the average # steps between the 3 cases
	int x, minimum, maximum, average;
	
	printf("\n-----------------------COLLATZ CONJECTURE-----------------------\n");
	printf("\nEnter an integer that is greater than zero (i.e. positive): ");
	scanf("%d", &x); //Value from the user that gets stored in int x
	printf("\n");

	int C[3] = {x,2*x,3*x}; //Stores the 3 cases that will be assessed by the collatz function

	//From Lab 3
	void *shared_memory = (void *)0;
	struct shared_use_st *shared;
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
	shared = (struct shared_use_st *)shared_memory;

	//From Lab 2
	gettimeofday(&start, NULL);

	pid1 = fork(); //Child1
	if (pid1 == 0){
		shared->S[0] = collatz(C[0]); //Calcuates collatz conjecture of int x and stores it in the shared steps struct
		printf("Child Process 1 is working with collatz integer %d: This integer requires %d steps\n",  C[0], shared->S[0]);
		exit(EXIT_SUCCESS);
	} else {
		pid2 = fork(); //Child2
		if (pid2 == 0){
			shared->S[1] = collatz(C[1]); //Calcuates collatz conjecture of int 2*x and stores it in the shared steps struct
			printf("Child Process 2 is working with collatz integer %d: This integer requires %d steps\n",  C[1], shared->S[1]);
			exit(EXIT_SUCCESS);
		} else {
			pid3 = fork(); //Child3
			if (pid3 == 0){
				shared->S[2] = collatz(C[2]); //Calcuates collatz conjecture of int 3*x and stores it in the shared steps struct
				printf("Child Process 3 is working with collatz integer %d: This integer requires %d steps\n",  C[2], shared->S[2]);
				exit(EXIT_SUCCESS);
			}
		}
	}

	wait(); //ensures all processes complete before program completes
	gettimeofday(&end, NULL);

	minimum = shared->S[0]; //initializes minimum to the # of steps when collatz(x) ***Cannot be arbitrary ie. 0 or 100 must be one of the three cases
	maximum = shared->S[0]; //initializes maximum to the # of steps when collatz(x) ***Cannot be arbitrary ie. 0 or 100 must be one of the three cases

	for (int i = 0; i < 3; i++){
		if (shared->S[i] < minimum){
			minimum = shared->S[i]; //compares minimum to the # steps when collatz(2*x) and when collatz(3*x), will set new minimum if necessary
		}
		if (shared->S[i] > maximum){
			maximum = shared->S[i]; //compares maximum to the # steps when collatz(2*x) and when collatz(3*x), will set new maximum if necessary
		}
	}

	average = (shared->S[0]+shared->S[1]+shared->S[2])/3; //averages all 3 cases and saves it in average

	printf("\n");

	printf("The Minimum # of steps for collatz function: %d\n", minimum); //prints maximum # steps out of the three cases
	printf("The Maximum # of steps for collatz function: %d\n", maximum); //prints minimum # steps out of the three cases
	printf("The Average # of steps for collats function: %d\n", average); //prints average # steps out of the three cases

	printf("\n");

	printf("Time required to perfom all operations: %ld micro sec\n", ((end.tv_sec * MICRO_SEC_IN_SEC + end.tv_usec)-(start.tv_sec * MICRO_SEC_IN_SEC + start.tv_usec))); //prints the time it takes to perform all operations from start to end (Lab 2)

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

//Function that calculates the collatz conjecture and returns the # steps until n == 1.
int collatz(int n){
	int steps = 0;
	while(n != 1) {
		if (n % 2 == 0){ //if even
			n = n / 2;
			steps++;
		} else { //else (if odd)
			n = 3 * n + 1;
			steps++;
		}
	}
	return steps;
}
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <sys/time.h>

#define MICRO_SEC_IN_SEC 1000000

int main()
{
    pid_t pid;
    char *message;
    int n;
    struct timeval start, end;

    printf("fork program starting\n");
    gettimeofday(&start, NULL);
    pid = fork();
    gettimeofday(&end, NULL);   
    
    switch(pid)
    {
    case -1:
        perror("fork failed");
        exit(1);
    case 0:
        printf("Elapsed Time: %ld micro sec\n", ((end.tv_sec * MICRO_SEC_IN_SEC + end.tv_usec) - (start.tv_sec * MICRO_SEC_IN_SEC + start.tv_usec)));
        message = "This is the child";
        printf("Child process: the value returned by fork is: %d\n", getppid()); //Same as PID
        n = 20;
        break;
    default:
        message = "This is the parent";
        n = 12;
        printf("Parent PID: %d\n", getpid());
        break;
    }

    for(; n > 0; n--) {
        puts(message);
        sleep(1);
    }
    exit(0);
}

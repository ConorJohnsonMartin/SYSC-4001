#include <stdlib.h>
#include "a.h"
#include <stdio.h>

extern void function_two();
extern void function_three();

int main(){
    printf("main\n");
    function_two();
    function_three();
    exit(EXIT_SUCCESS);
}
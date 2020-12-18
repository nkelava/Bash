#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int get_args_count(char **args) 
{
    int counter;

    for(counter = 0; args[counter]; ++counter) {};

    return counter;
}


void free_args(char **args, int last_element_index) 
{
    int index;

    for(index = 0; index <= last_element_index; ++index) {
        args[index] = NULL;    
    }

    free(args);
    args = NULL;
}


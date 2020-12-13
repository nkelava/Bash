#include <stdio.h>
#include <string.h>


int get_args_count(char **args) {
    int counter;

    for(counter = 0; args[counter]; ++counter) {};

    return counter;
}


int get_args_size(char **args) {
    int index = 0, args_size = 0;

    for(index = 0; args[index]; ++index) {
        args_size += strlen(args[index]) * sizeof(char);
    }

    return args_size;
}




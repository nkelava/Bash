#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "arguments.h"

#define BASH_CWD_BUFSIZE    256


char *modify_path(char *args_path, char *base_directory)
{
    // Delete home/current directory representation character from path
    // Move 1 position if it's only ~. move 2 if it's ~/ or ./ or don't move at all if it's none of them
    // If it's none of them than add current dir path
    // Reason for moving two spaces if there is / on second index is because / is added at the end of base string
    int offset = (args_path[1] == '/') ? 2 : ((isalpha(args_path[0])) ? 0 : 1);
    const int args_path_len = strlen(args_path);
    char *full_path = (char*)malloc((args_path_len - offset + strlen(base_directory)) * sizeof(char));

    memmove(args_path, args_path + offset, args_path_len);        
    strcat(strcpy(full_path, base_directory), args_path);

    return full_path;    
}


void cut_cwd_path(char *cwd, int move_up_count)
{
    int last_slash_index = strlen(cwd) - 1;
    int counter = 0;
    
    while(counter < move_up_count) {
        while((cwd[last_slash_index] != '/')  && (last_slash_index > 0))
        {
            --last_slash_index;
        }
        --last_slash_index;
        ++counter;
    }
    cwd[last_slash_index + 2] = '\0';
}


int count_moving_up(char *args_path)
{
    int index = 3;
    int counter = 1;
    int args_path_len = strlen(args_path);

    while( (args_path[index] == '.') && (args_path[index + 1] == '.') &&
           (args_path[index + 2] == '/') && (index < args_path_len))
    {
        ++counter;     
        index += 3;
    }
    memmove(args_path, args_path + index, args_path_len);
    
    return counter;
}


void handle_path(char **args)
{
    // Handle "." cases
    if(args[1][0] == '.') {
        char cwd[BASH_CWD_BUFSIZE];
        getcwd(cwd, BASH_CWD_BUFSIZE);
        // Handle current working directory cases
        if(args[1][1] != '.') {
            strcpy(args[1], modify_path(args[1], strcat(cwd, "/")));
        }
        // Handling moving up('../../...') cases
        else {
            int move_up_count = count_moving_up(args[1]);

            cut_cwd_path(cwd, move_up_count);
            strcpy(args[1], strcat(cwd, args[1]));
        }
    }
    // Handle home directory cases
    else if(args[1][0] == '~') {
        int last_element_index = get_args_count(args) - 1;

        strcpy(args[1], modify_path(args[1], args[last_element_index]));
    }
    // Handle root directory case
    else {
        if(args[1][0] == '/' && strlen(args[1]) == 1) {
            strcpy(args[1], "/");        
        }
    }
}
 

   

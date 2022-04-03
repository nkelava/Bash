#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define BASH_CWD_BUFSIZE        256
#define BASH_RL_BUFSIZE         1024
#define BASH_TOK_BUFSIZE        64
#define BASH_TOK_DELIM          " \t\r\n\a"
#define COLOR_RED(string)       "\x1b[31m" string "\x1b[0m"
#define COLOR_YELLOW(string)    "\x1b[33m" string "\x1b[0m"

#include "builtins.h"
#include "arguments.h"
#include "path_handling.h"


int bash_launch(char **args)
{
	pid_t pid, wpid;
	int status;

	pid = fork();

	if(pid == 0) {
		// Child process
		if(execvp(args[0], args) == -1) {
			perror(COLOR_RED("bash"));
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		// Error forking
		perror(COLOR_RED("bash"));
	} else {
		// Parent process
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
    free(args);
	return 1;
}


int bash_execute(char **args)
{
	int index;
    int num_of_builtins = bash_builtins_count();
    
	if (!args[0]) {
		// Empty command was entered
		return 1;
	}

	for(index = 0; index < num_of_builtins; ++index) {
		if(strcmp(args[0], get_builtin_string(index)) == 0) {
            // Return get_builtin_function(i, args);
            return get_builtin_function(index, args);
		}
	}
	return bash_launch(args);
}


char **bash_split_line(char *line)
{
	int bufsize = BASH_TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char *));
	char *token;

	if(!tokens) {
		fprintf(stderr, COLOR_RED("bash") ": allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, BASH_TOK_DELIM);
	
    while(token != NULL) {
		tokens[position] = token;
		++position;

		if (position >= bufsize) {
			bufsize += BASH_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char *));

			if(!tokens) {
				fprintf(stderr, COLOR_RED("bash") ": allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, BASH_TOK_DELIM);
	}
	tokens[position] = NULL;
	
    return tokens;
}


char *bash_read_line()
{
	int bufsize = BASH_RL_BUFSIZE, position = 0, character;
	char *buffer = malloc(bufsize * sizeof(char));

	if(!buffer) {
		fprintf(stderr, COLOR_RED("bash") ": allocation error\n");
		exit(EXIT_FAILURE);
	}

	while(1) {
		character = getchar();

		// If EOF is hit, replace it with null char and return
		if ((character == EOF) || (character == '\n')) {
			buffer[position] = '\0';
			return buffer;
		}
		else {
			buffer[position] = character;
		}
		++position;

		// If we have exceeded the buffer, reallocate
		if (position >= bufsize) {
			bufsize += BASH_RL_BUFSIZE;
			buffer = realloc(buffer, bufsize * sizeof(char));

			if(!buffer) {
				fprintf(stderr, COLOR_RED("bash") ": allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}


void bash_loop()
{
	int status;
    int last_element_index;
	char *line, **args;
    char *cwd = malloc(BASH_CWD_BUFSIZE * sizeof(char));
    const char *home_directory = strcat(getenv("HOME"), "/");

    if(!cwd) {
        fprintf(stderr, COLOR_RED("bash") ": allocation error\n");
        exit(EXIT_FAILURE);
    }

	do {
        printf(COLOR_YELLOW("%s") "> ", getcwd(cwd, BASH_CWD_BUFSIZE));
		line = bash_read_line();
		args = bash_split_line(line);
        // Add home directory path at the end
        last_element_index = get_args_count(args);
        args[last_element_index] = (char *)home_directory;
        
        handle_path(args);
		status = bash_execute(args);

		free(line);
        line = NULL;
		free_args(args, last_element_index);
	} while(status);
}




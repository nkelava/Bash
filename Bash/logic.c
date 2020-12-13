#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define BASH_RL_BUFSIZE 1024
#define BASH_TOK_BUFSIZE 64
#define BASH_TOK_DELIM " \t\r\n\a"

#include "builtins.h"
#include "arguments.h"


int bash_launch(char **args) {
	pid_t pid, wpid;
	int status;

	pid = fork();

	if(pid == 0) {
		// Child process
		if(execvp(args[0], args) == -1) {
			perror("bash");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		// Error forking
		perror("bash");
	} else {
		// Parent process
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}


int bash_execute(char **args, const char *home_directory) {
	int index, num_of_builtins = bash_builtins_count();
    int last_element_index = get_args_count(args);
    int args_size = get_args_size(args);
    // Allocate more space for args than put home directory path at the end 
    args = realloc(args, (args_size + strlen(home_directory)) * sizeof(char *));
   
	if ((args[0] == NULL) || (args == NULL)) {
		// Empty command was entered or reallocation failed
		return 1;
	}

    args[last_element_index] = (char *)home_directory;
   
	for(index = 0; index < num_of_builtins; ++index) {
		if(strcmp(args[0], get_builtin_string(index)) == 0) {
            // Return get_builtin_function(i, args);
            return get_builtin_function(index, args);
		}
	}
	return bash_launch(args);
}


char **bash_split_line(char *line) {
	int bufsize = BASH_TOK_BUFSIZE, position = 0;
	char **tokens = (char **)malloc(bufsize * sizeof(char *));
	char *token;

	if(!tokens) {
		fprintf(stderr, "bash: allocation error\n");
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
				fprintf(stderr, "bash: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, BASH_TOK_DELIM);
	}
	tokens[position] = NULL;
	
    return tokens;
}


char *bash_read_line() {
	int bufsize = BASH_RL_BUFSIZE, position = 0, character;
	char *buffer = (char *)malloc(bufsize * sizeof(char));

	if(!buffer) {
		fprintf(stderr, "bash: allocation error\n");
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
			buffer = realloc(buffer, bufsize);

			if(!buffer) {
				fprintf(stderr, "bash: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}


void bash_loop() {
	char *line, **args;
	int status;
    const char *home_directory = strcat(getenv("HOME"), "/");

	do {
		printf("> ");
		line = bash_read_line();
		args = bash_split_line(line);
		status = bash_execute(args, home_directory);

		free(line);
		free(args);
	} while(status);
}




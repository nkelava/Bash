#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define SHELL_RL_BUFSIZE 1024
#define SHELL_TOK_BUFSIZE 64
#define SHELL_TOK_DELIM " \t\r\n\a"

#include "builtins.h"


int shell_launch(char **args) {
	pid_t pid, wpid;
	int status;

	pid = fork();

	if(pid == 0) {
		// Child process
		if(execvp(args[0], args) == -1) {
			perror("Shell");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		// Error forking
		perror("Shell");
	} else {
		// Parent process
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}


int shell_execute(char **args) {
	int i, num_of_builtins = shell_builtins_count();

	if (args[0] == NULL) {
		// Empty command was entered
		return 1;
	}

	for(i = 0; i < num_of_builtins; ++i) {
		if(strcmp(args[0], (const char *)get_builtin_string(i)) == 0) {
            return get_builtin_function(i, args);
		}
	}
	return shell_launch(args);
}


char **shell_split_line(char *line) {
	int bufsize = SHELL_TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char));
	char *token;

	if(!tokens) {
		fprintf(stderr, "Shell: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, SHELL_TOK_DELIM);
	while(token != NULL) {
		tokens[position] = token;
		++position;

		if (position >= bufsize) {
			bufsize += SHELL_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));

			if(!tokens) {
				fprintf(stderr, "Shell: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, SHELL_TOK_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}


char *shell_read_line() {
	int bufsize = SHELL_RL_BUFSIZE, position = 0, c;
	char *buffer = malloc(bufsize * sizeof(char));

	if(!buffer) {
		fprintf(stderr, "Shell: allocation error\n");
		exit(EXIT_FAILURE);
	}

	while(1) {
		c = getchar();

		// If EOF is hit, replace it with null char and return
		if ( c == EOF || c == '\n') {
			buffer[position] = '\0';
			return buffer;
		}
		else {
			buffer[position] = c;
		}
		++position;

		// If we have exceeded the buffer, reallocate
		if (position >= bufsize) {
			bufsize += SHELL_RL_BUFSIZE;
			buffer = realloc(buffer, bufsize);
			if(!buffer) {
				fprintf(stderr, "Shell: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}


void shell_loop() {
	char *line, **args;
	int status;

	do {
		printf("> ");
		line = shell_read_line();
		args = shell_split_line(line);
		status = shell_execute(args);

		free(line);
		free(args);
	} while(status);
}



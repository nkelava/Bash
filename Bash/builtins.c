#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h> 
#include <string.h>

#define BASH_DIR_BUFSIZE 1024

#include "arguments.h"


int bash_cd(char **args);
int bash_help(char **args);
int bash_exit(char **args);
int bash_whoami(char **args);
int bash_home(char **args);
int bash_programmer(char **args);
int bash_tilde(char **args);
int bash_pwd(char **args);
int bash_echo(char **args);
int bash_time(char **args);


char *builtin_str[] = {
	"cd",
	"help",
	"exit",
    "whoami",
    "programmer",
    "~",
    "home",
    "pwd",
    "echo",
    "time",
};


int (*builtin_func[])(char **) = {
	&bash_cd,
	&bash_help,
	&bash_exit,
    &bash_whoami,
    &bash_programmer,
    &bash_home,
    &bash_tilde,
    &bash_pwd,
    &bash_echo,
    &bash_time
};


const char *get_builtin_string(int index) {
    return builtin_str[index];
}


int get_builtin_function(int index, char **args) {
    return (*builtin_func[index])(args);    //  i-1
}


int bash_builtins_count() {
	return (sizeof(builtin_str)/sizeof(char *));
}


char *modify_path(char **args, char *base_directory) {
    // Delete home/current directory representation character from path
    // Move 1 position if it's only ~ or. and move 2 if it's ~/ or ./
    // Reason for moving two spaces if there is / on second index is because / is added at the end of base string
    int offset = (args[1][1] == '/') ? 2 : 1;
    const int argument_path_len = strlen(args[1]);
    char *full_path = (char*)malloc((argument_path_len - offset + strlen(base_directory)) * sizeof(char));

    memmove(args[1], args[1] + offset, argument_path_len);        

    strcat(strcpy(full_path, base_directory), args[1]);

    return full_path;    
}


int bash_cd(char **args) {
	if (args[1] == NULL) {
		fprintf(stderr, "bash: expected argument to \"cd\"\n");
	} else {
        // Swap ~ with full home directory location
        if (args[1][0] == '~') {
            int last_element_index = get_args_count(args) - 1;

            strcpy(args[1], modify_path(args, args[last_element_index]));
        }
        else if (args[1][0] == '.') {
            char cwd[BASH_DIR_BUFSIZE];
            getcwd(cwd, BASH_DIR_BUFSIZE);
            strcat(cwd, "/");

            strcpy(args[1], modify_path(args, cwd));
        }
		if(chdir(args[1]) != 0) {
			perror("bash");
		}
	}
	return 1;
}


int bash_help(char **args) {
	int index, num_of_builtins = bash_builtins_count();
	
    printf("Bash Shell\n");
	printf("Type program names and arguments, and hit enter.\n");
	printf("The following functions are built in: \n");

	for(index = 0; index < num_of_builtins; ++index) {
		printf(" %s\n", builtin_str[index]);
	}
	printf("User the man command for information on other programs.\n");
	
    return 1;
}


int bash_exit(char **args) {
	return 0;
}


int bash_whoami(char **args) {
    char* username = getenv("USER"); 
    
    printf("User: %s\n", username);

    return 1; 
}


int bash_programmer(char **args) {
    char *code_author = "Nikola Kelava";    
        
    printf("written by: %s\n", code_author);    
    
    return 1;
}


int bash_home(char **args) {
    int last_element_index = get_args_count(args) - 1;
    char *home_directory = args[last_element_index];

    printf("bash: %s: Is a home directory.\n", home_directory);
    
    return 1;
}


int bash_tilde(char **args) {
    bash_home(args);
}


int bash_pwd(char **args) {
    char cwd[BASH_DIR_BUFSIZE]; 
    
    getcwd(cwd, BASH_DIR_BUFSIZE); 
    printf("%s\n", cwd);
    
    return 1;
}


int bash_echo(char **args) {
    // If there is no argument to echo print empty string
    if (args[1] == NULL) {
		args[1] = "";
	}

    int index;
    int args_count = get_args_count(args) - 1;  // -1 because last element of args is home directory path
    
    for(index = 1; index < args_count; ++index) {
        printf("%s ", args[index]);    
    }
    printf("\n");

	return 1;
}


int bash_time(char **args) {
    srand(time(NULL));
    time_t current_time;

    time(&current_time);

    printf("Current time:  %s", ctime(&current_time));

    return 1;
}




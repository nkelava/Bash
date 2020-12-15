#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h> 
#include <string.h>

#define BASH_CWD_BUFSIZE        256
#define COLOR_RED(string)       "\x1b[31m" string "\x1b[0m"
#define COLOR_CYAN(string)      "\x1b[36m" string "\x1b[0m"

#include "arguments.h"
#include "path_handling.h"


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
    &bash_tilde,
    &bash_home,
    &bash_pwd,
    &bash_echo,
    &bash_time
};


const char *get_builtin_string(int index)
{
    return builtin_str[index];
}


int get_builtin_function(int index, char **args)
{
    return (*builtin_func[index])(args);
}


int bash_builtins_count()
{
	return (sizeof(builtin_str)/sizeof(char *));
}


int bash_cd(char **args)
{
	if (args[1] == NULL) {
		fprintf(stderr, COLOR_RED("bash") ": expected argument to \"cd\"\n");
	} 
    // Swap ~ or ./ with full home/current directory location
    else {
        // Handle home directory cases
        if (args[1][0] == '~') {
            int last_element_index = get_args_count(args) - 1;

            strcpy(args[1], modify_path(args[1], args[last_element_index]));
        }
        // Handle current working directory cases
        else {
            char cwd[BASH_CWD_BUFSIZE];
            getcwd(cwd, BASH_CWD_BUFSIZE);

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
		if(chdir(args[1]) != 0) {
			perror(COLOR_RED("bash"));
		}
	}
	return 1;
}


int bash_help(char **args)
{
	int index, num_of_builtins = bash_builtins_count();
	
    printf(COLOR_CYAN("Bash Shell") "\n");
	printf("Type program names and arguments, and hit enter.\n");
	printf("The following functions are built in: \n");

	for(index = 0; index < num_of_builtins; ++index) {
		printf(" %s\n", builtin_str[index]);
	}
	printf("User the man command for information on other programs.\n");
	
    return 1;
}


int bash_exit(char **args)
{
	return 0;
}


int bash_whoami(char **args)
{
    char* username = getenv("USER"); 
    
    printf("User: " COLOR_CYAN("%s") "\n", username);

    return 1; 
}


int bash_programmer(char **args) 
{
    char *code_author = "Nikola Kelava";    
        
    printf("Written by: " COLOR_CYAN("%s") "\n", code_author);    
    
    return 1;
}


int bash_home(char **args)
{
    int last_element_index = get_args_count(args) - 1;
    char *home_directory = args[last_element_index];

    printf("bash: %s: Is a home directory.\n", home_directory);
    
    return 1;
}


int bash_tilde(char **args)
{
    bash_home(args);
}


int bash_pwd(char **args)
{
    char *cwd = (char *)malloc(BASH_CWD_BUFSIZE);
    
    printf("%s\n", getcwd(cwd, BASH_CWD_BUFSIZE));
    
    return 1;
}


int bash_echo(char **args)
{
    // If there is no argument to then echo print empty string
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


int bash_time(char **args)
{
    srand(time(NULL));
    time_t current_time;

    time(&current_time);
    printf("Current time: %s", ctime(&current_time));

    return 1;
}




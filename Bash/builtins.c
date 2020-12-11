#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h> 


int bash_cd(char **args);
int bash_help(char **args);
int bash_exit(char **args);
int bash_whoami(char **args);
int bash_home(char **args);
int bash_tilde(char **args);
int bash_pwd(char **args);
int bash_echo(char **args);
int bash_time(char **args);


char *builtin_str[] = {
	"cd",
	"help",
	"exit",
    "whoami",
    "~",
    "home",
    "pwd",
    "echo",
    "time",
    "cat"
};


int (*builtin_func[])(char **) = {
	&bash_cd,
	&bash_help,
	&bash_exit,
    &bash_whoami,
    &bash_home,
    &bash_tilde,
    &bash_pwd,
    &bash_echo,
    &bash_time
};


const char *get_builtin_string(int i) {
    return builtin_str[i];
}


int get_builtin_function(int i, char **args) {
    return (*builtin_func[i])(args);
}


int bash_builtins_count() {
	return (sizeof(builtin_str)/sizeof(char *));
}


int bash_cd(char **args) {
	if (args[1] == NULL) {
		fprintf(stderr, "bash: expected argument to \"cd\"\n");
	} else {
		if(chdir(args[1]) != 0) {
			perror("bash");
		}
	}
	return 1;
}


int bash_help(char **args) {
	int i, num_of_builtins = bash_builtins_count();
	
    printf("Bash Shell\n");
	printf("Type program names and arguments, and hit enter.\n");
	printf("The following functions are built in: \n");

	for(i=0; i < num_of_builtins; ++i) {
		printf(" %s\n", builtin_str[i]);
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


int bash_home(char **args) {
    char *home_directory = getenv("HOME");

    printf("bash: %s: Is a home directory.\n", home_directory);
    
    return 1;
}


int bash_tilde(char **args) {
    bash_home(args);
}


int bash_pwd(char **args) {
    char cwd[1024]; 
    
    getcwd(cwd, sizeof(cwd)); 
    printf("%s\n", cwd);
    
    return 1;
}


int bash_echo(char **args) {
    if (args[1] == NULL) {
		args[1] = "";
	}

    int index = 0;

    while(args[++index]) {
        printf("%s ", args[index]);
    }
    printf("\n");

	return 1;
}


int bash_time(char **args) {
    srand(time(NULL));
    time_t current_time;

    time(&current_time);

    printf("Current time = %s", ctime(&current_time));

    return 1;
}




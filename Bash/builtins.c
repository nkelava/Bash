#include <stdio.h>
#include <unistd.h>


int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);


char *builtin_str[] = {
	"cd",
	"help",
	"exit"
};


int (*builtin_func[])(char **) = {
	&shell_cd,
	&shell_help,
	&shell_exit
};


const char *get_builtin_string(int i) {
    return builtin_str[i];
}


int get_builtin_function(int i, char **args) {
    return (*builtin_func[i])(args);
}


int shell_builtins_count() {
	return (sizeof(builtin_str)/sizeof(char *));
}


int shell_cd(char **args) {
	if (args[1] == NULL) {
		fprintf(stderr, "Shell: expected argument to \"cd\"\n");
	} else {
		if(chdir(args[1]) != 0) {
			perror("Shell");
		}
	}
	return 1;
}


int shell_help(char ** args) {
	int i, num_of_builtins = shell_builtins_count();
	printf("Bash Shell");
	printf("Type program names and arguments, and hit enter.\n");
	printf("The following functions are built in: \n");

	for(i=0; i < num_of_builtins; ++i) {
		printf(" %s\n", builtin_str[i]);
	}
	printf("User the man command for information on other programs.\n");
	
    return 1;
}


int shell_exit(char **args) {
	return 0;
}



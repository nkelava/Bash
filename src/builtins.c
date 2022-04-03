#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "arguments.h"

#define BASH_CWD_BUFSIZE        256
#define FILE_BUFFER_SIZE        1024
#define COLOR_RED(string)       "\x1b[31m" string "\x1b[0m"
#define COLOR_CYAN(string)      "\x1b[36m" string "\x1b[0m"

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
int bash_ls(char **args);
int bash_touch(char **args);
int bash_cat(char **args);
int bash_cp(char **args);
int bash_mv(char **args);


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
    "ls",
    "touch",
    "cat",
    "cp",
    "mv"
};


char *builtin_info[] = {
    "ispisuje trenutni direktorij u kojem se korisnik nalazi",
    "mijenja trenutni direktorij",
    "prijkazuje popis naredbi i njihove opcije",
    "izlazi iz ljuske",
    "ispisuje trenutno logiranog korisnika",
    "ispisuje ime developera projekta",
    "ispisuje putanju home direktorija",
    "ispisuje putanju home direktorija",
    "ispisuje tekst koji navedemo kao argument",
    "ispisuje trenutni datum i trenutno vrijeme",
    "ispisuje sadrzaj direktorija",
    "kreira datoteku",
    "ispisuje sadrzaj datoteke",
    "kopira datoteku iz jednog direktorija u drugi direktorij",
    "premjesta datoteku iz jednom u drugi direktorij"
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
    &bash_time,
    &bash_ls,
    &bash_touch,
    &bash_cat,
    &bash_cp,
    &bash_mv
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
	if (!args[1]) {
		fprintf(stderr, COLOR_RED("bash") ": expected argument to \"cd\"\n");
	} 
    // Swap ~ or ./ with full home/current directory location
    else {
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
	printf("The following functions are built in: \n\n");

	for(index = 0; index < num_of_builtins; ++index) {
		printf(" %s - %s\n", builtin_str[index], builtin_info[index]);
	}
	printf("\nUser the man command for information on other programs.\n");
	
    return 1;
}


int bash_exit(char **args)
{
	return 0;
}


int bash_whoami(char **args)
{
    const char* username = getenv("USER"); 
    
    printf("User: " COLOR_CYAN("%s") "\n", username);

    return 1; 
}


int bash_programmer(char **args) 
{
    const char *code_author = "Nikola Kelava";    
        
    printf("Written by: " COLOR_CYAN("%s") "\n", code_author);    
    
    return 1;
}


int bash_home(char **args)
{
    const int last_element_index = get_args_count(args) - 1;
    const char *home_directory = args[last_element_index];

    printf("bash: %s: Is a home directory.\n", home_directory);
    
    return 1;
}


int bash_tilde(char **args)
{
    bash_home(args);
}


int bash_pwd(char **args)
{
    char *cwd = malloc(BASH_CWD_BUFSIZE  * sizeof(char));

    if(!cwd) {
        fprintf(stderr, COLOR_RED("bash") ": allocation error\n");
        exit(EXIT_FAILURE);
    }
    
    printf("%s\n", getcwd(cwd, BASH_CWD_BUFSIZE));

    free(cwd);    
    return 1;
}


int bash_echo(char **args)
{
    int index;
    const int args_count = get_args_count(args) - 1;  // -1 because last element of args is home directory path

    // If there is no argument to then echo print empty string
    if (!args[1]) {
		args[1] = "";
	}
    
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


int bash_ls(char **args)
{
    DIR *dir_stream;
    struct dirent *dir_ptr;

    if(!(dir_stream = opendir(args[1]))) {
        perror(COLOR_RED("bash"));
        return 1;
    }
    
    while((dir_ptr = readdir(dir_stream)) != NULL)
    {
        if(dir_ptr->d_name[0] != '.') {
            printf("%s\n", dir_ptr->d_name);
        }
    }

    rewinddir(dir_stream);
    closedir(dir_stream);

    return 1;
}


int bash_touch(char **args)
{
    int index;
    const int args_count = get_args_count(args) - 1;
    
    for(index = 1; index < args_count; ++index) {
        FILE* file_ptr = fopen(args[index], "w+");

        if(!file_ptr) {
            fprintf(stderr, COLOR_RED("bash") ": couldn't create the file.\n");
        }

        fclose(file_ptr);
    }
    return 1;
}


int bash_cat(char **args)
{
    int index;
    const int args_count = get_args_count(args) - 1;

    for(index = 1; index < args_count; ++index) {
        FILE *file_ptr = fopen(args[index], "r");
        char *line = malloc(FILE_BUFFER_SIZE * sizeof(char));

        if(!file_ptr) {
            fprintf(stderr, COLOR_RED("bash") ": couldn't read the file.\n");
            return 1;
        }
        else if(!line) {
            fprintf(stderr, COLOR_RED("bash") ": allocation error\n");
            exit(EXIT_FAILURE);
        }
        else {
            while(fgets(line, FILE_BUFFER_SIZE, file_ptr)) {
                printf("%s", line);
            }
        }
        free(line);
        fclose(file_ptr);
    }
    return 1;
}


int bash_cp(char **args)
{
    char ch;
    int index;
    const int args_count = get_args_count(args) - 1;
    const int dest_file_index = args_count - 1;
    FILE *dest_file = fopen(args[dest_file_index], "a");

    
    if(args_count == 1) {
        fprintf(stderr, COLOR_RED("bash") ": expected arguments for %s command.\n", args[0]);
        return 1;
    }

    if(!dest_file) {
        fprintf(stderr, COLOR_RED("bash") ": couldn't find/open the file.\n");
        return 1;
     }

    for(index = 1; index < dest_file_index; ++index) {
        FILE *src_file = fopen(args[index], "r");

        if(!src_file) {
            fprintf(stderr, COLOR_RED("bash") ": couldn't find/read the file.\n");
            fclose(dest_file);
            return 1;
        }

        while((ch = fgetc(src_file)) != EOF) {
            fputc(ch, dest_file);
        }
        
        rewind(src_file);
        fclose(src_file);
    }

    fclose(dest_file);
    return 1;
}


int bash_mv(char **args)
{
    const int args_count = get_args_count(args) - 1;
    char *source = args[1], *destination = args[2];
    FILE *file_ptr;
    
    if(args_count != 3) {
        fprintf(stderr, COLOR_RED("bash") ": expected 2 arguments (source/destination) for %s command.\n", args[0]);
        return 1;
    }

    
    if(access(destination, F_OK)) {
        if(rename(source, destination) != 0) {
            fprintf(stderr, COLOR_RED("bash") ": cannot move %s to %s.\n", source, destination);
        }
    }
    else {
        char *dest_file_name = strrchr(destination, '/');
        fprintf(stderr, COLOR_RED("bash") ": cannot rename file %s to %s because %s already exists.\n", strrchr(source, '/'), dest_file_name, dest_file_name);
    }

    return 1;
}




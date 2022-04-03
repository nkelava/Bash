#ifndef BUILTINS_H
#define BUILTINS_H


const char *get_builtin_string(int index);
int get_builtin_function(int index, char **args);
int bash_builtins_count();


#endif




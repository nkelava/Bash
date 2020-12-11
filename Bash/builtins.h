#ifndef BUILTINS_H
#define BUILTINS_H


const char *get_builtin_string(int i);
int get_builtin_function(int i, char **args);
int bash_builtins_count();


#endif

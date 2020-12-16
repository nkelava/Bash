#ifndef PATH_HANDLING_H
#define PATH_HANDLING_H


char *modify_path(char *args_path, char *base_directory);
void cut_cwd_path(char *cwd, int move_count);
int count_moving_up(char *args_path);
void handle_path(char **args);


#endif




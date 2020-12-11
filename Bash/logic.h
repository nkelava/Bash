#ifndef LOGIC_H
#define LOGIC_H


int shell_launch(char **args);
int shell_execute(char **args);
char **shell_split_line(char *line);
char *shell_read_line();
void shell_loop();


#endif



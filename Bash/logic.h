#ifndef LOGIC_H
#define LOGIC_H


int bash_launch(char **args);
int bash_execute(char **args);
char **bash_split_line(char *line);
char *bash_read_line();
void bash_loop();


#endif

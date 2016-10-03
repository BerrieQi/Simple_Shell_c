#ifndef VE482P1_CMDS_H
#define VE482P1_CMDS_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <stdbool.h>

int cd(char *args);
int ls(int argcount,char **args);
int cat(int argcount,char **args);

#endif //VE482P1_CMDS_H

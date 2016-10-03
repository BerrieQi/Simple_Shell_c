#ifndef VE482P1_CMDS_H
#define VE482P1_CMDS_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

int cd(char **args);
int ls(char **args);
int cat(char **args);

#endif //VE482P1_CMDS_H

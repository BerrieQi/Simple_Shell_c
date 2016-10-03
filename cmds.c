#include "cmds.h"

int cd(char **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "Error: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("Error");
        }
    }
    return 1;
}

int ls(char **args)
{
    return 1;
}

#define BUFFER_SIZE 4096
int cat(char **args)
{
    int fd;
    if((fd = open(args[1],O_RDONLY))<0)
    {
        printf("open file failed!\n");
        exit(0);
    }
    char *buf = malloc(BUFFER_SIZE);
    int fsize;
    fsize=read(fd,buf,BUFFER_SIZE);
    write(STDOUT_FILENO,buf,fsize);
    close(fd);
    return 0;
}

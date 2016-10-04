#include "cmds.h"

#define MAX_ADDR 512

int cd(char *args)
{
    if (args == NULL) {
        fprintf(stderr, "Error: expected argument to \"cd\"\n");
    } else {
        if (chdir(args) != 0) {
            perror("Error");
        }
    }
    char *wd=malloc(100);
    getcwd(wd,MAX_ADDR);
    printf("%s\n",wd);
    return 1;
}

int ls(int argcount,char **args)
{
    char *path=malloc(MAX_ADDR);
    getcwd(path,MAX_ADDR);
    bool a=false,l=false;
    //if more than one flag
    if (argcount>=2)
    {
        for (int it=1; it<argcount; it++)
        {
            if (args[it][0]=='-')
            {
                if (strchr(args[1],'a')!=0) a=true;
                if (strchr(args[1],'l')!=0) l=true;
            }
            else
                path=args[it];
        }
    }
    DIR *dir;


    return 0;
}

#define BUFFER_SIZE 4096
int cat(int argcount,char **args)
{
    int fd;
    if((fd = open(args[1],O_RDONLY))<0)
    {
        perror("Cannot open target file ");
        exit(0);
    }
    char *buf = malloc(BUFFER_SIZE);
    int fsize;
    fsize=read(fd,buf,BUFFER_SIZE);
    write(STDOUT_FILENO,buf,fsize);
    close(fd);
    return 1;
}

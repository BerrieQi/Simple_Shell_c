#include "cmds.h"

#define MAX_ADDR 512
#define BUFFER_SIZE 4096

int cd(char *args)
{
    if (args == NULL) {
        fprintf(stderr, "Error: expected argument to \"cd\"\n");
    } else {
        if (chdir(args) != 0) {
            perror("Error");
        }
    }
    /*char *wd=malloc(100);
    getcwd(wd,MAX_ADDR);
    printf("%s\n",wd);*/
    return 1;
}

void mode_format( int mode , char* str )
{
    strcpy(str,"----------");
    if ( S_ISDIR(mode) )  str[0] = 'd';
    if ( S_ISCHR(mode) )  str[0] = 'c';
    if ( S_ISBLK(mode) )  str[0] = 'b';
    if ( mode & S_IRUSR ) str[1] = 'r';
    if ( mode & S_IWUSR ) str[2] = 'w';
    if ( mode & S_IXUSR ) str[3] = 'x';
    if ( mode & S_IRGRP ) str[4] = 'r';
    if ( mode & S_IWGRP ) str[5] = 'w';
    if ( mode & S_IXGRP ) str[6] = 'x';
    if ( mode & S_IROTH ) str[7] = 'r';
    if ( mode & S_IWOTH ) str[8] = 'w';
    if ( mode & S_IXOTH ) str[9] = 'x';
}


int ls(int argcount,char **args, int *redir)
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
    if(!(dir=opendir(path)))
    {
        printf("open dictionary failed!\n");
        exit(0);
    }
    struct dirent *rddir;

    while(rddir=readdir(dir))
    {
        if(((rddir->d_name)[0] == '.') && (!a))
            continue;
        if (!l)
            printf("%s\t",rddir->d_name);
        else
        {
            struct stat state;
            lstat(rddir->d_name,&state);
            char *change_time = malloc(BUFFER_SIZE);
            strftime(change_time,BUFFER_SIZE,"%F %H",localtime(&(state.st_mtime)));
            char *buf=malloc(11);
            mode_format(state.st_mode,buf);
            printf("%s\t%s\t%ld\t%s \n",buf,change_time,state.st_size,rddir->d_name);
        }
    }
    printf("\n");
    closedir(dir);
    return 0;
}

int cat(int argcount,char **args, int *redir)
{
    int fd;
    char *buf = malloc(BUFFER_SIZE);
    int fsize;
    if ((*redir)==0)
    {
        if((fd = open(args[1],O_RDONLY))<0)
        {
            perror("Cannot open target file ");
            exit(0);
        }
        fsize=read(fd,buf,BUFFER_SIZE);
    }
    else
    {
        fsize=read(STDIN_FILENO,buf,BUFFER_SIZE);
    }
    write(STDOUT_FILENO,buf,fsize);
    close(fd);
    return 1;
}

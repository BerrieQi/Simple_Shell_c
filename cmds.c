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

int pwd(int argcount,char **args, int *redir)
{
    char* path = malloc(MAX_ADDR);
    printf("%s\n",getcwd(path,MAX_ADDR));
    free(path);
    return 0;
}

int echo(int argcount,char **args, int *redir)//"" space
{
    int pos= 0;
    //According to the project, one line is no more than 1024 characters
    char **tokens = malloc(1024 * sizeof(char*));
    if (!tokens) {
        fprintf(stderr, "Error: Allocation error\n");
        exit(EXIT_FAILURE);
    }
    char *token;
    char *quote;
    char *quote_2;
    quote=strchr(line,'\"');
    if (quote)//find first "
    {
        *quote=0;//cut at the first place
        quote_2=strchr(quote+1,'\"');
        if (!quote_2)//cannot find another "
        {
            int ifinput=1;
            char *temp_char=malloc(MAX_LINE_CHAR);
            while (ifinput)
            {
                int n=MAX_LINE_CHAR;
                printf("> ");
                fgets(temp_char,n,STDIN_FILENO);
                quote_2=strchr(temp_char,'\"');
                if (quote_2)
                {
                    ifinput=0;
                    *quote_2=0;
                }
                strcat(quote+1,temp_char);
            }
        } else
        {
            *quote_2=0;//cut at the second place
        }
    }
    token=strtok(line, TOK_DELIM);
    while (token != NULL) {
        tokens[pos] = token;
        pos++;
        if (1024 <= pos) {
            fprintf(stderr, "ve482sh: Too long input\n");
            exit(EXIT_FAILURE);
        }
        token = strtok(NULL, TOK_DELIM);
    }
    if (quote)
    {
        tokens[pos]=quote+1;
        pos++;
        token=strtok(quote_2+1, TOK_DELIM);//possible run time error
        while (token != NULL) {
            tokens[pos] = token;
            pos++;
            if (1024 <= pos) {
                fprintf(stderr, "ve482sh: Too long input\n");
                exit(EXIT_FAILURE);
            }
            token = strtok(NULL, TOK_DELIM);
        }
        tokens[pos]=NULL;
    } else
    {
        tokens[pos] = NULL;
    }

    *args_num=pos;
    return tokens;


    int i;
    for (i=1;i<argcount;i++)
        printf("%s",args[i]);
    printf("\n");
    return 0;
}
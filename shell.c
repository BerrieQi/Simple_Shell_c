#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include "cmds.h"

#define MAX_LINE_CHAR 1024
#define MAX_ADDR 512

static int stdin_fd,stdout_fd;
static char *current_base_path;

pid_t child_pid=-1;

//List of commands, followed by their corresponding functions.
char *com_str[] = {
        "ls",
        "cat",
        "pwd",
        "echo",
        "mkdir",
        "rm"
};

int (*com_func[]) (int ,char **, int *, int *) = {
        &ls,
        &cat,
        &pwd,
        &echo,
        &sh_mkdir,
        &rm
};

int num_arg()
{
    return sizeof(com_str) / sizeof(char *);//find how many pointers in pointer array for iteration
}

void Kill_all(int signum)
{
    kill(child_pid,SIGTERM);
    printf("\n");
    return;
}

#define TOK_DELIM " \t\n\r\a"
char **split_line(char *line, int *args_num, int *ifredir) {
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
                fgets(temp_char, n, stdin);
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
}

void execute(char *command, int *ifredir, int *ifpipe)
{
    char **args;
    int * args_num=malloc(sizeof(int));//number of args in this command
    args=split_line(command,args_num,ifredir);//how many internal functions we have
    int i;
    int argumentcount=num_arg();
    for (i = 0; i < argumentcount; i++) {
        if (strcmp(args[0], com_str[i]) == 0) {
            (*com_func[i])(*args_num,args,ifredir,ifpipe);
            free(args_num);
            exit(EXIT_FAILURE);//if it is internal command
        }
    }
    if (execv(args[0],args)<0)
    {
        char *path=malloc(MAX_ADDR);
        strcpy(path,current_base_path);
        if (strcmp(path,"/")) strcat(path,"/");//not in root
        strcat(path,args[0]);//write the complete path to the external function
        if (execv(path,args)<0)
        {
            perror("Command not found ");
            free(args_num);
            exit(EXIT_FAILURE);
        }
    }
}

void parse_command(char *command, int *ifredir, int *ifpipe)
{
    char *temp_cmd=malloc(MAX_LINE_CHAR);
    if (sscanf(command,"%s",temp_cmd)<=0) {
        return; //empty line
    }
    char *split;
    split=strpbrk(command,"|<>");
    if (split)
    {
        if ((*split)=='|') {
            *split = 0;//set a split sign \0
            /**********************************memory and here
            char *next_fore=malloc(MAX_ADDR);
            if (sscanf(split+1,"%s",next_fore)<=0 && sscanf(command,"%s",temp_cmd)>0)
            {
                int ifinput=1;
                while (ifinput)
                {
                    printf("> ");
                    if (scanf("%s",next_fore)>0) ifinput=0;
                    getchar();//get the enter
                }
            }
               */
            int file_dscp[2];
            pipe(file_dscp);
            int pid = fork();
            child_pid=getpid();
            if (pid == 0) {
                //child
                dup2(STDIN_FILENO,file_dscp[0]);//close child proc read
                dup2(file_dscp[1],STDOUT_FILENO);//stdout will be file_dscp[1]
                *ifpipe=1;
                execute(command,ifredir,ifpipe);//exe command before |
            }
            else if (pid>0)
            {
                //parent
                dup2(STDOUT_FILENO,file_dscp[1]);//close parent write
                dup2(file_dscp[0],STDIN_FILENO);//stdin will be file_dscp[0]
                *ifpipe=1;
                parse_command(split+1,ifredir,ifpipe);//keep exe next part of command
            } else
            {
                perror("fork");
            }
        }
        else if ((*split)=='>')//">"
        {
            *split=0;//another cut
            int ifappend=0;
            if (*(split+1)=='>') {
                ifappend = 1;
                split = split + 1;
            }
            char *fout_name=malloc(MAX_ADDR);
            if (sscanf(split+1,"%s",fout_name)<=0 && sscanf(command,"%s",temp_cmd)>0)
            {
                int ifinput=1;
                while (ifinput)
                {
                    printf("> ");
                    if (scanf("%s",fout_name)>0) ifinput=0;
                    getchar();//get the enter
                }
            }
            int fout;
            if (!ifappend)
                fout = open(fout_name, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH| S_IWOTH);
            else
                fout = open(fout_name, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH| S_IWOTH);
            if (fout==-1)
            {
                perror("Cannot create or open target file ");
                return;
            } else
            {
                close(STDOUT_FILENO);
                dup(fout);
                parse_command(command,ifredir,ifpipe);//after set output to file then continue
                return;
            }


        } else//"<"
        {
            *split=0;//another cut
            char *split_2;
            char *fin_name;
            fin_name = malloc(MAX_ADDR);
            split_2=strpbrk(split+1,"|<>");
            if (split_2)
            {
                char temp=*split_2;
                *split_2=0;//temporary cut to get out file name
                sscanf(split+1,"%s",fin_name);
                *split_2=temp;
                strcat(command,split_2);
                //might need to first take out then do modification
            } else sscanf(split+1,"%s",fin_name);
            int fin;
            fin = open(fin_name,O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH| S_IWOTH);
            if (fin==-1)
            {
                perror("Open input file error ");
                return;
            } else
            {
                *ifredir=1;
                close(STDIN_FILENO);
                dup(fin);
                parse_command(command,ifredir,ifpipe);
                return;
            }
        }
    } else
    {
        int pid;
        pid=fork();
        child_pid=pid;
        //printf("par %d\n",child_pid);
        if (pid==0) execute(command,ifredir,ifpipe);//child one command
        else if (pid>0) //parent when one command wait and exit
        {
            int *status=NULL;
            //redirect the input and output and set pipe to not
            //bug here, too early to reset pipe or redirection
            //still bug about <>|
            *ifredir=0;
            *ifpipe=0;
            dup2(stdin_fd,STDIN_FILENO);
            dup2(stdout_fd,STDOUT_FILENO);
            waitpid(pid,status,WUNTRACED);
        } else
        {
            perror("fork");
        }
    }
    return;
}



int main()
{
    stdin_fd=dup(STDIN_FILENO);
    stdout_fd=dup(STDOUT_FILENO);
    char *command=malloc(MAX_LINE_CHAR);
    char *ifexit=malloc(MAX_LINE_CHAR);
    current_base_path=malloc(MAX_LINE_CHAR);
    current_base_path="/usr/bin";
    signal(SIGINT,Kill_all);
    do
    {
        child_pid=getpid();
        //printf("main %d \n",child_pid);
        char *mightpath=malloc(MAX_ADDR);
        int *ifredir=malloc(1*sizeof(int));
        int *ifpipe=malloc(1*sizeof(int));
        *ifredir=0;
        *ifpipe=0;
        printf("ve482sh $ ");
        size_t char_num;
        if (getline(&command,&char_num,stdin)<0)//ctrl D
        {
            printf("\n");
            break;
        }
        sscanf(command,"%s%s",ifexit,mightpath);
        int isCD=0;
        if (!strcmp(ifexit,"exit")) exit(EXIT_SUCCESS);//exit the shell
        else
        if (!strcmp(ifexit,"cd"))
        {
            cd(mightpath);
            isCD=1;
        }
        if (!isCD) parse_command(command,ifredir,ifpipe);
        free(mightpath);
    }while (1);
}

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "cmds.h"

#define MAX_LINE_CHAR 1024
#define MAX_ADDR 512

static int stdin_fd,stdout_fd;
static char *current_base_path;

//List of commands, followed by their corresponding functions.
char *com_str[] = {
        "ls",
        "cat"
};

int (*com_func[]) (int argcount,char **) = {
        &ls,
        &cat
};

int num_arg()
{
    return sizeof(com_str) / sizeof(char *);//find how many pointers in pointer array for iteration
}

#define TOK_DELIM " \t\n\r\a"
char **split_line(char *line)
{
    int pos= 0;
    //According to the project, one line is no more than 1024 characters
    char **tokens = malloc(1024 * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "Error: Allocation error\n");
        exit(EXIT_FAILURE);
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
    tokens[pos] = NULL;
    return tokens;
}

void execute(char *command)
{
    char **args;
    args=split_line(command);
    int i;
    int argumentcount=num_arg();
    for (i = 0; i < argumentcount; i++) {
        if (strcmp(args[0], com_str[i]) == 0) {
            (*com_func[i])(argumentcount,args);
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
            exit(EXIT_FAILURE);
        }
    }
}

void parse_command(char *command)
{
    if (command == NULL) {
        exit(EXIT_FAILURE); //empty line
    }
    char *split;
    split=strpbrk(command,"|<>");
    if (split)
    {
        if ((*split)=='|') {
            *split = 0;//set a split sign \0
            int file_dscp[2];
            pipe(file_dscp);
            int pid = fork();
            if (pid == 0) {
                //child proc, exe the command before |
                close(file_dscp[0]);//close child proc read
                close(STDOUT_FILENO);//close program standard output
                dup(file_dscp[1]);//stdout will be file_dscp[1]
                execute(command);//exe command before |
            }
            else if (pid>0)
            {
                //parent
                close(file_dscp[1]);//close parent write
                close(STDIN_FILENO);//close program standard in
                dup(file_dscp[0]);//stdin will be file_dscp[0]
                parse_command(split+1);//keep exe next part of command
            } else
            {
                perror("fork");
            }
        }
        else if ((*split)=='>')//">"
        {
            *split=0;//another cut
            char *fout_name=malloc(MAX_ADDR);
            sscanf(split+1,"%s",fout_name);
            if (fout_name==NULL)
            {
                perror("Please check output file name ");
                return;
            }
            int fout;
            fout = open(fout_name, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH| S_IWOTH);
            if (fout==-1)
            {
                perror("Cannot create or open target file ");
                return;
            } else
            {
                close(STDOUT_FILENO);
                dup(fout);
                parse_command(command);//after set output to file then continue
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
                close(STDIN_FILENO);
                dup(fin);
                parse_command(command);
                return;
            }
        }
    } else
    {
        int pid;
        pid=fork();
        if (pid==0) execute(command);//child one command
        else if (pid>0) //parent when one command wait and exit
        {
            int *status=NULL;
            //redirect the input and output
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
    getcwd(current_base_path,MAX_ADDR);
    do
    {
        printf("ve482sh $ ");
        fgets(command,MAX_LINE_CHAR,stdin);
        char *mightpath=malloc(MAX_ADDR);
        sscanf(command,"%s%s",ifexit,mightpath);
        int isCD=0;
        if (!strcmp(ifexit,"exit")) exit(EXIT_SUCCESS);//exit the shell
            else
                if (!strcmp(ifexit,"cd"))
                {
                    cd(mightpath);
                    isCD=1;
                }
        if (!isCD) parse_command(command);
    }while (1);
}

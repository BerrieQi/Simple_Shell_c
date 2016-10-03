#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


//List of commands, followed by their corresponding functions.
char *com_str[] = {
        "cd",
        "ls",
        "exit"
};

//Function Declarations for builtin shell commands:
int cd(char **args);
int ls(char **args);
int sh_exit(char **args);

int (*com_func[]) (char **) = {
        &cd,
        &ls,
        &sh_exit
};
int num_arg();
int launch_proc(char **args);
int execute(char **args);
#define TOK_DELIM " \t\r\n\a"
char **split_line(char *line);
char *read_line(void);
void loop(void);

int main(int argc, char **argv)
{
    //this is main for shell, it will stay in the loop until exit.
    loop();//seem to be useless
    return EXIT_SUCCESS;
}

void loop(void)
{
    //refer: Tutorial - Write a Shell in C
    //in loop the shell keeps running
    char *line;
    char **args;
    int status;

    do {
        printf("ve482sh $ ");
        line = read_line();
        args = split_line(line);
        status = execute(args);
        //free the memory for next loop
        free(line);
        free(args);
    } while (status);
}

char *read_line(void)
{
    //refer: Tutorial - Write a Shell in C / Writing Your Own Shell
    char *line = NULL;
    size_t linesize = 0; // have getline allocate a buffer for us, ssize or size?
    getline(&line, &linesize, stdin);
    return line;
}

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
    token = strtok(line, TOK_DELIM);
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

int execute(char **args)
{
    if (args[0] == NULL) {
        return 1; //empty line
    }
    else if (!strcmp(args[0],"exit"))
        return 0;
    else
        return launch_proc(args);
}

int launch_proc(char **args)
{
    pid_t pid,wpid;//pid the command process, wpid the parent process
    int status;

    pid=fork();//here pid is not the pid we see in ps
    if (pid==0)//swapper or sched has process ID 0 and is responsible for paging, child
    {
        int i;
        for (i = 0; i < num_arg(); i++) {
            if (strcmp(args[0], com_str[i]) == 0) {
                if ((*com_func[i])(args)==0)
                    exit(EXIT_SUCCESS);//return 0
                else
                    exit(EXIT_FAILURE);//return 1
            }
        }
        if (execvp(args[0],args)==-1)
        {
            perror("Error");
        }
        exit(EXIT_FAILURE);//to return 1 for status in main loop still be 1
    }
    else if (pid<0)
        {
            perror("Error: Error forking.\n");
        }
        else
        {
            do//parent process
            {
                wpid = waitpid(pid, &status, WUNTRACED);
            }while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
    return 1;
}

int num_arg()
{
    return sizeof(com_str) / sizeof(char *);//find how many pointers in pointer array for iteration
}

//Command implementations.
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

int sh_exit(char **args)
{
    return 0;
}


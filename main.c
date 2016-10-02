#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



#define LSH_TOK_DELIM " \t\r\n\a"
char **split_line(char *line)
{
    int bufsize = 1024, pos= 0;
    char **tokens = malloc(1024 * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }
    token = strtok(line, LSH_TOK_DELIM);
    while (token != NULL) {
        tokens[pos] = token;
        pos++;
        if (pos >= 1024) {
            fprintf(stderr, "ve482sh: Too long input\n");
            exit(EXIT_FAILURE);
        }
        token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[pos] = NULL;
    return tokens;
}

char *read_line(void)
{
    //refer: Tutorial - Write a Shell in C
    const DefaultSize=64;
    char *line = NULL;
    ssize_t linesize = 0; // have getline allocate a buffer for us
    getline(&line, &linesize, stdin);
    return line;
}

void loop(void)
{
    //refer: Tutorial - Write a Shell in C
    //in loop the shell keeps running
    char *line;
    char **args;
    int status;

    do {
        printf("$ ");
        line = read_line();
        args = split_line(line);
        status = execute(args);
        //free the memory for next loop
        free(line);
        free(args);
    } while (status);
}

int main(int argc, char **argv)
{
    //this is main for shell, it will stay in the loop until exit.
    loop();
    return EXIT_SUCCESS;
}
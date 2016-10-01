#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *read_line(void)
{
    //refer: Tutorial - Write a Shell in C
    const DefaultSize=64;
    int linesize = DefaultSize;
    int pos= 0;
    char *line = malloc(sizeof(char) * linesize);
    int c;

    if (!line) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    flag=true;

    while (flag) {
        // Read a character
        c = getchar();
        // end of line
        if (c == EOF || c == '\n') {
            line[pos] = '\0';
            return line;
        } else {
            line[pos] = c;
        }
        position++;

        // reallocate if too long
        if (pos >= linesize) {
            linesize =linesize+DefaultSize;
            //use new_line to prevent realloc failure
            void *new_line = realloc(line,sizeof(char) * linesize);
            if (!new_line) {
                fprintf(stderr, "lsh: allocation error\n");
                free(line);
                exit(EXIT_FAILURE);
            }
            line=new_line;
        }
    }
}

void loop(void)
{
    //refer: Tutorial - Write a Shell in C
    //in loop we
    char *line;
    char **args;
    int status;

    do {
        printf("$ ");
        line = read_line();
        args = split_line(line);
        status = execute(args);

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
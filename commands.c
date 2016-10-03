#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "shell.h"
#include "shell.y"
#include "shell.l"

int main(int argc, char **argv)
{
    //this is main for shell, it will stay in the loop until exit.
    loop();//seem to be useless
    return EXIT_SUCCESS;
}

A Simple Shell
----------------------------------

Gneral Information:
This VE482 shell implements basic functions like other shells.

Functions and User Guideline
1. The shell runs in a constant loop, it has cd ls cat pwd echo mkdir rm commands and other external commands like grep vi xargs;
2. Basic ls command, only support flag -a and -l;
3. It supports commands like apt-get;
4. It supports pipe and redirection
   (e.g. echo 123 | grep 1
         echo 456 > 1
         echo 678 >> 1
         cat < 1)
5. It supports more complex commands with pipes (e.g. ls | xargs grep .txt)
6. With Ctrl+d or type command exit, it will do clean exit without memory leak;
7. Pipe composed with file I/O redirection (e.g. cat < 1.txt — sort -R > 2.txt);
8. Wait for command to be completed when encountering > or |;
9. Handle Quotation mark as whole part;
10. Catch syntax error like cat <;
11. Change working directory;
12. Press Ctrl C to exit current process.

Bugs:
1. About cat
    Due to my design of implement, I found when I typed
        cat 1 | grep e
    the shell will require input from stdin.
    It is caused by pipe and redirecion shares the same way of input and output in cat function.
    To change this design to solve this problem is a lot of work, which asks me to rewrite a lot of codes.
    Due to the limit of time, I will leave this bug here and try to fix it later.
    Now to use pipe and cat at the same time, make sure use redirection for cat to get the input. Or there will be problems.

Testing:
Change the working directory to this folder in terminal and then type make to use Makefile to generate .o file shell. Then type ./shell to run the shell.

Reference:
In this project, I mainly referenced the following documents:
Implementing a Job Control Shell (https://www.gnu.org/software/libc/manual/html_node/Implementing-a-Shell.html#Implementing-a-Shell)
Tutorial - Write a Shell in C By Stephen Brennan (https://brennan.io/2015/01/16/write-a-shell-in-c/)
Writing Your Own Shell (https://www.cs.purdue.edu/homes/grr/SystemsProgrammingBook/Book/Chapter5-WritingYourOwnShell.pdf)
Signal Handling (https://www.gnu.org/software/libc/manual/html_node/Signal-Handling.html#Signal-Handling)
https://github.com/minority1728645/gsh

2016.10.05
Peiyuan Qi
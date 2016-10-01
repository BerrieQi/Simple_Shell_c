Unix Script for Ex 5
----------------------------------

Gneral Information:
This script implement question 1-7 in ex5.

Reference:
In this project, I mainly referenced the following document and tutorial:
Implementing a Job Control Shell (https://www.gnu.org/software/libc/manual/html_node/Implementing-a-Shell.html#Implementing-a-Shell)
Tutorial - Write a Shell in C By Stephen Brennan (https://brennan.io/2015/01/16/write-a-shell-in-c/)

user add -m -g operator peiyuan 									//this line adds a new user named peiyuan
ps																	//this line shows all process running now
cat /proc/cpuinfo													//this line shows the cpu info
cat /proc/meminfo													//this line shows the free memory
echo testouput | tee f1.txt f2.txt									//this line redirects some random outputs into two files (f1.txt and f2.txt)
cat f1.txt f2.txt > testfile.txt									//this line concatenate f1.txt and f2.txt into testfile.txt
hexdump testfile.txt												//this line outputs testfile.txt in the form of hexadecimal
find /usr/src -name "*semaphore*" | xargs grep 'ddekit_sem_down'	//this line finds the file have the spcified name and content, here I regard one line as a command.


Testing File:
Run it in terminal

2016.09.22
Peiyuan Qi
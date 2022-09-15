#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>


int main()
{
    int fd[2]; // pipe file descriptors
    int fd2[2]; // pipe file descriptors

    char args1[50];
    char args2[50];

    if (pipe(fd)==-1)
    {
        printf("error has occured with pipe \n");
        return 1;
    }
    if (pipe(fd2)==-1)
    {
        printf("error has occured with pipe \n");
        return 1;
    }

    sprintf(args1,"%d", fd[0]); // file descriptors to send over
    sprintf(args2,"%d", fd2[1]);

    int pid = fork();
    if(pid == 0) // in child process
    {
        printf("input command: \n");
        execl("./server", args1, args2, (char *)NULL);
    }
    else    // out of child process
    {

        int flag =1;
        while(flag == 1)
        {
            int stat;
            char server[300];
            char check[50] = "exit\n";
            char usrinput[50];
            fgets(usrinput,50,stdin);

            if(write(fd[1], usrinput, 50) == -1)
            {
                printf("error writing to pipe \n");
                return 2;
            }

            if(read(fd2[0], server, 300 ) == -1)
            {
               printf("error reading pipe \n");
                return 3;
            }
            if(strcmp(usrinput, check)==0){// checks if input matches exit string
                flag = 0;
                waitpid(pid, &stat, 0); // waits fo child to finish
                printf("response: server complete.\n");
                printf("Interface: child process (%s) completed.\n", server);
                printf("Interface: child process exit status = %d. \n", stat);
                printf("Interface: complete.");
            }
            else{
                printf("response: %s", server);
            }
        }
    }

    return 0;
}


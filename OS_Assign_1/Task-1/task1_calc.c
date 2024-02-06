#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <calc.h>

int main(int argc, char **argv){

  /*************************************************************
   write code for the main process that will establish
   the communication channel between cleint and server using
   pipes
  *************************************************************/

    pid_t pid;
    int fd1[2], fd2[2];

    if (pipe(fd1) < 0 || pipe(fd2) < 0) {
        perror("Pipe Creation failed");
        exit(-1);
    }

    pid = fork();
    if(pid < 0){
        perror("Fork failed\n");
        return 1;
    }

    if(pid == 0){ 
        //printf("Server started\n");
        server(fd1[0], fd2[1]);
        exit(0);
    }

    client(fd2[0], fd1[1]);

    //printf("Waiting for the server to stop\n");
    wait(NULL);
    close(fd1);
    close(fd2);

    return 0;
}

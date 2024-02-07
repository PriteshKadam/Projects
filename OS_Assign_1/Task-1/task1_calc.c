/*************************************************************
Author : Pritesh Kadam

OS Assignment 1 - Client-Server Calculator.

This file forks a child process which will act as a calculation server.
The parent process will act as a client. 
There will be two pipes for communication :
    Pipe1 : Client will write & Server will read from this.
    Pipe2 : Client will read and server will write on this.

Client will send space separated mathematical expression 
Server will calculate and send the result to client.

 *************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <calc.h>
#include <stdbool.h>

#define READ_END 0
#define WRITE_END 1

bool g_debug = false;

#define DEBUG_LOG(...) if(g_debug){printf(__VA_ARGS__);}

int main(int argc, char **argv){

  /*************************************************************
   write code for the main process that will establish
   the communication channel between cleint and server using
   pipes
  *************************************************************/

    if ((argc == 2) && (strncmp(argv[1], "debug", strlen("debug")) == 0)) {
        g_debug = true;
    }

    pid_t pid;
    int pipe1[2], pipe2[2];

    // Create two pipes .
    if (pipe(pipe1) < 0 || pipe(pipe2) < 0) {
        perror("Pipe Creation failed");
        exit(-1);
    }

    // Create child process.
    pid = fork();
    if(pid < 0){
        perror("Fork failed\n");
        return 1;
    }

    if(pid == 0){ 
        // Server/Child process will read from pipe1 and write to pipe2
        DEBUG_LOG("Server started\n");
        close(pipe1[WRITE_END]);
        close(pipe2[READ_END]);

        server(pipe1[READ_END], pipe2[WRITE_END]);
        exit(0);
    }

    // Client process will read from pipe2 and write to pipe1
    close(pipe1[READ_END]);
    close(pipe2[WRITE_END]);

    client(pipe2[READ_END], pipe1[WRITE_END]);

    DEBUG_LOG("Waiting for the server to stop\n");
    wait(NULL);

    return 0;
}

/*************************************************************
Author : Pritesh Kadam

OS Assignment 1 - Chat Between Two Users
Create two pipes for communication between two users.
Launches two processes and passes the rread/write pipe descriptors as arguments.
User 1 writes on pipe1 and reads from pipe2
User 2 writes on pipe2 and reads from pipe1
 *************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

#define MAXFD 20

#define READ_END 0
#define WRITE_END 1


bool g_debug = false;

#define DEBUG_LOG(...) if(g_debug){printf("[%s : ", argv[0]);printf(__VA_ARGS__);printf("]\n");}

int main(int argc, char **argv){
  /* we invoke driver program as follows:
     ./task2_driver <user1_content> <user1_store> <user2_content> <user2_store>

     sample content for user 1 and 2 are available as chat-1.txt and chat-2.txt file.

     use task2_user binary to exec user program as child process.
  */

    int pid_user1 = 0;
    int pid_user2 = 0;
    int pipe1[2], pipe2[2];
    char pipe2_readend[MAXFD], pipe1_writeend[MAXFD], pipe1_readend[MAXFD], pipe2_writeend[MAXFD];
    char* verbose = "nodebug";

    //printf("ARGS: 0[%s], 1[%s], 2[%s], 3[%s], 4[%s]\n", argv[0], argv[1], argv[2], argv[3], argv[4]);

    if (argc < 5)
    {
        perror("Required 5 arguments\n");
        exit(-1);
    }

    if (argc == 6 && strncmp(argv[5], "debug", strlen("debug")) == 0)
    {
        g_debug = true;
        verbose = "debug";
    }

    if (pipe(pipe1) < 0 || pipe(pipe2) < 0) {
        perror("Pipe Creation failed");
        exit(-1);
    }

    memset(pipe2_readend, 0, MAXFD);
    memset(pipe1_writeend, 0, MAXFD);
    memset(pipe1_readend, 0, MAXFD);
    memset(pipe2_writeend, 0, MAXFD);

    sprintf(pipe2_readend, "%d", pipe2[READ_END]);
    sprintf(pipe1_readend, "%d", pipe1[READ_END]);
    sprintf(pipe1_writeend, "%d", pipe1[WRITE_END]);
    sprintf(pipe2_writeend, "%d", pipe2[WRITE_END]);


    pid_user1 = fork();
    if (pid_user1 < 0) {
        perror("Fork failed\n");
        return 1;
    }

    if (pid_user1 == 0) {
        // Child. Run user1
        DEBUG_LOG("Launching User1\n");
        close(pipe1[READ_END]);
        close(pipe2[WRITE_END]);
        execl("./Task-2/task2_user", "task2_user_1", pipe2_readend, pipe1_writeend, argv[1], argv[2], verbose, NULL);
        perror("execl failed user1");
        exit(0);
    }

    pid_user2 = fork();
    if (pid_user2 < 0) {
        perror("Fork user 2 failed\n");
        return 1;
    }

    if (pid_user2 == 0) {
        // Child. Run user2
        DEBUG_LOG("Launching User2\n");
        close(pipe2[READ_END]);
        close(pipe1[WRITE_END]);
        execl("./Task-2/task2_user", "task2_user_2", pipe1_readend, pipe2_writeend, argv[3], argv[4], verbose, NULL);
        perror("execl failed user2");
        exit(0);
    }

    DEBUG_LOG("Waiting for child processes\n");
    wait(NULL);
    DEBUG_LOG("Driver Exiting\n");

  return 0;
}

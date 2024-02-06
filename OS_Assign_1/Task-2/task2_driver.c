#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXFD 20

int main(int argc, char **argv){
  /* we invoke driver program as follows:
     ./task2_driver <user1_content> <user1_store> <user2_content> <user2_store>

     sample content for user 1 and 2 are available as chat-1.txt and chat-2.txt file.

     use task2_user binary to exec user program as child process.
  */

    int pid_user1 = 0;
    int pid_user2 = 0;
    int fd1[2], fd2[2];
    char user1readfd[MAXFD], user1writefd[MAXFD], user2readfd[MAXFD], user2writefd[MAXFD];
    char* verbose = "nodebug";

    //printf("ARGS: 0[%s], 1[%s], 2[%s], 3[%s], 4[%s]\n", argv[0], argv[1], argv[2], argv[3], argv[4]);

    if (argc < 5)
    {
        perror("Required 5 arguments\n");
        exit(-1);
    }

    if (pipe(fd1) < 0 || pipe(fd2) < 0) {
        perror("Pipe Creation failed");
        exit(-1);
    }

    memset(user1readfd, 0, MAXFD);
    memset(user1writefd, 0, MAXFD);
    memset(user2readfd, 0, MAXFD);
    memset(user2writefd, 0, MAXFD);

    sprintf(user1readfd, "%d", fd2[0]);
    sprintf(user2readfd, "%d", fd1[0]);
    sprintf(user1writefd, "%d", fd1[1]);
    sprintf(user2writefd, "%d", fd2[1]);

    if (argc == 6 && strncmp(argv[5], "verbose", strlen("verbose")) == 0)
    {
        verbose = "debug";
    }

    pid_user1 = fork();
    if (pid_user1 < 0) {
        perror("Fork failed\n");
        return 1;
    }

    if (pid_user1 == 0) {
        // Child. Run user1
        printf("Launching User1\n");
        execl("./Task-2/task2_user", "task2_user_1", user1readfd, user1writefd, argv[1], argv[2], verbose, NULL);
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
        printf("Launching User2\n");
        execl("./Task-2/task2_user", "task2_user_2", user2readfd, user2writefd, argv[3], argv[4], verbose, NULL);
        perror("execl failed user2");
        exit(0);
    }

    //printf("Sleeping for 3 secs\n");
    //sleep(1);
    printf("Waiting for child processes\n");
    wait(NULL);
    printf("Driver Exiting\n");

  return 0;
}

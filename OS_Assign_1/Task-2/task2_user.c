#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdarg.h>

#define MAX_BUFF 500
#define STDIN 0
#define STDOUT 1
#define STDERR  2

bool debug = false;

#define DEBUG_LOG(...) if(debug){printf("[%s : ", argv[0]);printf(__VA_ARGS__);printf("]\n");}

int main(int argc, char **argv){

    /*************************************************************
        Implement the user program that can be invoked by the
        driver program to establish chat communication channel
        bwtween two user.

        please implement your program which take 4 arguments
        as follows

        argv[1]: read handle of channel
        argv[2]: write handle of channel
        argv[3]: chat content
        argv[4]: file to store other party message

    *************************************************************/

    if(argc < 5)
    {
        DEBUG_LOG("Received:[%d], Required[5] Args\n", argc);
        exit(-1);
    }

    //printf("ARGS: 0[%s], 1[%s], 2[%s], 3[%s], 4[%s]\n", argv[0], argv[1], argv[2], argv[3], argv[4]);

    int readfd = atoi(argv[1]);   // read handle of channel
    int writefd = atoi(argv[2]);  // write handle of channel
    char *chat_file = argv[3];    // chat content file
    char *store_file = argv[4];   // store file
    FILE *content, *store;        // file stream to read your chat content and store other user's message
    char buffer[MAX_BUFF];
    char chat[MAX_BUFF];
    size_t line_Size = MAX_BUFF;
    bool initiator = false;
    bool checkInit = true;
    char* line = buffer;

    if (strncmp(argv[5], "debug", strlen("debug")) == 0) {
        debug = true;
    }

    DEBUG_LOG("READFD:[%d], WRITEFD[%d]\n", readfd, writefd);

    // Start your implementation
    FILE* chatFile = fopen(chat_file, "r");
    if (chatFile == NULL) {
        DEBUG_LOG("Could not open chat file");
        return -1;
    }

    FILE* storeFile = fopen(store_file, "w");
    if (storeFile == NULL) {
        DEBUG_LOG("Could not open store file");
        fclose(chatFile);
        return -1;
    }
 
    memset(line, 0, MAX_BUFF);

    // Read the chat_file line by line
    while (getline(&line, &line_Size, chatFile) > 0) {

        //DEBUG_LOG(line);
        if (checkInit == true){
            if (strncmp(line, "initiate", strlen("initiate")) == 0) {
                initiator = true;
                checkInit = false;
                DEBUG_LOG("Initiator");
                continue;
            }else {
                initiator = false;
                checkInit = false;
                DEBUG_LOG("Non-Initiator");
            }
        }

        if (initiator)
        {
            // Write line
            DEBUG_LOG("Writing : %s ", line);
            if (write(writefd, line, MAX_BUFF) == 0){
                DEBUG_LOG("Failed to send");
                break;
            }

            if (strncmp(line, "bye", strlen("bye")) == 0) {
                DEBUG_LOG("Sent BYE!\n");
                break;
            }

            // Wait for chat
            DEBUG_LOG("Waiting for chat");
            memset(chat, 0, MAX_BUFF);
            if (read(readfd, chat, MAX_BUFF) == 0){
                DEBUG_LOG("Failed to read");
                break;
            }

            DEBUG_LOG("Received : %s", chat);

            if (strncmp(chat, "bye", strlen("bye")) == 0){
                DEBUG_LOG("Received BYE!\n");
                break;
            }

            // Write reply to store
            fprintf(storeFile, "%s", chat);
        }
        else
        {
            // Wait for chat
            DEBUG_LOG("Waiting for chat");
            memset(chat, 0, MAX_BUFF);
            if (read(readfd, chat, MAX_BUFF) == 0) {
                DEBUG_LOG("Failed to read");
                break;
            }

            DEBUG_LOG("Received : %s", chat);

            if (strncmp(chat, "bye", strlen("bye")) == 0){
                DEBUG_LOG("Received BYE!\n");
                break;
            }

            // Write to store
            fprintf(storeFile, "%s", chat);

            // Write chat
            DEBUG_LOG("Writing: %s", line);
            if (write(writefd, line, MAX_BUFF) == 0) {
                DEBUG_LOG("Failed to send\n");
                break;
            }

            if (strncmp(line, "bye", strlen("bye")) == 0) {
                DEBUG_LOG("Sent BYE!\n");
                break;
            }
        }
    }

    DEBUG_LOG("Exiting\n");

    fclose(chatFile);
    fclose(storeFile);

    return 0;
}

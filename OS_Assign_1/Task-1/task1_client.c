/*************************************************************
OS Assignment 1 - Client - Server Calculator.

This file contains the client code.

*************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <calc.h>
#include <stdbool.h>

extern bool g_debug ;
#define DEBUG_LOG(...) if(g_debug){printf("Client : ");printf(__VA_ARGS__);}

void client(int readfd, int writefd){
  /*************************************************************
   write client code to be used by the parent process
  *************************************************************/
  char buf[MAX_BUFF];
  size_t len;
  
  memset(buf, 0, MAX_BUFF);

  DEBUG_LOG("Enter Expressions.(END to exit)\n");
  
  while (1){

	// Read from STDIN
	fgets(buf, MAX_BUFF, stdin);
	DEBUG_LOG("EXP : %s\n", buf);
	if (strncmp(buf, "END", strlen("END")) == 0){
		break;
	}

	// Write to pipe
	if (write(writefd, buf, MAX_BUFF) == 0){
		perror("Failed to send to server\n");
		continue;
	}

	// Wait for result
	memset(buf, 0, MAX_BUFF);
	if (read(readfd, buf, MAX_BUFF) == 0){
		perror("Failed to read from server\n");
		continue;
	}

	// Write to STDOUT
	printf("RESULT: %s\n", buf);
  }

  DEBUG_LOG("Sending EXIT signal to server.\n");
  write(writefd, "END", 3);
  DEBUG_LOG("Exiting client");

  return;

}

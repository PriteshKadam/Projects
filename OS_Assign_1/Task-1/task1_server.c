/*************************************************************
Author : Pritesh Kadam

OS Assignment 1 - Client-Server Calculator.

This file contains the server code. It receives the expression from client
calculates the result and sends back to the client.

 *************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <calc.h>
#include <stdbool.h>

extern bool g_debug;
#define DEBUG_LOG(...) if(g_debug){printf("Server : ");printf(__VA_ARGS__);}

void server(int readfd, int writefd){
	/*************************************************************
	write server code to be used by the child process
	*************************************************************/
	char buf[MAX_BUFF];
	size_t len;

	memset(buf, 0, MAX_BUFF);

	while (1)
	{
		double result = 0;
		int len = 0;

		DEBUG_LOG("Reading from PIPE");
		memset(buf, 0, MAX_BUFF);
		if (read(readfd, buf, MAX_BUFF) == 0){
			perror("Failed to read from client\n");
			continue;
		}

		DEBUG_LOG("Received Expression: %s\n", buf);

		if (0 == strcmp(buf, "END")){
			break;
		}

		result = calculate(buf);
		memset(buf, 0, MAX_BUFF);
		len = sprintf(buf, "%.3f", result);

		DEBUG_LOG("RESULT: %s", buf);

		// Write to pipe
		DEBUG_LOG("Writing to pipe\n");
		if (write(writefd, buf, len) != len){
			perror("Failed to reply to client\n");
			continue;
		}	
	}
	DEBUG_LOG("Server Exiting\n");
}

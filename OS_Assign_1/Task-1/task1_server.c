#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <calc.h>

void server(int readfd, int writefd){
	/*************************************************************
	write server code to be used by the child process
	*************************************************************/
	char buf[MAX_BUFF];
	size_t len;

	FILE* fptr = fopen("serverlog.txt", "w");
	if (fptr == NULL){
		printf("Could not open file");
		return;
	}
	/* Implement server functionlity here
	use calculate function to evaluate expression
	signature: calculate(char *buf)
	*/

	memset(buf, 0, MAX_BUFF);

	while (1)
	{
		double result = 0;
		int len = 0;

		// Read from PIPE
		memset(buf, 0, MAX_BUFF);
		if (read(readfd, buf, MAX_BUFF) == 0){
			perror("Failed to read from client\n");
			continue;
		}

		fprintf(fptr, "Received Expression: %s\n", buf);

		if (0 == strcmp(buf, "END")){
			break;
		}

		result = calculate(buf);
		memset(buf, 0, MAX_BUFF);
		len = sprintf(buf, "%.3f", result);

		fprintf(fptr, "RESULT: %s", buf);

		// Write to pipe
		if (write(writefd, buf, len) != len){
			perror("Failed to reply to client\n");
			continue;
		}	
	}

	fclose(fptr);

	//printf("Server Exiting\n");
  
}

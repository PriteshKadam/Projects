#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <calc.h>


void client(int readfd, int writefd){
  /*************************************************************
   write client code to be used by the parent process
  *************************************************************/
  char buf[MAX_BUFF];
  size_t len;

  // implement client functionality
  
  memset(buf, 0, MAX_BUFF);

  //printf("Enter Expressions.(END to exit)\n");
  
  while (1){

	// Read from STDIN
	fgets(buf, MAX_BUFF, stdin);
	//printf("EXP : %s\n", buf);
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

  //printf("Sending EXIT signal to server.\n");
  write(writefd, "END", 3);
  //printf("Exiting client");

  return;

}

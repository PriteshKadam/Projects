/*************************************************************
Author : Pritesh Kadam

OS Assignment 1 - Directory Space Usage

This program calculates the size of directory passed as parameter.
GetSize function will for new process for subdirectories under root.

 *************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

#define DEBUG_LOG(...) if(g_debug){printf(__VA_ARGS__);}

// Refer : https://man7.org/linux/man-pages/man2/lstat.2.html

bool g_debug = false;

// Calculate the Size of all files and directories under it and the size of the directory itself
// forks a new process for root subdirectories
off_t GetSize(char dirPath[], bool isRoot)
{
	struct stat sb;
	DIR* pRootDir;
	struct dirent* dirEnt;
	char childPath[4096];
	off_t size = 0;
	int fd[2];

	pRootDir = opendir(dirPath);
	if (!pRootDir) {
		perror("Failed to open root directory");
		return -1;
	}

	if (dirPath[strlen(dirPath) - 1] != '/') {
		strcat(dirPath, "/");
	}

	strcpy(childPath, dirPath);

	if (isRoot == true)
	{
		DEBUG_LOG("Root directory. Creating pipes:\n");
		if (pipe(fd) < 0) {
			perror("Pipe Creation failed");
			exit(-1);
		}
	}

	while ((dirEnt = readdir(pRootDir)) != NULL) {

		childPath[strlen(dirPath)] = '\0';
		strcat(childPath, dirEnt->d_name);
		DEBUG_LOG("GetSize: DirEnt[%s] \n", dirEnt->d_name);

		if (strcmp(dirEnt->d_name, ".") == 0)
		{
			if (lstat(childPath, &sb) == -1) {
				perror("lstat self failed");
				exit(-1);
			}
			// directory entry size
			size = size + sb.st_size;
			DEBUG_LOG("Skipped \n");
			continue;
		}

		if (strcmp(dirEnt->d_name, "..") == 0)
		{
			continue;
		}

		if (lstat(childPath, &sb) == -1) {
			perror("lstat childPath failed");
			exit(-1);
		}

		switch (sb.st_mode & S_IFMT) {
		case S_IFDIR:
		{
			DEBUG_LOG("directory\n");
			if (isRoot == true)
			{
				DEBUG_LOG("Creating child\n");
				int pid = fork();
				if (pid < 0) {
					perror("Fork failed\n");
					exit(-1);
				}

				if (pid == 0) {
					close(fd[0]);
					char subdirPath[4096];
					strcpy(subdirPath, childPath);
					off_t childSize = GetSize(subdirPath, false);

					DEBUG_LOG("Child writing size[%ld]\n", childSize);
					if (write(fd[1], &childSize, sizeof(off_t)) != sizeof(off_t)) {
						perror("Failed to write size\n");
					}

					exit(0);
				}
			}
			else
			{
				size = size + GetSize(childPath, false);
			}
			break;
		}

		case S_IFLNK:
		{
			DEBUG_LOG("symlink\n");
			char symlinkpath[4096];
			strcpy(symlinkpath, childPath);

			struct stat symlinksb;
			if (stat(symlinkpath, &symlinksb) == -1) {
				perror("lstat");
				exit(-1);
			}

			if ((symlinksb.st_mode & S_IFMT) == S_IFREG)
			{
				DEBUG_LOG("symlink to file");
				size = size + symlinksb.st_size;
				break;

			}
			DEBUG_LOG("symlink to Directory");
			char actualpath[4096];
			realpath(symlinkpath, actualpath);
			DEBUG_LOG("Actual Path : %s", actualpath);
			size = size + GetSize(actualpath, false);
			break;
		}

		case S_IFREG: {
			DEBUG_LOG("file\n");
			size = size + sb.st_size;
			break;
		}
		default:
			DEBUG_LOG("unknown\n");
			break;
		}
	}

	if (isRoot == true)
	{
		DEBUG_LOG("Waiting for child \n");
		wait(NULL);
		DEBUG_LOG("Child process done\n");

		close(fd[1]);
		off_t readSize;
		DEBUG_LOG("Reading from pipe \n");
		ssize_t bytes_read;
		while ((bytes_read = read(fd[0], &readSize, sizeof(off_t)) > 0)) {
			size = size + readSize;
			DEBUG_LOG("Reading from child[%ld] \n", readSize);
		}
	}
	closedir(pRootDir);

	return size;
}

int main(int argc, char* argv[])
{
	struct stat sb;
	off_t size = 0;
	char rootDir[256];
	int fd[2];

	if (argc < 2) {
		DEBUG_LOG("Usage: ./%s <DirectoryPath>\n", argv[0]);
		exit(-1);
	}
	if (argc == 3 && strcmp(argv[2], "debug") == 0)
	{
		g_debug = true;
	}
	if (pipe(fd) < 0) {
		perror("Pipe Creation failed");
		exit(-1);
	}

	strcpy(rootDir, argv[1]);
	if (rootDir[strlen(rootDir) - 1] != '/') {
		strcat(rootDir, "/");
	}
	DEBUG_LOG("Path : %s\n", rootDir);

	if (lstat(rootDir, &sb) == -1) {
		perror("lstat");
		exit(-1);
	}

	if ((sb.st_mode & S_IFMT) != S_IFDIR)
	{
		perror("Not a directory");
		exit(-1);
	}
	
	size = GetSize(rootDir, fd[1]);
	printf("%ld\n", size);
	return 0;
}

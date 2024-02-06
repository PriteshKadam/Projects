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
off_t GetSize(char dirPath[])
{
	struct stat sb;
	DIR* pRootDir;
	struct dirent* dirEnt;
	char childPath[4096];
	off_t size = 0;

	pRootDir = opendir(dirPath);
	if (!pRootDir) {
		perror("Failed to open root directory");
		return -1;
	}

	if (dirPath[strlen(dirPath) - 1] != '/') {
		strcat(dirPath, "/");
	}

	strcpy(childPath, dirPath);

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
			size = size + GetSize(childPath);
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

			if ((symlinksb.st_mode & S_IFMT) == S_IFDIR)
			{
				// symlink to file. Directly add the size of file
				size = size + symlinksb.st_size;
				break;

			}
			// symlink to Directory
			char actualpath[4096];
			realpath(symlinkpath, actualpath);

			size = size + GetSize(actualpath);;
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

	return size;
}

int main(int argc, char* argv[])
{
	struct stat sb;
	off_t size = 0;
	DIR *pRootDir;
	struct dirent* dirEnt;
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

	// Parent process should find the size of the files
	// immediately under it along with the Root directory size
	// Exclude symlink size

	// Enumrate directory
	// if subdirectory : for new process
	// if file : add file size
	// if symlink : exlude link size

	//size = // directory size 

	pRootDir = opendir(rootDir);
	if (!pRootDir) {
		perror("Failed to open root directory");
	}

	char childPath[4096];
	strcpy(childPath, rootDir);
	while ((dirEnt = readdir(pRootDir)) != NULL) {
		
		childPath[strlen(rootDir)] = '\0';
		strcat(childPath, dirEnt->d_name);
		DEBUG_LOG("%s \n", childPath);

		if(strcmp(dirEnt->d_name, ".") == 0)
		{
			if (lstat(childPath, &sb) == -1) {
				perror("lstat failed");
				exit(-1);
			}
			// directory entry size
			size = size + sb.st_size;
			continue;
		}

		if(strcmp(dirEnt->d_name, "..") == 0)
		{
			continue;
		}

		if (lstat(childPath, &sb) == -1) {
			perror("lstat failed");
			exit(-1);
		}

		switch (sb.st_mode & S_IFMT) {
		case S_IFDIR:  
		{
			DEBUG_LOG("directory\n");
			//size = size + sb.st_size;
			int pid = fork();
			if (pid < 0) {
				perror("Fork failed\n");
				exit(-1);
			}

			if (pid == 0) {
				close(fd[0]);
				char subdirPath[4096]; 
				strcpy(subdirPath, childPath);
				off_t childSize =  GetSize(subdirPath);

				DEBUG_LOG("Child writing size[%ld]\n", childSize);
				if (write(fd[1], &childSize, sizeof(off_t)) != sizeof(off_t)) {
					perror("Failed to write size\n");
				}

				exit(0);
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
				// symlink to file. Directly add the size
				size = size + symlinksb.st_size;
				break;

			}
			// symlink to Directory
			char actualpath[4096];
			realpath(symlinkpath, actualpath);

			int childSize = GetSize(actualpath);
			size = size + childSize;
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

	printf("%ld\n", size);
	closedir(pRootDir);

	return 0;
}

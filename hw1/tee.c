#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>


pthread_attr_t attr;
int fwFile[2];
int fwStd[2];
FILE* fp;
char* file;

void* FilePrinter(void*);
void* StdPrinter(void*);

int main(int argc, char* argv[]) {
	if(argc < 2) {
		perror("Error no file\n");
		exit(1);
	}
	// look for additional option clean / -c
	for(int i = 2; i < argc; i++) {
		if(!strcmp(argv[i], "clean") || !strcmp(argv[i], "-c")) {
			truncate(argv[1], 0);
			break;
		}
	}
	file = argv[1];
  /* set global thread attributes */
	for(int i = 0; i < 3; i++) end[i] = 0;	// set all end to false (0)
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	char c;
	int result;
	result = pipe(fwFile);
	if (result < 0) {
		perror("Error file pipe\n");
		exit(1);
	}
	result = pipe(fwStd);
	if (result < 0) {
		perror("Error std pipe\n");
		exit(1);
	}
	pthread_t fileOut, stdOut;
	pthread_create(&fileOut, &attr, FilePrinter, NULL);
	pthread_create(&stdOut, &attr, StdPrinter, NULL);
	while (1) {
		if((c = getchar()) != EOF) {
			result = write(fwFile[1], &c, 1);
			if (result < 1) {
				perror("Error write file pipe\n");
				exit(1);
			}
			result = write(fwStd[1], &c, 1);
			if (result < 1) {
				perror("Error write std pipe\n");
				exit(1);
			}
		}
	}
}

void* trash;
void* FilePrinter(void* arg) {
	char c;
	int result;
	while (1) {
		fp = fopen(file, "a");
		result = read(fwFile[0], &c, 1);
		if (result != 1) {
			perror("Error read file pipe\n");
			exit(1);
		}
		fprintf(fp, "%s", &c);
		fclose(fp);
	}
	return trash;
}
void* StdPrinter(void* arg) {
	char c;
	int result;
	while (1) {
		result = read(fwStd[0], &c, 1);
		if (result != 1) {
			perror("Error read file pipe\n");
			exit(1);
		}
		printf("%s", &c);
	}
	return trash;
}

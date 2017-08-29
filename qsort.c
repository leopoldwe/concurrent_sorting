// -----------------------------------------------------------
// NAME : Leopold Eckert                     User ID: lweckert
// DUE DATE : 10/17/2014
// PROGRAM ASSIGNMENT # 2
// FILE NAME : qsort.c
// PROGRAM PURPOSE :
//    This program does a quicksort using several processes.
// -----------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

// ----------------------------------------------------------- 
// FUNCTION  main :                          
//     The function that is called when the program is run.
//     Does a quick sort.
// PARAMETER USAGE :
//    argc: number of arguments, should be four.
//    argv: the arguments, left side of a section of an array
//          to sort, followed by right side, followed by the 
//          size of the shared memory.
// FUNCTION CALLED :
//    atoi
//    getpid
//    ftok
//    printf
//    shmget
//    shmat
//    sprintf
//    fork
//    execvp
//    wait
//    exit
//    shmdt
// -----------------------------------------------------------

int main(int argc, char** argv) {
	int left;
	int right;
	int status;
	int currentLeft;
	int currentRight;
	int size;
	int pivot;
	int temp;
	pid_t processId;
	pid_t forkId;
	key_t key;
	int sharedId;
	int* data;
	char arg1[32];
	char arg2[32];
	char* newArgs[5];
	char buffer[512];
	
	if (argc != 4) {
		printf("There should be three arguments!");
		exit(1);
	}
	//Get arguments:
	left = atoi(argv[1]);
	right = atoi(argv[2]);
	size = right - left + 1;
	
	//Base case:
	if (size < 2) {
		return 0;
	}
	
	processId = getpid();
	
	//Get shared memory:
	key = atoi(argv[4]);
	sharedId = shmget(key, atoi(argv[3]), 0666);
	if (sharedId < 0) {
		printf("failed to get id.");
		exit(1);
	}
	data = (int *) shmat(sharedId, NULL, 0);
	if (data == 0) {
		printf("Could not get data.");
		exit(1);
	}
	
	//Print out area to sort:
	sprintf(buffer, "   ### Q-PROC(%d): entering with a[%d..%d]\n   ", (int)processId, left, right);
	for (int i = left; i < right; i++) {
		sprintf(buffer, "%s%4d", buffer, data[i]);
	}
	sprintf(buffer, "%s\n", buffer);
	printf("%s", buffer);
	
	//Do one iteration of quick sort:
	pivot = data[left + (size / 2) - 1];
	printf("   ### Q-PROC(%d): pivot element is a[%d] = %4d\n", (int)processId, (left + (size / 2) - 1), pivot);
	currentLeft = left;
	currentRight = right;
	while (currentLeft <= currentRight) {
		if (data[currentLeft] < pivot) {
			currentLeft++;
		}
		else if(data[currentRight] > pivot) {
			currentRight--;
		}
		else {
			temp = data[currentLeft];
			data[currentLeft] = data[currentRight];
			data[currentRight] = temp;
		}
	}
	
	//Create children to do other iterations:
	forkId = fork();
	if (forkId < 0) {
		printf("qsort failed to fork\n");
		exit(2);
	}
	else if (forkId == 0) {
		sprintf(arg1, "%d", left);
		sprintf(arg2, "%d", currentRight);
		newArgs[0] = argv[0];
		newArgs[1] = arg1;
		newArgs[2] = arg2;
		newArgs[3] = argv[3];
		newArgs[4] = argv[4];
		
		execvp("./qsort", newArgs);
	}
	forkId = fork();
	if (forkId < 0) {
		printf("qsort failed to fork\n");
		exit(2);
	}
	else if (forkId == 0) {
		sprintf(arg1, "%d", currentLeft);
		sprintf(arg2, "%d", right);
		newArgs[0] = argv[0];
		newArgs[1] = arg1;
		newArgs[2] = arg2;
		newArgs[3] = argv[3];
		execvp("./qsort", newArgs);
	}
	
	// Wait for them:
	wait(&status);
	wait(&status);
	//Now, the data is sorted:
	sprintf(buffer, "   ### Q-PROC(%d): section a[%d..%d] sorted\n   ", (int)processId, left, right);
	for (int i = left; i < right; i++) {
		sprintf(buffer, "%s%4d", buffer, data[i]);
	}
	sprintf(buffer, "%s\n", buffer);
	printf("%s", buffer);
	
	//Detach shared memory and exit:
	shmdt(data);
	printf("   ### Q-PROC(%d): exits", (int)processId);
	return 0;
	
}

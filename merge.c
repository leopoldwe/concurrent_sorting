// -----------------------------------------------------------
// NAME : Leopold Eckert                     User ID: lweckert
// DUE DATE : 10/17/2014
// PROGRAM ASSIGNMENT # 2
// FILE NAME : merge.c
// PROGRAM PURPOSE :
//    This program does a merge of two arrays using several
//    processes.
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
//     Decides what to do with the command line arguments.
// PARAMETER USAGE :
//    argc: number of arguments.
//    argv: the arguments, if first call, argv[1] should be 0, 
//          otherwise, should be argv[1] should be either 1 
//          or 2 for x or y, followed by its element, 
//          followed by the sizes of the shared arrays.
// FUNCTION CALLED :
//    
// -----------------------------------------------------------

int main(int argc, char** argv) {
	int assignedArray;
	int assignedNumber;
	int numberOfFirst;
	int numberOfSecond;
	int status;
	pid_t forkId;
	key_t mergeKey1;
	key_t mergeKey2;
	key_t finalKey;
	int mergeId1;
	int mergeId2;
	int finalId;
	int* mergeData1;
	int* mergeData2;
	int* finalData;
	char arg1[1];
	char arg2[16];
	char* newArgs[8];
	
	assignedArray = atoi(argv[1]);
	assignedNumber = atoi(argv[2]);
	numberOfFirst = atoi(argv[3]);
	numberOfSecond = atoi(argv[4]);
	
	newArgs[0] = argv[0];
	newArgs[1] = arg1;
	newArgs[2] = arg2;
	newArgs[3] = argv[3];
	newArgs[4] = argv[4];
	newArgs[5] = argv[5];
	newArgs[6] = argv[6];
	newArgs[7] = argv[7];
	
	//If this process needs to create all other processes:
	if (assignedArray == 0) {
		//Create processes for each member of x:
		sprintf(arg1, "1");
		for (int i = 0; i < numberOfFirst; i++) {
			forkId = fork();
			if (forkId < 0) {
				printf("merge failed to fork\n");
				exit(2);
			}
			else if (forkId == 0) {
				sprintf(arg2, "%d", i);
				execvp("./merge", newArgs);
			}
			
		}
		//Create processes for each member of y:
		sprintf(arg1, "2");
		for (int i = 0; i < numberOfSecond; i++) {
			forkId = fork();
			if (forkId < 0) {
				printf("merge failed to fork\n");
				exit(2);
			}
			else if (forkId == 0) {
				sprintf(arg2, "%d", i);
				execvp("./merge", newArgs);
			}
			
		}
		
		//Wait for all child processes
		for (int i = 0; i < (numberOfFirst + numberOfSecond); i++) {
			wait(&status);
		}
		return 0;
	}
	
	
	//If process is actually involved in merging:
	//Get keys and attach memory:
	mergeKey1 = ftok("./", 'm');
	mergeKey2 = ftok("./", 'n');
	finalKey = ftok("./", 'o');
	
}
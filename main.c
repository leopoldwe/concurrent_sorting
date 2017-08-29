// -----------------------------------------------------------
// NAME : Leopold Eckert                     User ID: lweckert
// DUE DATE : 10/17/2014
// PROGRAM ASSIGNMENT # 2
// FILE NAME : main.c
// PROGRAM PURPOSE :
//    This program handles creation and deletion of shared
//    memory, and starting the quick sort and merge sort.
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
//     Creates shared memory and starts the quick sort and
//     merge sort.
// PARAMETER USAGE :
//    argc: number of arguments
//    argv: the arguments
// FUNCTION CALLED :
//    scanf
//    ftok
//    shmget
//    shmat
//    printf
//    sprintf
//    shmdt
//    shmctl
//    fork
//    execvp
// -----------------------------------------------------------

int main(int argc, char** argv) {
	key_t quickKey;
	key_t mergeKey1;
	key_t mergeKey2;
	key_t mergeKeyFinal;
	pid_t forkId;
	char buffer [512];
	char *quickArgs[5];
	char *mergeArgs[9];
	char arg0[16];
	char arg1[32];
	char arg2[32];
	char arg3[32];
	char arg4[32];
	char arg5[32];
	char arg6[32];
	char arg7[32];
	char arg8[32];
	int status;
	int quickId;
	int mergeId1;
	int mergeId2;
	int mergeIdFinal;
	int quickArraySize;
	int mergeArraySize1;
	int mergeArraySize2;
	int* sharedQuickArray;
	int* sharedMergeArray1;
	int* sharedMergeArray2;
	int* sharedMergeArrayFinal;
	
	printf("*** MAIN: Quicksort and Binary Merge with Multiple Processes:\n\n");
	
	mergeArgs[0] = arg0;
	mergeArgs[1] = arg1;
	mergeArgs[2] = arg2;
	mergeArgs[3] = arg3;
	mergeArgs[4] = arg4;
	mergeArgs[5] = arg5;
	mergeArgs[6] = arg6;
	mergeArgs[7] = arg7;
	quickArgs[0] = arg0;
	quickArgs[1] = arg1;
	quickArgs[2] = arg2;
	quickArgs[3] = arg3;
	
	//Load in arrays from stdin.
	scanf("%d", &quickArraySize);
	int quickSortArray[quickArraySize];
	for (int i = 0; i < quickArraySize; i++) {
		scanf("%d", &quickSortArray[i]);
	}
	scanf("%d", &mergeArraySize1);
	int mergeSortArray1[mergeArraySize1];
	for (int i = 0; i < mergeArraySize1; i++) {
		scanf("%d", &mergeSortArray1[i]);
	}
	scanf("%d", &mergeArraySize2);
	int mergeSortArray2[mergeArraySize2];
	for (int i = 0; i < mergeArraySize2; i++) {
		scanf("%d", &mergeSortArray2[i]);
	}
	
	//Create Keys.
	quickKey = ftok("./", 'q');
	printf("*** MAIN: qsort shared memory key = %d\n", (int)quickKey);
	mergeKey1 = ftok("./", 'm');
	printf("*** MAIN: first merge data shared memory key = %d\n", (int)mergeKey1);
	mergeKey2 = ftok("./", 'n');
	printf("*** MAIN: second merge data shared memory key = %d\n", (int)mergeKey2);
	mergeKeyFinal = ftok("./", 'o');
	printf("*** MAIN: merged shared memory key = %d\n", (int)mergeKeyFinal);
	
	//Create Shared Memory.
	quickId = shmget(quickKey, sizeof(quickSortArray), IPC_CREAT | 0666);
	printf("*** MAIN: qsort shared memory created\n");
	mergeId1 = shmget(mergeKey1, sizeof(mergeSortArray1), IPC_CREAT | 0666);
	printf("*** MAIN: first merge data shared memory created\n");
	mergeId2 = shmget(mergeKey2, sizeof(mergeSortArray2), IPC_CREAT | 0666);
	printf("*** MAIN: second merge data shared memory created\n");
	mergeIdFinal = shmget(
						mergeKeyFinal,
						(sizeof(mergeSortArray1) + sizeof(mergeSortArray2)),
						IPC_CREAT | 0666);
	printf("*** MAIN: merged shared memory created\n");
	
	//Attach shared memory.
	sharedQuickArray = (int *) shmat(quickId, NULL, 0);
	printf("*** MAIN: qsort shared memory attached and is ready to use\n");
	sharedMergeArray1 = (int *) shmat(mergeId1, NULL, 0);
	printf("*** MAIN: first merge data shared memory attached and is ready to use\n");
	sharedMergeArray2 = (int *) shmat(mergeId2, NULL, 0);
	printf("*** MAIN: second merge data shared memory attached and is ready to use\n");
	sharedMergeArrayFinal = (int *) shmat(mergeIdFinal, NULL, 0);
	printf("*** MAIN: merged shared memory attached and is ready to use\n");
	
	//Place data into shared memory.
	sprintf(buffer, "*** MAIN: Input array for qsort has %d elements:\n", quickArraySize);
	for (int i = 0; i < quickArraySize; i++) {
		sprintf(buffer, "%s%4d", buffer, quickSortArray[i]);
		sharedQuickArray[i] = quickSortArray[i];
	}
	sprintf(buffer, "%s\n", buffer);
	printf("%s", buffer);
	
	sprintf(buffer, "*** MAIN: Input array x[] for merge has %d elements:\n", mergeArraySize1);
	for (int i = 0; i < mergeArraySize1; i++) {
		sprintf(buffer, "%s%4d", buffer, mergeSortArray1[i]);
		sharedMergeArray1[i] = mergeSortArray1[i];
	}
	sprintf(buffer, "%s\n", buffer);
	printf("%s", buffer);
	
	sprintf(buffer, "*** MAIN: Input array y[] for merge has %d elements:\n", mergeArraySize2);
	for (int i = 0; i < mergeArraySize2; i++) {
		sprintf(buffer, "%s%4d", buffer, mergeSortArray2[i]);
		sharedMergeArray2[i] = mergeSortArray2[i];
	}
	sprintf(buffer, "%s\n", buffer);
	printf("%s", buffer);
	
	//TODO: Call Sorts
	sprintf(arg0, "./qsort");
	sprintf(arg1, "0");
	sprintf(arg2, "%d", quickArraySize - 1);
	sprintf(arg3, "%d", (int)sizeof(quickSortArray));
	sprintf(arg4, "%d", (int)quickKey);
	printf("*** MAIN: about to spawn the process for qsort\n");
	forkId = fork();
	if (forkId < 0) {
		printf("main failed to fork\n");
		exit(2);
	}
	else if (forkId == 0) {
		execvp("./qsort", quickArgs);
	}
	
	//TODO: Wait
	while (wait(&forkId) < 0);
	//TODO: Print Results
	sprintf(buffer, "*** MAIN: sorted array by qsort:\n");
	for (int i = 0; i < quickArraySize; i++) {
		sprintf(buffer, "%s%4d", buffer, quickSortArray[i]);
		sharedQuickArray[i] = quickSortArray[i];
	}
	sprintf(buffer, "%s\n", buffer);
	printf("%s", buffer);
	
	//Detach shared memory
	shmdt(sharedQuickArray);
	printf("*** MAIN: qsort shared memory successfully detached\n");
	shmdt(sharedMergeArray1);
	printf("*** MAIN: first merge data shared memory successfully detached\n");
	shmdt(sharedMergeArray2);
	printf("*** MAIN: second merge data shared memory successfully detached\n");
	shmdt(sharedMergeArrayFinal);
	printf("*** MAIN: merged shared memory successfully detached\n");
	
	//Remove shared memory
	shmctl(quickId, IPC_RMID, NULL);
	printf("*** MAIN: qsort shared memory successfully removed\n");
	shmctl(mergeId1, IPC_RMID, NULL);
	printf("*** MAIN: first merge data shared memory successfully removed\n");
	shmctl(mergeId2, IPC_RMID, NULL);
	printf("*** MAIN: second merge data shared memory successfully removed\n");
	shmctl(mergeIdFinal, IPC_RMID, NULL);
	printf("*** MAIN: merged shared memory successfully removed\n");
	
	return 0;
}
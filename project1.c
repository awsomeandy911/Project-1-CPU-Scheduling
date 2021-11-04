#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//struct that stores Process objects in a Linked List
struct LinkedList 
{
	struct pidUnique 
	{
		int currentRunTime;
		int totalRunTime;
		int startTime;
		int value;
	} pidUnique;

	int unique;
	int pid;
	int burst;
	int priority;
	int nonVoluntary;
	struct LinkedList *next;
}; typedef struct LinkedList LinkedList;


//checks if pid exists in the linked list
int exists(LinkedList *list, int num) 
{
	while (list != NULL) 
	{
		if(list->pid == num) 
		{
			return 1;
			break;
		}
		list = list->next;
	}
	return 0;
}

//checks if pid is unique in the linked list
LinkedList * unique(LinkedList *node, int p) 
{
	if(node == NULL)
	{
		return NULL;
	}
	else if(node->pid == p)
	{
		return node;
	}
	//returns node if pid is unique
	else 
		return unique(node->next, p);
}

//searchs for non voulntary context switches
void searchNonVoluntary(LinkedList *head)
 {
	LinkedList * current = head;
	while (head != NULL && current != NULL) 
	{
		//set temp to hold previous pid
		int prevPID = current->pidUnique.value;

		//point head to next
		if (current->next != NULL) 
			current = current->next;
		
		//if head's next node exists then go through the condition
		if(head->next != NULL) 
		{
			//switch if previous isn't the same as current and if it has been encountered before
			//if the current pid exists within head as of now
			if ((prevPID != current->pidUnique.value) && (exists(head->next, prevPID) == 1) && (current->next != NULL)) 
			{
				head->nonVoluntary = 1;
			}
			else if (prevPID == current->pidUnique.value) 
			{
				head->nonVoluntary = 0;
			}
			else if (exists(head->next, prevPID) == 0) 
			{ 
				head->nonVoluntary = 0;
			}
		}
		head = head->next;
	}
}

//calculates the non voluntary context switch
int getNonVoluntary(LinkedList *head)
 {
	int sum = 0;
	while(head != NULL) 
	{
		sum += head->nonVoluntary;
		head = head->next;
	}
	return sum;
}

//insert into linked list
void insert(LinkedList **head, int pid_t, int burst_t, int priority_t) 
{
	if ((*head) == NULL) 
	{
		(*head) = (LinkedList *) malloc(sizeof(LinkedList));
		(*head)->pid = pid_t;
		(*head)->burst = burst_t;
		(*head)->priority = priority_t;
		(*head)->pidUnique.totalRunTime = 0;
		(*head)->nonVoluntary = 0;
		(*head)->next = NULL;
	}
	else 
	{
		insert(&(*head)->next, pid_t, burst_t, priority_t);

	
		//if it's a unique pid, then store it separately into the unique struct
		if(unique((*head), (*head)->pid) == (*head)) 
		{
			(*head)->pidUnique.value = (*head)->pid;
			(*head)->pidUnique.totalRunTime = (*head)->burst;
        	(*head)->pidUnique.startTime = 0;
		}
		//if it's not unique add to the unique PID's total runtime
		else 
		{
			(*head)->pidUnique.totalRunTime += (*head)->pidUnique.totalRunTime;
		}
	}

}

int main(int argc, char *argv[]) 
{

	FILE *fp;
	char process[1024];
	char instructionList[1024];
	char secondline[1024];
	const char whitespace[2] = " ";
	struct LinkedList *LinkedList = NULL;
	struct LinkedList *root = NULL;

	if (argc != 2) 
	{
		printf("Error, must specify a file name\n");
		return EXIT_FAILURE;
	}
	fp = fopen(argv[1], "r");

	if(!fp) 
	{
		perror("Error opening input file\n");
		return EXIT_FAILURE;
	}

	//read first line then convert to int
	fgets(process, sizeof(process), fp);	
	int numProcess = atoi(process);

	//read second line then store values accordingly
	fgets(secondline, sizeof(secondline), fp);

	//set the parsed first value as pid then convert to int;
	char* total_pid_c = strtok(secondline, whitespace);
	int pidUnique = atoi(total_pid_c);

	
	//holds the stats for each pidUnique in multiple arrays
    int burstTime[pidUnique];
    int completionTime[pidUnique];
	int waitTime[pidUnique];
	int arrivalTime[pidUnique];
	int firstTime[pidUnique];
	int responseTime[pidUnique];
    memset(&burstTime, 0, pidUnique*sizeof(int));
    memset(&completionTime, 0, pidUnique*sizeof(int));
	memset(&waitTime, 0, pidUnique*sizeof(int));
	memset(&arrivalTime, 0, pidUnique*sizeof(int));
	memset(&firstTime, 0, pidUnique*sizeof(int));
	memset(&responseTime, 0, pidUnique*sizeof(int));

	int pid;
	int burst;
	int priority;
	int nonVoluntary = 0;
    int totalBurst = 0;
    int totalTurnAround = 0;
	int totalWaitTime = 0;
	int totalResponseTime = 0;

	//reads in the instructions
	while(fgets(instructionList, sizeof(instructionList), fp) != NULL) 
	{
		//sets the variables to read in
		sscanf(instructionList, "%d %d %d", &pid, &burst, &priority);

		//calculates total burst with each instruction
        totalBurst += burst;

		//variables used for turnaround time
        burstTime[pid - 1] += burst;
       	completionTime[pid - 1] = totalBurst;

		//check to see if this is the first time pid has shown up
		if(arrivalTime[pid - 1] == 0) 
		{
			//set response time to burst recieved and start time to total burst
			arrivalTime[pid - 1] = burst;
			firstTime[pid - 1] = totalBurst;
		}

		//insert data into linked list
		insert(&LinkedList, pid, burst, priority);
	}

	//goes through the list to find Nonvoluntary
	searchNonVoluntary(LinkedList);	

	//loop to add all stats for the indices that pertain to unique pid
	for(int i = 0; i < pidUnique; i++) 
	{
        //total turnaround time = each of unique PID's completion time
        totalTurnAround += completionTime[i];

        //waiting time = turnaround time - its burst time
        waitTime[i] = completionTime[i] - burstTime[i];

		//sum of each pid's wait time
        totalWaitTime += waitTime[i];

		//response time = total burst time - current burst it took
		responseTime[i] = firstTime[i] - arrivalTime[i];

		//sum of each pid's response time
		totalResponseTime += responseTime[i];
	}

	// calculate non-voluntary switch
	nonVoluntary = getNonVoluntary(LinkedList);	

    //calculate CPU utilization
    float CPU = 100 / numProcess;

    //calculate throughput
    float throughput = (float)pidUnique / totalBurst;

	//calculate turnaround time
	double averageTurnAround = (double)totalTurnAround / pidUnique;

	//calculate waiting time
	double averageWaiting = (double)totalWaitTime / pidUnique;	

	//calculate response time
	float averageResponse = (float)totalResponseTime / pidUnique;

	//prints out pidUnique, nonvoluntary, CPU, throughput, turnaround, waiting, and response time
	fprintf(stdout, "%d\n%d\n%.2f\n%.2f\n%.2lf\n%.2lf\n%.2f\n", pidUnique, nonVoluntary, CPU, throughput, averageTurnAround, averageWaiting, averageResponse);

	free(LinkedList);
	return EXIT_SUCCESS;
}

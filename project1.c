#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//struct that stores Process objects in a Linked List
struct LinkedList 
{
	struct unique_pid 
	{
		int current_runtime;
		int total_runtime;
		int start_time;
		int value;
	} unique_pid;

	int unique;
	int pid;
	int burst;
	int priority;
	int nonvol;
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

void search_for_non_vol(LinkedList *head)
 {
	LinkedList * current = head;
	while (head != NULL && current != NULL) 
	{
		//set temp to hold previous pid
		int prevPID = current->unique_pid.value;

		//point head to next
		if (current->next != NULL) 
			current = current->next;
		
		//if head's next node exists then go through the condition
		if(head->next != NULL) 
		{
			//switch if previous isn't the same as current and if it has been encountered before
			//if the current pid exists within head as of now
			if ((prevPID != current->unique_pid.value) && (exists(head->next, prevPID) == 1) && (current->next != NULL)) 
			{
				head->nonvol = 1;
			}
			else if (prevPID == current->unique_pid.value) 
			{
				head->nonvol = 0;
			}
			else if (exists(head->next, prevPID) == 0) 
			{ 
				head->nonvol = 0;
			}
		}
		head = head->next;
	}
}

int calculate_nonvol(LinkedList *head)
 {
	int sum = 0;
	while(head != NULL) 
	{
		sum += head->nonvol;
		head = head->next;
	}
	return sum;
}

void insert(LinkedList **head, int pid_t, int burst_t, int priority_t) 
{
	if ((*head) == NULL) 
	{
		(*head) = (LinkedList *) malloc(sizeof(LinkedList));
		(*head)->pid = pid_t;
		(*head)->burst = burst_t;
		(*head)->priority = priority_t;
		(*head)->unique_pid.total_runtime = 0;
		(*head)->nonvol = 0;
		(*head)->next = NULL;
	}
	else 
	{
		insert(&(*head)->next, pid_t, burst_t, priority_t);

	
		//if it's a unique pid, then store it separately into the unique struct
		if(unique((*head), (*head)->pid) == (*head)) 
		{
			(*head)->unique_pid.value = (*head)->pid;
			(*head)->unique_pid.total_runtime = (*head)->burst;
        	(*head)->unique_pid.start_time = 0;
		}
		//if it's not unique add to the unique PID's total runtime
		else 
		{
			(*head)->unique_pid.total_runtime += (*head)->unique_pid.total_runtime;
		}
	}

}
int main(int argc, char *argv[]) 
{

	FILE *fp;
	char process[1024];
	char list_instructions[1024];
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
	int num_process = atoi(process);

	//read second line then store values accordingly
	fgets(secondline, sizeof(secondline), fp);

	//set the parsed first value as pid then convert to int;
	char* total_pid_c = strtok(secondline, whitespace);
	int unique_pid = atoi(total_pid_c);

	
	//holds the stats for each unique_pid in multiple arrays
    int burst_time[unique_pid];
    int completion_time[unique_pid];
	int wait_time[unique_pid];
	int arrival_time[unique_pid];
	int first_time[unique_pid];
	int response_time[unique_pid];
    memset(&burst_time, 0, unique_pid*sizeof(int));
    memset(&completion_time, 0, unique_pid*sizeof(int));
	memset(&wait_time, 0, unique_pid*sizeof(int));
	memset(&arrival_time, 0, unique_pid*sizeof(int));
	memset(&first_time, 0, unique_pid*sizeof(int));
	memset(&response_time, 0, unique_pid*sizeof(int));

	int pid_c;
	int burst_c;
	int priority_c;
	int nonvol = 0;
    int total_burst = 0;
    int total_turnaround_time = 0;
	int total_wait_time = 0;
	int total_response_time = 0;

	//reads in the instructions
	while(fgets(list_instructions, sizeof(list_instructions), fp) != NULL) 
	{
		//sets the variables to read in
		sscanf(list_instructions, "%d %d %d", &pid_c, &burst_c, &priority_c);

		//calculates total burst with each instruction
        total_burst += burst_c;

		//variables used for turnaround time
        burst_time[pid_c - 1] += burst_c;
       	completion_time[pid_c - 1] = total_burst;

		//check to see if this is the first time pid_c has shown up
		if(arrival_time[pid_c - 1] == 0) 
		{
			//set response time to burst recieved and start time to total burst
			arrival_time[pid_c - 1] = burst_c;
			first_time[pid_c - 1] = total_burst;
		}

		//insert data into linked list
		insert(&LinkedList, pid_c, burst_c, priority_c);
	}

	//goes through the list to find Nonvoluntary
	search_for_non_vol(LinkedList);	

	//loop to add all stats for the indices that pertain to unique pid
	for(int i = 0; i < unique_pid; i++) 
	{
        //total turnaround time = each of unique PID's completion time
        total_turnaround_time += completion_time[i];

        //waiting time = turnaround time - its burst time
        wait_time[i] = completion_time[i] - burst_time[i];

		//sum of each pid's wait time
        total_wait_time += wait_time[i];

		//response time = total burst time - current burst it took
		response_time[i] = first_time[i] - arrival_time[i];

		//sum of each pid's response time
		total_response_time += response_time[i];
	}

	// calculate non-voluntary switch
	nonvol = calculate_nonvol(LinkedList);	

    //calculate CPU utilization
    float CPU = 100 / num_process;

    //calculate throughput
    float throughput = (float)unique_pid / total_burst;

	//calculate turnaround time
	double avg_turnaround_time = (double)total_turnaround_time / unique_pid;

	//calculate waiting time
	double avg_waiting_time = (double)total_wait_time / unique_pid;	

	//calculate response time
	float avg_response_time = (float)total_response_time / unique_pid;

	//prints out unique_pid, nonvoluntary, CPU, throughput, turnaround, waiting, and response time
	fprintf(stdout, "%d\n%d\n%.2f\n%.2f\n%.2lf\n%.2lf\n%.2f\n", unique_pid, nonvol, CPU, throughput, avg_turnaround_time, avg_waiting_time, avg_response_time);

	free(LinkedList);
	return EXIT_SUCCESS;
}

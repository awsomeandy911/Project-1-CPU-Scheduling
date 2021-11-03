#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//struct that stores Process objects in a Linked List
struct LinkedList 
{
	struct uniquePid 
	{
		int currentRunTime;
		int totalRunTime;
		int startTime;
		int value;
	} uniquePid;

	int unique;
	int pid;
	int burst;
	int priority;
	int nonVoluntary;
	struct LinkedList *next;

};typedef struct LinkedList LinkedList;

//method to check if pid exists in the linked list
int pidExists(LinkedList *list, int num) 
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

//method to check if pid is unique in the linked list
LinkedList * pidUnique(LinkedList *node, int process)
{
	if(node == NULL)
	{
		return NULL;
	}

	//returns node if pid is unique
	else if(node->pid == process)
	{
		return node;
	}

	else 
		return pidUnique(node->next, process);
}

void searchNonVoluntary(LinkedList *head) 
{
	LinkedList * current = head;
	
	while (head != NULL && current != NULL) 
	{
		
		int previousPid = current->uniquePid.value;
		
		if (current->next != NULL) 
		{
			current = current->next;
		}
		
		if(head->next != NULL)
		{
			
			if ((previousPid != current->uniquePid.value) && (pidExists(head->next, previousPid) == 1) && (current->next != NULL)) 
			{
				head->nonVoluntary = 1;
			}
			else if (previousPid == current->uniquePid.value)
			{
				head->nonVoluntary = 0;
			}

			else if (pidExists(head->next, previousPid) == 0)
			{ 
				head->nonVoluntary = 0;
			}
		}
		head = head->next;
	}
}

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

void insert(LinkedList **head, int pid_t, int burst_t, int priority_t)
{
	if ((*head) == NULL)
	{
		(*head) = (LinkedList *) malloc(sizeof(LinkedList));
		(*head)->pid = pid_t;
		(*head)->burst = burst_t;
		(*head)->priority = priority_t;
		(*head)->uniquePid.totalRunTime = 0;
		(*head)->nonVoluntary = 0;
		(*head)->next = NULL;
	}

	else 
	{
		insert(&(*head)->next, pid_t, burst_t, priority_t);

		if(unique((*head), (*head)->pid) == (*head)) 
		{
			(*head)->uniquePid.value = (*head)->pid;
			(*head)->uniquePid.totalRunTime = (*head)->burst;
        	(*head)->uniquePid.startTime = 0;
		}

		else 
		{
			(*head)->uniquePid.totalRunTime += (*head)->uniquePid.totalRunTime;
		}
	}

}

int main(int argc, char *argv[]) 
{

	FILE *fp;
	char process[1024];
	char N[1024];
	char secondLine[1024];
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

	fgets(process, sizeof(process), fp);	
	int num_process = atoi(process);

	
	fgets(secondLine, sizeof(secondLine), fp);


	char* total_pid_c = strtok(secondLine, whitespace);
	int uniquePid = atoi(total_pid_c);

	
    int burst_time[uniquePid];
    int completion_time[uniquePid];
	int wait_time[uniquePid];
	int arrival_time[uniquePid];
	int first_time[uniquePid];
	int response_time[uniquePid];

    memset(&burst_time, 0, uniquePid*sizeof(int));
    memset(&completion_time, 0, uniquePid*sizeof(int));
	memset(&wait_time, 0, uniquePid*sizeof(int));
	memset(&arrival_time, 0, uniquePid*sizeof(int));
	memset(&first_time, 0, uniquePid*sizeof(int));
	memset(&response_time, 0, uniquePid*sizeof(int));

	int pid_c;
	int burst_c;
	int priority_c;
	int nonVoluntary = 0;
    int total_burst = 0;
    int total_turnaround_time = 0;
	int total_wait_time = 0;
	int total_response_time = 0;

	
	while(fgets(N, sizeof(N), fp) != NULL)
	{
		
		sscanf(N, "%d %d %d", &pid_c, &burst_c, &priority_c);

		
        total_burst += burst_c;

	
        burst_time[pid_c - 1] += burst_c;
       	completion_time[pid_c - 1] = total_burst;

		if(arrival_time[pid_c - 1] == 0)
		{
			arrival_time[pid_c - 1] = burst_c;
			first_time[pid_c - 1] = total_burst;
		}

		
		insert(&LinkedList, pid_c, burst_c, priority_c);
	}


	searchNonVoluntary(LinkedList);	

	for(int i = 0; i < uniquePid; i++)
	{
      
        total_turnaround_time += completion_time[i];

    
        wait_time[i] = completion_time[i] - burst_time[i];
		
        total_wait_time += wait_time[i];

		
		response_time[i] = first_time[i] - arrival_time[i];
		
		total_response_time += response_time[i];
	}

	
	nonVoluntary = getNonVoluntary(LinkedList);	

   
    float cpu = 100 / num_process;

    
    float throughput = (float)uniquePid / total_burst;


	double avg_turnaround_time = (double)total_turnaround_time / uniquePid;

	
	double avg_waiting_time = (double)total_wait_time / uniquePid;	

	
	float avg_response_time = (float)total_response_time / uniquePid;

	fprintf(stdout, "%d\n%d\n%.2f\n%.2f\n%.2lf\n%.2lf\n%.2f\n", uniquePid, nonVoluntary, cpu, throughput, avg_turnaround_time, avg_waiting_time, avg_response_time);

	free(LinkedList);
	return EXIT_SUCCESS;
}

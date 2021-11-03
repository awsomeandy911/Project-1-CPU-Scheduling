#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct LinkedList {
	struct unique_pid {
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
};
typedef struct LinkedList LinkedList;

int exists(LinkedList *list, int num) {
	while (list != NULL) {
		if(list->pid == num) {
			return 1;
			break;
		}
		list = list->next;
	}
	return 0;
}

LinkedList * unique(LinkedList *node, int p) {
	if(node == NULL)
		return NULL;
	else if(node->pid == p)
		return node;
	else 
		return unique(node->next, p);
}

void search_for_non_vol(LinkedList *head) {
	LinkedList * current = head;
	while (head != NULL && current != NULL) {
		
		int prevPID = current->unique_pid.value;
		
		if (current->next != NULL) 
			current = current->next;
		
		
		if(head->next != NULL) {
			
			if ((prevPID != current->unique_pid.value) && (exists(head->next, prevPID) == 1) && (current->next != NULL)) {
				head->nonvol = 1;
			}
			else if (prevPID == current->unique_pid.value) {
				head->nonvol = 0;
			}
			else if (exists(head->next, prevPID) == 0) { 
				head->nonvol = 0;
			}
		}
		head = head->next;
	}
}

int calculate_nonvol(LinkedList *head) {
	int sum = 0;
	while(head != NULL) {
		sum += head->nonvol;
		head = head->next;
	}
	return sum;
}

void insert(LinkedList **head, int pid_t, int burst_t, int priority_t) {
	if ((*head) == NULL) {
		(*head) = (LinkedList *) malloc(sizeof(LinkedList));
		(*head)->pid = pid_t;
		(*head)->burst = burst_t;
		(*head)->priority = priority_t;
		(*head)->unique_pid.total_runtime = 0;
		(*head)->nonvol = 0;
		(*head)->next = NULL;
	}
	else {
		insert(&(*head)->next, pid_t, burst_t, priority_t);

		if(unique((*head), (*head)->pid) == (*head)) {
			(*head)->unique_pid.value = (*head)->pid;
			(*head)->unique_pid.total_runtime = (*head)->burst;
        	(*head)->unique_pid.start_time = 0;
		}

		else {
			(*head)->unique_pid.total_runtime += (*head)->unique_pid.total_runtime;
		}
	}

}
int main(int argc, char *argv[]) {

	FILE *fp;
	char process[1024];
	char list_instructions[1024];
	char secondline[1024];
	const char whitespace[2] = " ";
	struct LinkedList *LinkedList = NULL;
	struct LinkedList *root = NULL;

	if (argc != 2) {
		printf("Error, must specify a file name\n");
		return EXIT_FAILURE;
	}
	fp = fopen(argv[1], "r");

	if(!fp) {
		perror("Error opening input file\n");
		return EXIT_FAILURE;
	}

	fgets(process, sizeof(process), fp);	
	int num_process = atoi(process);

	
	fgets(secondline, sizeof(secondline), fp);


	char* total_pid_c = strtok(secondline, whitespace);
	int unique_pid = atoi(total_pid_c);

	
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

	
	while(fgets(list_instructions, sizeof(list_instructions), fp) != NULL) {
		
		sscanf(list_instructions, "%d %d %d", &pid_c, &burst_c, &priority_c);

		
        total_burst += burst_c;

	
        burst_time[pid_c - 1] += burst_c;
       	completion_time[pid_c - 1] = total_burst;

		if(arrival_time[pid_c - 1] == 0) {
			arrival_time[pid_c - 1] = burst_c;
			first_time[pid_c - 1] = total_burst;
		}

		
		insert(&LinkedList, pid_c, burst_c, priority_c);
	}


	search_for_non_vol(LinkedList);	

	for(int i = 0; i < unique_pid; i++) {
      
        total_turnaround_time += completion_time[i];

    
        wait_time[i] = completion_time[i] - burst_time[i];
		
        total_wait_time += wait_time[i];

		
		response_time[i] = first_time[i] - arrival_time[i];
		
		total_response_time += response_time[i];
	}

	
	nonvol = calculate_nonvol(LinkedList);	

   
    float cpu = 100 / num_process;

    
    float throughput = (float)unique_pid / total_burst;


	double avg_turnaround_time = (double)total_turnaround_time / unique_pid;

	
	double avg_waiting_time = (double)total_wait_time / unique_pid;	

	
	float avg_response_time = (float)total_response_time / unique_pid;

	fprintf(stdout, "%d\n%d\n%.2f\n%.2f\n%.2lf\n%.2lf\n%.2f\n", unique_pid, nonvol, cpu, throughput, avg_turnaround_time, avg_waiting_time, avg_response_time);

	free(LinkedList);
	return EXIT_SUCCESS;
}

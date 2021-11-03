#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//sturct that stores the process objects
struct Process
{
    //variable for the process that need to run
    int pid;

    //variable for the amount of time neede to run on CPU
    int bursts;

    //varible for the priority of the process
    int priority;

}; typedef struct Process Process;

//gets Throughput i.e the amount of execution elements completed
void getThroughput(struct Process process, int pid, int bursts, float throughput)
{
    throughput = pid / bursts;
}

//gets TurnAround Time i.e the amount of time from submission to task completion
void getTurnAroundT(struct Process process, int n, int burstTime[], int waitTime[], int turnAround[])
{
    //calculates turn around time
    for(int i = 0; i < n; i++)
    {
        turnAround[i] = burstTime[i] + waitTime[i];
    }
}

//gets Waiting Time i.e the amount of time a task is waiting in the Ready queue
void getWaitingT(struct Process process, int n, int burstTime[], int waitingTime[])
{
     //first process's waiting time is 0
    waitingTime[0] = 0;

    //calculates waiting time
    for(int i = 1; i < n; i++)
    {
        waitingTime[i] = burstTime[i-1] + waitingTime[i-1];
    }
}
 
//gets Response Time i.e the average time it takes for a task to run
double getResponseT(struct Process process, int n, int burstTime[])
{
    //variables for turnAround and waiting times
    int turnAround[n], waitingTime[n];

    //gets the turn around time
    getTurnAroundT(process, n, burstTime, waitingTime, turnAround);

    //gets the waitingtime
    getWaitingT(process, n, burstTime, waitingTime);

    for(int i = 0; i < n; i++)
    {
        
    }

}

int main(int argc, char **argv)
{
    //variable for the number of process available to run instructions
    int process = 1;

    int num_p, num_N;

    double CPU_UTILIZATION = 100.00;

}

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <queue>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// compile with g++
struct process
{
    string processNum;
    double arrivalTime;
    double startTime;
    double executionTime;
    double remainingExcTime;
    void FCFS(double sTime, double endTime);
};

class scheduler
{
    process *inputProcesses;

public:
    scheduler(process *psc, double *st, double *et)
    {
        inputProcesses = psc;
        double *startTime = st;
        double *stopTime = et;
    }
    void FCFS(std::vector<scheduler *> &schedules);
    void preemptiveSJF(std::vector<scheduler *> &schedules);
    process *accessProcss()
    {
        return inputProcesses;
    }
    // void arrangeExeTime(std::vector<scheduler *> &schedules);
};

// make this schedular accept the array of the class
// this arranges the array in the order of scheduling in fcfs
void bubSrt(std::vector<scheduler *> &schedules, string type)
{
    int i = 0, j = 0;
    for (i = 0; i < schedules.size() - 1; i++)
    {
        // Last i elements are already in place
        if (type == "arrivalTime")
        {
            for (j = 0; j < schedules.size() - i - 1; j++)
            {
                if (schedules[j]->accessProcss()->arrivalTime > schedules[j + 1]->accessProcss()->arrivalTime)
                {
                    auto newTmp = schedules[j];
                    schedules[j] = schedules[j + 1];
                    schedules[j + 1] = newTmp;
                }
                if (schedules[j]->accessProcss()->arrivalTime == schedules[j + 1]->accessProcss()->arrivalTime && schedules[j]->accessProcss()->executionTime > schedules[j + 1]->accessProcss()->executionTime)
                {
                    auto newTmp = schedules[j];
                    schedules[j] = schedules[j + 1];
                    schedules[j + 1] = newTmp;
                }
            }
        }
        else
        {
            for (j = 0; j < schedules.size() - i - 1; j++)
            {
                if (schedules[j]->accessProcss()->executionTime >= schedules[j + 1]->accessProcss()->executionTime)
                {
                    auto newTmp = schedules[j];
                    schedules[j] = schedules[j + 1];
                    schedules[j + 1] = newTmp;
                }
            }
        }
    }
}
void scheduler::FCFS(vector<scheduler *> &schedules)
{
    double throughput, responseTime, turnAroundTime = 0;
    double totalEt = 0;
    // now calculate the time and send it to the function
    bubSrt(schedules, "arrivalTime");
    int i = 0;
    double totaltime = 0.0;
    double endtime = 0.0;
    double stime = schedules[0]->inputProcesses->arrivalTime;
    for (int i = 0; i < schedules.size(); i++)
    {
        totalEt += schedules[i]->inputProcesses->executionTime;
    }
    totalEt += schedules[0]->inputProcesses->arrivalTime;
    cout << "Scheduling using FCFS" << endl;

    for (i = 0; i < schedules.size(); i++)
    {
        endtime = stime + schedules[i]->inputProcesses->executionTime;
        totaltime = totaltime + schedules[i]->inputProcesses->executionTime;
        double tmpAT = schedules[i]->accessProcss()->arrivalTime;
        double tmpRT = stime - tmpAT;
        responseTime = responseTime + tmpRT;
        double tmpTRT = endtime - tmpAT;
        turnAroundTime = turnAroundTime + tmpTRT;
        cout << " ========== " << endl;
        schedules[i]->inputProcesses->FCFS(stime, endtime);
        stime = endtime;
    }
    cout << "\n the average response time is " << responseTime / schedules.size() << endl;
    cout << "\n the average turnaround time is " << turnAroundTime / schedules.size() << endl;
    cout << "\n the average throughput time is " << schedules.size() / totaltime << endl;
}
void scheduler::preemptiveSJF(vector<scheduler *> &schedules)
{
    auto localSchedules = schedules;
    bubSrt(localSchedules, "arrivalTime");
    double clock = localSchedules[0]->accessProcss()->arrivalTime;
    vector<scheduler *> unfinisedPsc;
    int totalEt = 0;
    for (int i = 0; i < schedules.size(); i++)
    {
        totalEt += schedules[i]->inputProcesses->executionTime;
    }
    totalEt += schedules[0]->inputProcesses->arrivalTime;
    // answers
    double throughput, responseTime, turnAroundTime = 0;
    // loop
    while (clock < totalEt)
    {
        if (localSchedules.empty() == false)
        {
            if (schedules.size() == localSchedules.size())
            {
                // cout << "The process " << localSchedules[0]->accessProcss()->processNum << " has strarted at " << clock << endl;
                localSchedules[0]->accessProcss()->startTime = clock;
                localSchedules[0]->accessProcss()->remainingExcTime = localSchedules[0]->accessProcss()->remainingExcTime - 1;
                unfinisedPsc.push_back(localSchedules[0]);
                clock++;
                // check if finishes
                // function to print when it finishes
                if (unfinisedPsc[0]->accessProcss()->remainingExcTime == 0)
                {
                    cout << "the process " << unfinisedPsc[0]->accessProcss()->processNum << " started at " << unfinisedPsc[0]->accessProcss()->startTime << " stopped at " << clock << endl;
                    // cout << " the arival time is " << unfinisedPsc[0]->accessProcss()->arrivalTime << endl;
                    double tmpRT = unfinisedPsc[0]->accessProcss()->startTime - unfinisedPsc[0]->accessProcss()->arrivalTime;
                    responseTime = responseTime + tmpRT;
                    double tmpAT = unfinisedPsc[0]->accessProcss()->arrivalTime;
                    double tmpTRT = clock - tmpAT;
                    turnAroundTime = turnAroundTime + tmpTRT;
                    unfinisedPsc.erase(unfinisedPsc.begin());
                    clock--;
                }
                bubSrt(unfinisedPsc, "executionTime");
                if (!localSchedules.empty())
                {
                    localSchedules.erase(localSchedules.begin());
                }

                // check if the next process also has the same starting time but more execution time
                // it will be skipped by the clock and it order to account for it we manually add it to the unfinised stack and keep its starting value -1
                // so we can keep track of it
                if (unfinisedPsc[0]->accessProcss()->arrivalTime == localSchedules[0]->accessProcss()->arrivalTime && localSchedules.size() != 0)
                {
                    localSchedules[0]->accessProcss()->startTime = -1;
                    unfinisedPsc.push_back(localSchedules[0]);
                    localSchedules.erase(localSchedules.begin());
                    bubSrt(unfinisedPsc, "executionTime");
                }
            }
            // new process with more execution time
            else if ((clock == localSchedules[0]->accessProcss()->arrivalTime) && localSchedules[0]->accessProcss()->executionTime > unfinisedPsc[0]->accessProcss()->executionTime)
            {

                // cout << "The process " << localSchedules[0]->accessProcss()->processNum << " is not because the exection time is more than the process before" << endl;
                // cout << "resuming the process which was already running" << endl;
                if (unfinisedPsc[0]->accessProcss()->startTime == -1)
                {
                    unfinisedPsc[0]->accessProcss()->startTime = clock;
                }
                unfinisedPsc[0]->accessProcss()->remainingExcTime -= 1;
                clock++;
                if (unfinisedPsc[0]->accessProcss()->remainingExcTime == 0)
                {
                    cout << "the process " << unfinisedPsc[0]->accessProcss()->processNum << " started at " << unfinisedPsc[0]->accessProcss()->startTime << " stopped at " << clock << endl;
                    double tmpRT = unfinisedPsc[0]->accessProcss()->startTime - unfinisedPsc[0]->accessProcss()->arrivalTime;
                    responseTime = responseTime + tmpRT;
                    double tmpAT = unfinisedPsc[0]->accessProcss()->arrivalTime;
                    double tmpTRT = clock - tmpAT;
                    turnAroundTime = turnAroundTime + tmpTRT;
                    unfinisedPsc.erase(unfinisedPsc.begin());
                    clock--;
                }

                localSchedules[0]->accessProcss()->startTime = -1;
                unfinisedPsc.push_back(localSchedules[0]);

                bubSrt(unfinisedPsc, "executionTime");
                if (!localSchedules.empty())
                {
                    localSchedules.erase(localSchedules.begin());
                }
            }
            else if (clock != localSchedules[0]->accessProcss()->arrivalTime)
            {
                if (unfinisedPsc[0]->accessProcss()->startTime == -1)
                {
                    unfinisedPsc[0]->accessProcss()->startTime = clock;
                }
                unfinisedPsc[0]->accessProcss()->remainingExcTime -= 1;
                clock++;
                if (unfinisedPsc[0]->accessProcss()->remainingExcTime == 0)
                {
                    cout << "the process " << unfinisedPsc[0]->accessProcss()->processNum << " started at " << unfinisedPsc[0]->accessProcss()->startTime << " stopped at " << clock << endl;
                    double tmpRT = unfinisedPsc[0]->accessProcss()->startTime - unfinisedPsc[0]->accessProcss()->arrivalTime;
                    responseTime = responseTime + tmpRT;
                    double tmpAT = unfinisedPsc[0]->accessProcss()->arrivalTime;
                    double tmpTRT = clock - tmpAT;
                    turnAroundTime = turnAroundTime + tmpTRT;
                    unfinisedPsc.erase(unfinisedPsc.begin());
                    bubSrt(unfinisedPsc, "executionTime");
                    clock--;
                }
                bubSrt(unfinisedPsc, "executionTime");
            }
            else if ((clock == localSchedules[0]->accessProcss()->arrivalTime) && localSchedules[0]->accessProcss()->executionTime < unfinisedPsc[0]->accessProcss()->executionTime)
            {
                // cout << "The process " << localSchedules[0]->accessProcss()->processNum << " is starting at " << clock << endl;
                // cout << "The process " << unfinisedPsc[0]->accessProcss()->processNum << " has been stopped at " << clock << endl;

                // only add the starting time for the processes which are in the
                localSchedules[0]->accessProcss()->startTime = clock;
                localSchedules[0]->accessProcss()->remainingExcTime -= 1;
                clock++;
                if (unfinisedPsc[0]->accessProcss()->startTime == -1)
                {
                    unfinisedPsc[0]->accessProcss()->startTime = clock;
                }
                if (unfinisedPsc[0]->accessProcss()->remainingExcTime == 0)
                {

                    cout << "the process " << unfinisedPsc[0]->accessProcss()->processNum << " started at " << unfinisedPsc[0]->accessProcss()->startTime << " stopped at " << clock << endl;
                    double tmpRT = unfinisedPsc[0]->accessProcss()->startTime - unfinisedPsc[0]->accessProcss()->arrivalTime;
                    responseTime = responseTime + tmpRT;
                    double tmpAT = unfinisedPsc[0]->accessProcss()->arrivalTime;
                    double tmpTRT = clock - tmpAT;
                    turnAroundTime = turnAroundTime + tmpTRT;
                    unfinisedPsc.erase(unfinisedPsc.begin());
                    bubSrt(unfinisedPsc, "executionTime");
                    clock--;
                }
                unfinisedPsc.push_back(localSchedules[0]);
                bubSrt(unfinisedPsc, "executionTime");
                if (!localSchedules.empty())
                {
                    localSchedules.erase(localSchedules.begin());
                }

                if (unfinisedPsc[0]->accessProcss()->arrivalTime == localSchedules[0]->accessProcss()->arrivalTime && localSchedules.size() != 0)
                {

                    localSchedules[0]->accessProcss()->startTime = -1;
                    unfinisedPsc.push_back(localSchedules[0]);
                    localSchedules.erase(localSchedules.begin());
                    bubSrt(unfinisedPsc, "executionTime");
                }
            }
            // incoming process has same execution time as the curenntly running process make
            // the currently running process run to reduce the over head of switching
            else if ((clock == localSchedules[0]->accessProcss()->arrivalTime) && localSchedules[0]->accessProcss()->executionTime == unfinisedPsc[0]->accessProcss()->executionTime)
            {
                unfinisedPsc.push_back(localSchedules[0]);
                bubSrt(unfinisedPsc, "executionTime");
                unfinisedPsc[0]->accessProcss()->startTime = clock;
                if (!localSchedules.empty())
                {
                    localSchedules.erase(localSchedules.begin());
                }
                // cout << " The new process " << unfinisedPsc[0]->accessProcss()->processNum << " will be running" << endl;
                unfinisedPsc[0]->accessProcss()->remainingExcTime -= 1;
                clock++;
                if (unfinisedPsc[0]->accessProcss()->remainingExcTime == 0)
                {

                    // unfinisedPsc[0]->accessProcss()->preemptiveSJF(localSchedules[0]->accessProcss()->startTime, clock);
                    cout << "the process " << unfinisedPsc[0]->accessProcss()->processNum << " started at " << unfinisedPsc[0]->accessProcss()->startTime << " stopped at " << clock << endl;
                    double tmpRT = unfinisedPsc[0]->accessProcss()->startTime - unfinisedPsc[0]->accessProcss()->arrivalTime;
                    responseTime = responseTime + tmpRT;
                    double tmpAT = unfinisedPsc[0]->accessProcss()->arrivalTime;
                    double tmpTRT = clock - tmpAT;
                    turnAroundTime = turnAroundTime + tmpTRT;
                    if (!unfinisedPsc.empty())
                    {

                        unfinisedPsc.erase(unfinisedPsc.begin());
                        clock--;
                    }
                    bubSrt(unfinisedPsc, "executionTime");
                }

                // check if the next process also has the same starting time but more execution time
                // it will be skipped by the clock and it order to account for it we manually add it to the unfinised stack and keep its starting value -1
                // so we can keep track of it
                if (unfinisedPsc[0]->accessProcss()->arrivalTime == localSchedules[0]->accessProcss()->arrivalTime && localSchedules.size() != 0)
                {
                    localSchedules[0]->accessProcss()->startTime = -1;
                    unfinisedPsc.push_back(localSchedules[0]);
                    localSchedules.erase(localSchedules.begin());
                    bubSrt(unfinisedPsc, "executionTime");
                }
            }
        }
        else
        { 
            for (int i = 0; i < unfinisedPsc.size(); i++)
            {
                if (unfinisedPsc[i]->accessProcss()->startTime == -1)
                {
                    unfinisedPsc[i]->accessProcss()->startTime = clock;
                }
                clock = clock + unfinisedPsc[i]->accessProcss()->remainingExcTime;
                if (i == 1)
                {
                    clock++;
                }
                // clock++;
                cout << "the clock " << clock << endl;
                cout << "the process " << unfinisedPsc[i]->accessProcss()->processNum << " started at " << unfinisedPsc[i]->accessProcss()->startTime << " stopped at " << clock << endl;
                double tmpRT = unfinisedPsc[i]->accessProcss()->startTime - unfinisedPsc[i]->accessProcss()->arrivalTime;
                responseTime = responseTime + tmpRT;
                double tmpAT = unfinisedPsc[0]->accessProcss()->arrivalTime;
                double tmpTRT = clock - tmpAT;
                turnAroundTime = turnAroundTime + tmpTRT;
            }
            break;
        }
    }
    // find out average through put, response and turn around time
    // responseTime = responseTime;
    cout << "\n the average response time is " << responseTime / schedules.size() << endl;
    cout << "\n the average turnaround time is " << turnAroundTime / schedules.size() << endl;
    cout << "\n the average throughput time is " << schedules.size() / clock << endl;
}

void process::FCFS(double sTime, double endTime)
{
    // write code here to print the schedule for given processes
    // write the start time and the stop time

    cout << "Schedule for process " << processNum << endl;
    cout << "Start time \t Stop time" << endl;
    cout << sTime << "\t \t" << endTime << endl;
}

int main(int argc, char **argv)
{
    // no need to create process
    //  write code here to create schedule for the input processes
    //  loop to read through the arguments
    int i = 0;
    // find out the number of processes entered by the user
    // int numProcess = argc / 3;
    std::vector<scheduler *> schedules;
    char type;
    int tmpSt, tmpEt;
    string name = " ";
    // creating objects of class schedular and storing inside the vector
    for (int i = 1; i < argc - 1; i = i + 3)
    {
        // cout << argv[i] << endl;
        //  create an object of the class
        if ((i + 1) < argc)
        {
            name = argv[i];
            tmpSt = atoi(argv[i + 1]); // this is the starting time
            tmpEt = atoi(argv[i + 2]); // this is the ending time
        }
        // passing the starting and the ending object
        auto tmProcess = new process;
        tmProcess->arrivalTime = tmpSt;
        tmProcess->executionTime = tmpEt;
        tmProcess->remainingExcTime = tmpEt;
        tmProcess->processNum = name;
        auto tmp = new scheduler(tmProcess, NULL, NULL);
        schedules.push_back(tmp);

        // cout << "the starting time fOr P" << tmProcess->processNum << " is " << schedules[i-1]->inputProcesses->arrivalTime << " the execution time is " << tmpEt << endl;
    }
    scheduler newTmp(NULL, NULL, NULL);
    if (string(argv[argc - 1]) == "FCFS")
        newTmp.FCFS(schedules);
    else
        newTmp.preemptiveSJF(schedules);
    return 0;
}
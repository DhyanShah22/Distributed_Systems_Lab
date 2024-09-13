#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <fstream>
#include <mutex>
#include <atomic>

using namespace std;

class Process {
public:
    int pid;
    atomic<bool> isLeader;
    atomic<bool> isAlive;
    vector<Process*> *processes;
    mutex &outputMutex;
    ofstream &outputFile;

    Process(int pid, vector<Process*> *processes, mutex &outputMutex, ofstream &outputFile)
        : pid(pid), isLeader(false), isAlive(true), processes(processes), outputMutex(outputMutex), outputFile(outputFile) {}

    void start() {
        while (true) {
            if (isAlive) {
                checkLeader();
            }
            this_thread::sleep_for(chrono::milliseconds(500));  // Reduced sleep time for faster response
        }
    }

    void checkLeader() {
        if (isLeader) return;  // If it's already a leader, do nothing

        bool leaderAlive = false;
        for (auto &proc : *processes) {
            if (proc->isLeader && proc->isAlive) {
                leaderAlive = true;
                break;
            }
        }

        if (!leaderAlive) {
            initiateElection();
        }
    }

    void initiateElection() {
        logOutput("Process " + to_string(pid) + " initiating election.");

        for (auto &proc : *processes) {
            if (proc->pid > pid && proc->isAlive) {
                logOutput("Process " + to_string(pid) + " sending election message to Process " + to_string(proc->pid));
                if (proc->respondToElection()) {
                    return;
                }
            }
        }

        becomeLeader();
    }

    bool respondToElection() {
        if (isAlive) {
            logOutput("Process " + to_string(pid) + " responds to election.");
            return true;
        }
        return false;
    }

    void becomeLeader() {
        isLeader = true;
        logOutput("Process " + to_string(pid) + " becomes the new leader.");

        for (auto &proc : *processes) {
            if (proc != this && proc->isAlive) {
                logOutput("Process " + to_string(pid) + " informs Process " + to_string(proc->pid) + " about new leadership.");
            }
        }
    }

    void fail() {
        isAlive = false;
        logOutput("Process " + to_string(pid) + " has failed.");
    }

    void recover() {
        isAlive = true;
        logOutput("Process " + to_string(pid) + " has recovered.");
        if (!isLeader) {
            initiateElection();
        }
    }

private:
    void logOutput(const string &message) {
        lock_guard<mutex> lock(outputMutex);
        outputFile << message << endl;
        cout << message << endl;
    }
};

void simulate(vector<Process*> &processes, mutex &outputMutex, ofstream &outputFile) {
    vector<thread> threads;

    for (auto &proc : processes) {
        threads.emplace_back(&Process::start, proc);
    }

    this_thread::sleep_for(chrono::seconds(1));
    processes.back()->fail(); // Simulate leader failure

    this_thread::sleep_for(chrono::seconds(2));
    processes.back()->recover(); // Simulate leader recovery

    for (auto &t : threads) {
        t.detach();  // Detach threads for faster termination
    }
}

int main() {
    ofstream outputFile("bully_election_output1.txt");
    mutex outputMutex;

    vector<Process*> processes;
    for (int i = 1; i <= 5; ++i) {
        processes.push_back(new Process(i, &processes, outputMutex, outputFile));
    }

    processes.back()->isLeader = true; 
    outputFile << "Initial leader is Process " << processes.back()->pid << endl;
    cout << "Initial leader is Process " << processes.back()->pid << endl;

    simulate(processes, outputMutex, outputFile);

    this_thread::sleep_for(chrono::seconds(5));  

    for (auto &proc : processes) {
        delete proc;
    }

    outputFile.close();
    return 0;
}

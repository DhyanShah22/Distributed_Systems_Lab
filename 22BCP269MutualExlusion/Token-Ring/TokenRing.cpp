#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <fstream>
#include <chrono>
#include <condition_variable>
#include <atomic>
#include <map>

std::mutex mtx; // Mutex for mutual exclusion
std::condition_variable cv; // Condition variable for synchronization
std::atomic<bool> running(true); // Flag to control the dynamic joining loop
std::ofstream outfile("Token_Based_output.txt", std::ios::out | std::ios::app); // Output file

std::map<int, std::atomic<bool>> process_status; // Track status of each process (active or failed)

struct Process {
    int id;
    bool hasToken;
    std::atomic<bool> inCriticalSection;
    Process* nextProcess; // Pointer to the next process in the ring

    Process(int id, bool hasToken = false) : id(id), hasToken(hasToken), inCriticalSection(false), nextProcess(nullptr) {
        process_status[id] = true; // Mark process as active
    }

    void requestCriticalSection() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&] { return hasToken && process_status[id]; });
        inCriticalSection = true;
        outfile << "Process " << id << " has entered critical section\n";
        outfile.flush(); // Ensure immediate write
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate critical section work
        inCriticalSection = false;
        outfile << "Process " << id << " has exited critical section\n";
        outfile.flush(); // Ensure immediate write
        hasToken = false; // Pass token to next process
        if (nextProcess) {
            nextProcess->receiveToken();
        }
        cv.notify_all();
    }

    void receiveToken() {
        std::unique_lock<std::mutex> lock(mtx);
        hasToken = true;
        cv.notify_all();
    }

    void simulateFailure() {
        process_status[id] = false; // Simulate a failure
        outfile << "Process " << id << " failed\n";
        outfile.flush(); // Ensure immediate write
    }

    void recover() {
        std::unique_lock<std::mutex> lock(mtx);
        process_status[id] = true; // Mark process as recovered
        outfile << "Process " << id << " recovered\n";
        outfile.flush(); // Ensure immediate write
        cv.notify_all();
    }
};

void tokenThread(Process &p) {
    for (int i = 0; i < 3; ++i) {
        if (!process_status[p.id]) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Wait and check for process recovery
            continue;
        }
        p.requestCriticalSection();
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate non-critical section work
    }
}

void dynamicJoin(std::vector<Process*> &processes, std::vector<std::thread> &threads) {
    int new_id = processes.size() + 1;
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Check every second
        Process* new_process = new Process(new_id);
        processes.push_back(new_process);

        // Add new process to the ring
        if (processes.size() > 1) {
            Process* last_process = processes[processes.size() - 2];
            last_process->nextProcess = new_process;
            new_process->nextProcess = processes[0]; // Form a ring
        } else {
            new_process->nextProcess = new_process; // Single process case
        }

        // Start a thread for the new process
        threads.emplace_back(tokenThread, std::ref(*new_process));
        outfile << "New process " << new_id << " joined the system\n";
        outfile.flush(); // Ensure immediate write
        new_id++;
    }
}

void monitorProcesses(std::vector<Process*> &processes) {
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        for (auto &p : processes) {
            if (!process_status[p->id]) {
                p->recover();
            }
        }
    }
}

int main() {
    // Initialize processes with the first process holding the token
    Process p1(1, true);
    Process p2(2);
    Process p3(3);
    std::vector<Process*> processes = {&p1, &p2, &p3};
    std::vector<std::thread> threads;

    // Set up ring
    p1.nextProcess = &p2;
    p2.nextProcess = &p3;
    p3.nextProcess = &p1; // Complete the ring

    for (auto &p : processes) {
        threads.emplace_back(tokenThread, std::ref(*p));
    }

    std::thread dynamic_thread(dynamicJoin, std::ref(processes), std::ref(threads));
    std::thread monitor_thread(monitorProcesses, std::ref(processes));

    std::this_thread::sleep_for(std::chrono::seconds(10)); // Let the simulation run for some time
    running = false; // Stop dynamic joining and monitoring threads

    dynamic_thread.join();
    monitor_thread.join();
    for (auto &th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }

    // Clean up dynamically allocated processes
    for (auto &p : processes) {
        delete p;
    }

    outfile.close();
    return 0;
}

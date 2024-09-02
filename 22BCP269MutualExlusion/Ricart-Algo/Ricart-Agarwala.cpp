#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <fstream>
#include <chrono>
#include <queue>
#include <condition_variable>
#include <algorithm>
#include <atomic>
#include <map>

std::mutex mtx; // Mutex for mutual exclusion
std::condition_variable cv; // Condition variable for synchronization
bool in_critical_section = false; // Flag to indicate if a process is in the critical section
std::queue<int> request_queue; // Queue to manage requests
std::ofstream outfile("Ricart_Agarwala_output.txt", std::ios::out | std::ios::app); // Output file
std::atomic<bool> running(true); // Flag to control the dynamic joining loop
std::map<int, std::atomic<bool>> process_status; // Track status of each process (active or failed)

struct Process {
    int id;
    bool requesting;
    int timestamp;

    Process(int id) : id(id), requesting(false), timestamp(0) {
        process_status[id] = true; // Mark process as active
    }

    void requestCriticalSection(int ts) {
        std::unique_lock<std::mutex> lock(mtx);
        requesting = true;
        timestamp = ts;
        outfile << "Process " << id << " requested critical section at timestamp " << ts << std::endl;
        request_queue.push(id);
        cv.notify_all();
    }

    void releaseCriticalSection() {
        std::unique_lock<std::mutex> lock(mtx);
        requesting = false;
        outfile << "Process " << id << " released critical section\n";
        in_critical_section = false;
        cv.notify_all();
    }

    void enterCriticalSection() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&] { return !in_critical_section && request_queue.front() == id; });
        in_critical_section = true;
        outfile << "Process " << id << " entered critical section\n";
    }

    void simulateFailure() {
        process_status[id] = false; // Simulate a failure
        outfile << "Process " << id << " failed\n";
    }

    void recover() {
        std::unique_lock<std::mutex> lock(mtx);
        process_status[id] = true; // Mark process as recovered
        outfile << "Process " << id << " recovered\n";
        cv.notify_all();
    }
};

void processThread(Process &p) {
    for (int i = 0; i < 3; ++i) { // Simulate multiple requests
        if (!process_status[p.id]) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Wait and check if process is recovered
            continue;
        }
        int ts = std::chrono::system_clock::now().time_since_epoch().count();
        p.requestCriticalSection(ts);
        p.enterCriticalSection();
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate critical section work
        p.releaseCriticalSection();
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate non-critical section work
    }
}

void dynamicJoin(std::vector<Process> &processes, std::vector<std::thread> &threads) {
    int new_id = processes.size() + 1;
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Check every second
        Process new_process(new_id);
        processes.push_back(new_process);
        threads.emplace_back(processThread, std::ref(processes.back()));
        outfile << "New process " << new_id << " joined the system\n";
        new_id++;
    }
}

void monitorProcesses(std::vector<Process> &processes) {
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        for (auto &p : processes) {
            if (!process_status[p.id]) {
                p.recover();
            }
        }
    }
}

int main() {
    int num_processes = 3;
    std::vector<std::thread> threads;
    std::vector<Process> processes;

    for (int i = 0; i < num_processes; ++i) {
        processes.emplace_back(i + 1);
    }

    for (int i = 0; i < num_processes; ++i) {
        threads.emplace_back(processThread, std::ref(processes[i]));
    }

    std::thread dynamic_thread(dynamicJoin, std::ref(processes), std::ref(threads));
    std::thread monitor_thread(monitorProcesses, std::ref(processes));

    std::this_thread::sleep_for(std::chrono::seconds(10)); // Let the simulation run for some time
    running = false; // Stop the dynamic joining and monitoring threads

    dynamic_thread.join();
    monitor_thread.join();
    for (auto &th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }

    outfile.close();
    return 0;
}

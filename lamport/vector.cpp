#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <random>
#include <chrono>
#include <queue>
#include <map>
#include <fstream>  // Include for file handling

using namespace std;

std::mutex mtx;
std::condition_variable cv;
std::map<int, std::queue<std::vector<int>>> message_buffers;
std::vector<std::vector<int>> vector_clocks;
std::vector<std::mutex*> buffer_mutexes;

void print_event_order(int process_id, const std::vector<std::vector<int>>& event_order, std::ofstream& file) {
    std::cout << "Process " << process_id << " event order: ";
    file << "Process " << process_id << " event order: ";
    for (const auto& event : event_order) {
        std::cout << "[";
        file << "[";
        for (int clock : event) {
            std::cout << clock << " ";
            file << clock << " ";
        }
        std::cout << "] ";
        file << "] ";
    }
    std::cout << std::endl;
    file << std::endl;
}

void thread_func(int process_id, int num_iterations, int num_processes, std::ofstream& file) {
    std::vector<std::vector<int>> event_order;
    std::vector<int>& vector_clock = vector_clocks[process_id];
    std::queue<std::vector<int>> local_buffer;
    
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0, 2);
    
    for (int i = 0; i < num_iterations; ++i) {
        // Internal event
        std::this_thread::sleep_for(std::chrono::milliseconds(distribution(generator) * 100));
        vector_clock[process_id]++;
        event_order.push_back(vector_clock);
        
        // Sending message
        int recipient_id = (process_id + 1) % num_processes;
        {
            std::lock_guard<std::mutex> lock(*buffer_mutexes[recipient_id]);
            message_buffers[recipient_id].push(vector_clock);
        }
        cv.notify_all();
        
        // Receiving message
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&] { return !message_buffers[process_id].empty(); });
        
        std::lock_guard<std::mutex> buffer_lock(*buffer_mutexes[process_id]);
        while (!message_buffers[process_id].empty()) {
            std::vector<int> received_clock = message_buffers[process_id].front();
            message_buffers[process_id].pop();
            for (int j = 0; j < num_processes; ++j) {
                vector_clock[j] = std::max(vector_clock[j], received_clock[j]);
            }
            vector_clock[process_id]++;
            event_order.push_back(vector_clock);
        }
        
        print_event_order(process_id, event_order, file);
    }
}

int main() {
    int num_processes, num_iterations;
    
    std::cout << "Enter the number of processes: ";
    std::cin >> num_processes;
    
    std::cout << "Enter the number of iterations: ";
    std::cin >> num_iterations;

    // Open a file to store the output
    std::ofstream file("output.txt");
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }
    
    vector_clocks.resize(num_processes, std::vector<int>(num_processes, 0));
    
    // Initialize message_buffers map with empty queues
    for (int i = 0; i < num_processes; ++i) {
        message_buffers[i] = std::queue<std::vector<int>>();
    }

    // Initialize buffer_mutexes with pointers to std::mutex objects
    std::vector<std::mutex*> mutex_pointers;
    for (int i = 0; i < num_processes; ++i) {
        std::mutex* mutex_ptr = new std::mutex();
        mutex_pointers.push_back(mutex_ptr);
    }
    buffer_mutexes = mutex_pointers;

    std::vector<std::thread> threads;
    for (int i = 0; i < num_processes; ++i) {
        threads.emplace_back(thread_func, i, num_iterations, num_processes, std::ref(file));
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    // Clean up dynamically allocated mutexes
    for (auto mutex_ptr : buffer_mutexes) {
        delete mutex_ptr;
    }

    file.close();  // Close the file
    
    return 0;
}

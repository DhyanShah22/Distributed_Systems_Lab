#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <random>
#include <chrono>
#include <queue>
#include <map>

std::mutex mtx;
std::condition_variable cv;
std::map<int, std::queue<int>> message_buffers;
std::vector<int> logical_clocks;
std::vector<std::mutex*> buffer_mutexes;

void print_event_order(int thread_id, const std::vector<int>& event_order) {
	std::cout << "Thread " << thread_id << " event order: ";
	for (int event : event_order) {
    	std::cout << event << " ";
	}
	std::cout << std::endl;
}
void thread_func(int thread_id, int num_iterations) {
	std::vector<int> event_order;
	int& logical_clock = logical_clocks[thread_id];
	std::queue<int> local_buffer;
    
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(0, 2);
    
	for (int i = 0; i < num_iterations; ++i) {
    	// Internal event
    	std::this_thread::sleep_for(std::chrono::milliseconds(distribution(generator) * 100));
    	logical_clock++;
    	event_order.push_back(logical_clock);
   	 
    	// Sending message
    	int recipient_id = (thread_id + 1) % logical_clocks.size();
    	{
        	std::lock_guard<std::mutex> lock(*buffer_mutexes[recipient_id]);
        	message_buffers[recipient_id].push(logical_clock);
    	}
    	cv.notify_all();
   	 
    	// Receiving message
    	std::unique_lock<std::mutex> lock(mtx);
    	cv.wait(lock, [&] { return !message_buffers[thread_id].empty(); });
   	 
    	std::lock_guard<std::mutex> buffer_lock(*buffer_mutexes[thread_id]);
    	while (!message_buffers[thread_id].empty()) {
        	int received_clock = message_buffers[thread_id].front();
        	message_buffers[thread_id].pop();
        	logical_clock = std::max(logical_clock, received_clock) + 1;
        	event_order.push_back(logical_clock);
    	}
   	 
    	print_event_order(thread_id, event_order);
	}
}

int main() {
	int num_threads = 2;  // Updated to use 2 processes (threads)
	int num_iterations = 5;
    
	logical_clocks.resize(num_threads, 0);
    
	// Initialize message_buffers map with empty queues
	for (int i = 0; i < num_threads; ++i) {
    	message_buffers[i] = std::queue<int>();
	}

	// Initialize buffer_mutexes with pointers to std::mutex objects
	std::vector<std::mutex*> mutex_pointers;
	for (int i = 0; i < num_threads; ++i) {
    	std::mutex* mutex_ptr = new std::mutex();
    	mutex_pointers.push_back(mutex_ptr);
	}
	buffer_mutexes = mutex_pointers;

	std::vector<std::thread> threads;
	for (int i = 0; i < num_threads; ++i) {
    	threads.emplace_back(thread_func, i, num_iterations);
	}
    
	for (auto& t : threads) {
    	t.join();
	}
    
	// Clean up dynamically allocated mutexes
	for (auto mutex_ptr : buffer_mutexes) {
    	delete mutex_ptr;
	}
	return 0;
}

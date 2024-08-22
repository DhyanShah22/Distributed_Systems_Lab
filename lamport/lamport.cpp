#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <random>

#define NUM_THREADS 3
#define NUM_ITERATIONS 10
#define BUFFER_SIZE 5

struct Message {
    int sender_id;
    int timestamp;
};

class BoundedBuffer {
private:
    std::queue<Message> buffer;
    std::mutex mtx;
    std::condition_variable not_full, not_empty;

public:
    void addMessage(const Message& msg) {
        std::unique_lock<std::mutex> lock(mtx);
        not_full.wait(lock, [this] { return buffer.size() < BUFFER_SIZE; });
        buffer.push(msg);
        not_empty.notify_all();
    }

    Message getMessage() {
        std::unique_lock<std::mutex> lock(mtx);
        not_empty.wait(lock, [this] { return !buffer.empty(); });
        Message msg = buffer.front();
        buffer.pop();
        not_full.notify_all();
        return msg;
    }

    bool isEmpty() {
        std::lock_guard<std::mutex> lock(mtx);
        return buffer.empty();
    }
};

class Thread {
private:
    int id;
    int clock;
    std::vector<std::string> event_log;
    BoundedBuffer& buffer;
    std::mt19937 rng;

    void internalEvent() {
        clock++;
        event_log.push_back("Thread " + std::to_string(id) + " internal event at time " + std::to_string(clock));
    }

    void sendMessage() {
        clock++;
        Message msg = {id, clock};
        buffer.addMessage(msg);
        event_log.push_back("Thread " + std::to_string(id) + " sent message at time " + std::to_string(clock));
    }

    void receiveMessage() {
        Message msg = buffer.getMessage();
        if (msg.sender_id != id) {
            clock = std::max(clock, msg.timestamp) + 1;
            event_log.push_back("Thread " + std::to_string(id) + " received message from Thread " + std::to_string(msg.sender_id) + " at time " + std::to_string(clock));
        }
    }

    void randomSleep() {
        std::uniform_int_distribution<int> dist(100, 500);
        std::this_thread::sleep_for(std::chrono::milliseconds(dist(rng)));
    }

public:
    Thread(int id, BoundedBuffer& buf) : id(id), clock(0), buffer(buf), rng(std::random_device{}()) {}

    void run() {
        for (int i = 0; i < NUM_ITERATIONS; i++) {
            randomSleep();
            internalEvent();

            randomSleep();
            if (rng() % 2 == 0) {
                sendMessage();
            } else {
                receiveMessage();
            }
        }

        // Print event log
        std::cout << "Thread " << id << " event log:\n";
        for (const auto& log : event_log) {
            std::cout << log << std::endl;
        }
    }
};

int main() {
    BoundedBuffer buffer;
    std::vector<std::thread> threads;
    std::vector<Thread*> thread_objects;

    // Create and start threads
    for (int i = 0; i < NUM_THREADS; i++) {
        Thread* t = new Thread(i, buffer);
        thread_objects.push_back(t);
        threads.emplace_back(&Thread::run, t);
    }

    // Join threads
    for (auto& th : threads) {
        th.join();
    }

    // Clean up
    for (auto t : thread_objects) {
        delete t;
    }

    return 0;
}

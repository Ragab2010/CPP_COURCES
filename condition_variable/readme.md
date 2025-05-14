##  The code & explanation  for  Deadlocks ,the lock tags

```cpp
/*
     ///////////////////
    📬 std::condition_variable
        🔔 What Is It?

        A mechanism for blocking a thread until notified by another thread.
        Used for producer/consumer, event waiting, etc.

        🛠️ Components:
        std::mutex for protection.->reasone for blocking 
        std::condition_variable to wait/notify.-> for notify
        std::unique_lock<std::mutex> to work with the condition variable.(unique_lock more flexible).

        Example: Wait for a Flag:
            #include <condition_variable>
            #include <mutex>
            #include <thread>
            #include <iostream>

            std::mutex mtx;
            std::condition_variable cv;
            bool ready = false;

            void worker() {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [] { return ready; }); // Wait until ready == true
                std::cout << "Worker thread proceeds\n";
            }

            int main() {
                std::thread t(worker);

                std::this_thread::sleep_for(std::chrono::seconds(1));
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    ready = true;
                }
                cv.notify_one(); // Wake up waiting thread

                t.join();
            }
        Key Operations Supported by std::condition_variable:
            wait(): Blocks the current thread until notified.
            wait_for(): Blocks the current thread for a specific duration or until notified.
            wait_until(): Blocks the current thread until a specific time point or until notified.
            notify_one(): Wakes up one thread that is waiting on the condition variable.
            notify_all(): Wakes up all threads that are waiting on the condition variable.

        Example 1: Basic Usage of std::condition_variable
            In this example, one thread will produce data, and another will consume it. 
            The consumer waits until the producer signals that data is available.

            #include <iostream>
            #include <thread>
            #include <mutex>
            #include <condition_variable>
            #include <queue>

            std::mutex mtx;
            std::condition_variable cv;
            std::queue<int> data_queue;
            bool ready = false;

            void producer() {
                std::this_thread::sleep_for(std::chrono::seconds(2)); // Simulate work
                std::lock_guard<std::mutex> lock(mtx);
                data_queue.push(42);  // Produce data
                ready = true;         // Set condition to true
                cv.notify_one();      // Notify consumer
                std::cout << "Producer produced data\n";
            }

            void consumer() {
                std::cout << "Consumer waiting for data...\n";
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, []{ return ready; }); // Wait until 'ready' is true
                // At this point, data is available, so consume it
                int data = data_queue.front();
                data_queue.pop();
                ready = false;  // Reset the condition
                std::cout << "Consumer consumed data: " << data << "\n";
            }

            int main() {
                std::thread t1(producer);
                std::thread t2(consumer);

                t1.join();
                t2.join();

                return 0;
            }
            Explanation:
            Producer Thread: The producer sleeps for a few seconds (simulating work), then produces some data and sets the ready flag to true.
            Consumer Thread: The consumer waits until the ready flag becomes true. When it does, the consumer processes the data.
            Here, cv.wait(lock, []{ return ready; }) blocks the consumer until ready is set to true, signaling that the condition has been met.

        Example 2: wait_for with Timeout
            In this example, the consumer will wait for a maximum of 5 seconds before checking the condition again. If the condition is not met within that time, it will print a timeout message.

            #include <iostream>
            #include <thread>
            #include <mutex>
            #include <condition_variable>

            std::mutex mtx;
            std::condition_variable cv;
            bool ready = false;

            void producer() {
                std::this_thread::sleep_for(std::chrono::seconds(3)); // Simulate work
                std::lock_guard<std::mutex> lock(mtx);
                ready = true;  // Set condition to true
                cv.notify_one();  // Notify consumer
                std::cout << "Producer produced data\n";
            }

            void consumer() {
                std::cout << "Consumer waiting for data...\n";
                std::unique_lock<std::mutex> lock(mtx);
                
                // Wait for a maximum of 5 seconds
                if(cv.wait_for(lock, std::chrono::seconds(5), []{ return ready; })) {
                    std::cout << "Consumer received data\n";
                } else {
                    std::cout << "Consumer timed out waiting for data\n";
                }
            }

            int main() {
                std::thread t1(producer);
                std::thread t2(consumer);

                t1.join();
                t2.join();

                return 0;
            }
            Explanation:
            The consumer waits for up to 5 seconds using cv.wait_for. If the condition becomes true within 5 seconds, it processes the data. If not, it reports a timeout.

        Example 3: notify_all to Wake Up Multiple Threads
            In this example, multiple consumer threads wait for the same condition, and the producer will notify all waiting consumers when data becomes available.

            #include <iostream>
            #include <thread>
            #include <mutex>
            #include <condition_variable>
            #include <vector>

            std::mutex mtx;
            std::condition_variable cv;
            bool ready = false;

            void consumer(int id) {
                std::cout << "Consumer " << id << " waiting for data...\n";
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, []{ return ready; });  // Wait until the condition is true
                std::cout << "Consumer " << id << " received data\n";
            }

            void producer() {
                std::this_thread::sleep_for(std::chrono::seconds(2)); // Simulate work
                std::lock_guard<std::mutex> lock(mtx);
                ready = true;  // Set condition to true
                cv.notify_all();  // Notify all consumers
                std::cout << "Producer produced data and notified all consumers\n";
            }

            int main() {
                std::vector<std::thread> consumers;
                
                // Create multiple consumer threads
                for(int i = 0; i < 3; ++i) {
                    consumers.emplace_back(consumer, i);
                }
                
                // Create producer thread
                std::thread t1(producer);
                
                // Wait for all threads to finish
                for(auto& t : consumers) {
                    t.join();
                }
                
                t1.join();

                return 0;
            }
            Explanation:
            Multiple Consumers: Here, three consumer threads are created. Each one waits for data to be produced.
            Producer: Once the producer produces data, it calls cv.notify_all() to wake up all waiting consumers, which then print out that they received the data.

        Example 4: notify_one for Producer-Consumer Pattern
            In this example, the producer produces data, and only one consumer is woken up at a time.

            #include <iostream>
            #include <thread>
            #include <mutex>
            #include <condition_variable>

            std::mutex mtx;
            std::condition_variable cv;
            int data = 0;
            bool ready = false;

            void consumer(int id) {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, []{ return ready; });  // Wait until ready becomes true
                std::cout << "Consumer " << id << " is consuming data: " << data << "\n";
                ready = false;  // Reset condition
                cv.notify_one();  // Notify next consumer (if any)
            }

            void producer() {
                std::this_thread::sleep_for(std::chrono::seconds(1));  // Simulate some work
                std::lock_guard<std::mutex> lock(mtx);
                data = 42;  // Set some data
                ready = true;  // Mark that data is ready
                cv.notify_one();  // Notify one consumer
                std::cout << "Producer produced data\n";
            }

            int main() {
                std::thread t1(producer);
                std::thread t2(consumer, 1);
                std::thread t3(consumer, 2);

                t1.join();
                t2.join();
                t3.join();

                return 0;
            }
            Explanation:
            Single Consumer Notification: In this case, cv.notify_one() wakes up only one consumer thread to process the data at a time. After one consumer consumes the data, it notifies the next waiting consumer.


    ///////////////////
*/

```
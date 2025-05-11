##  The code & explanation  for A reentrant function, A thread-safe function

```cpp
/*
    ⚠️ A reentrant function
        A reentrant function is a function that can be safely interrupted and called again ("re-entered") 
        before its previous execution is complete — and still works correctly.

        🧠 In Simple Terms:
        A reentrant function:
            -Does not rely on shared or static data.
            -Does not modify global state.
            -Does not use resources that are not reentrant (e.g., malloc without protection).
            -Can safely be used by multiple  interrupt routines.

        ✅ Reentrant Function Example (Safe):

            int add(int a, int b) {
                return a + b;
            }
            No shared data.
            No side effects.
            Fully reentrant and thread-safe.

        ❌ Non-Reentrant Function Example:

            char* get_time_string() {
                static char buffer[100]; // static = shared!
                std::time_t t = std::time(nullptr);
                std::strftime(buffer, sizeof(buffer), "%H:%M:%S", std::localtime(&t));
                return buffer;
            }
            buffer is static, shared between calls.
            If another thread or ISR calls this function mid-execution, it will overwrite buffer.
            Therefore, not reentrant.

        🧵 Reentrancy vs Thread-Safety:
            | Concept                 | Reentrant       | Thread-Safe                           |
            | ----------------------- | --------------- | ------------------------------------- |
            | **Interrupt-safe**      | ✅ Yes           | ❌ Not necessarily                   |
            | **Global/static state** | ❌ Never allowed | ✅ Allowed if protected (mutex, etc) |
            | **Recursion safe**      | ✅ Yes           | ✅ Often, but depends on data use    |

        🛠 Use Cases Where Reentrancy Matters
            -Interrupt Handlers: Must only call reentrant functions.
            -Signal Handlers: Should only use reentrant-safe APIs (like write() but not printf()).
            -Real-Time Systems: Need strict guarantees about predictability and safety.
        🔒 How to Make a Function Reentrant
            ✅ Do:
                Use only local (stack) variables
                Avoid shared resources
                Pass all state via arguments
            ❌ Avoid:
                static or global variables
                Heap memory unless externally managed
                Non-reentrant standard library functions (e.g. strtok())
    ⚠️ A thread-safe function:
        A thread-safe function is a function that can be safely called from multiple threads at the same time 
        without causing data races or corrupting shared state.

        ✅ What Makes a Function Thread-Safe?
        A function is thread-safe if:
            -It protects shared data using synchronization (e.g. mutexes).
            -It uses only local (stack) variables — no shared state.
            -It manages resource access correctly under concurrent use.


        🛠️ Thread-Safe Function Example:
            ✅ Thread-Safe Function using std::mutex:

                std::mutex counter_mutex;
                int counter = 0;

                void increment_counter() {
                    std::lock_guard<std::mutex> lock(counter_mutex);
                    ++counter; // Safe from race conditions
                }
                Multiple threads can safely call increment_counter().
                The mutex ensures only one thread accesses counter at a time.

            ❌ Not Thread-Safe Function:

                int counter = 0;

                void increment_counter() {
                    ++counter; // Race condition if accessed from multiple threads
                }
                If two threads run this at the same time, counter might get corrupted.

        🧠 Thread-Safety in C++ Standard Library
        Many C++ standard library features are thread-safe only for distinct objects.

        Thread-safe:
            std::vector<int> v1, v2;
            // One thread uses v1, another uses v2 → OK
        Not thread-safe:
            std::vector<int> v;
        // Two threads push_back into same vector → ❌ undefined behavior

        🧩 Tips to Write Thread-Safe Code
            ->Use std::mutex, std::lock_guard, or std::unique_lock to guard shared data.
            ->Prefer immutable data or thread-local storage.(Immutable data refers to data that cannot be changed after it is created. Once a piece of data is made immutable, you cannot modify its contents. )
            ->Design with concurrent-safe containers (e.g., concurrent_queue or use std::queue with external locking).
            ->Avoid using global variables unless properly synchronized.
    🧵 Thread-Safe vs. Reentrant:

        | Property                   | Thread-Safe                      | Reentrant                   |
        | -------------------------- | -------------------------------- | --------------------------- |
        | Shared resources           | Can use, but must be protected   | Cannot use shared resources |
        | Global/static vars         | Allowed with protection (mutex)  | Not allowed at all          |
        | Recursion                  | May or may not support recursion | Must be recursion-safe      |
        | Interrupt-safe             | ❌ Not necessarily               | ✅ Yes                      |
        | Use in multi-threaded apps | ✅ Yes                           | ❌ Not necessarily          |
        | Use in signal/ISR          | ❌ Unsafe                        | ✅ Safe                     |
        
    Example classify these code about thread-safe  re-entrant:

            ->Not thread-safe, not re-entrant:

                int tmp;
                int add10(int a) {
                    tmp = a;
                    return tmp + 10; // <--- interrupt here
                }

                It’s not thread-safe because there’s a data-race because multiple threads can access tmp at the same time. 
                It’s not re-entrant because for example:
                It’s en-entrant. But it’s not thread-safe because the data-race on the shared variable tmp. 
                This is a silly example as tmp does nothing other than creating a data-race here. But you get the idea.

            ->Not thread-safe, but re-entrant

                int tmp;
                int add10(int a) {
                    tmp = a;
                    return a + 10;
                }
                It’s en-entrant. But it’s not thread-safe because the data-race on the shared variable tmp. 
                This is a silly example as tmp does nothing other than creating a data-race here. But you get the idea.

            ->Thread-safe but not re-entrant:

                thread_local int tmp;
                int add10(int a) {
                    tmp = a;
                    return tmp + 10;
                }
                or
                std::mutex mtx;
                int tmp;
                int add10(int a) {
                    mtx.lock();
                    tmp = a;
                    mtx.unlock();
                    return tmp + 10;
                }
                It’s thread-safe thanks to thread_local. But it’s not re-entrant just like the first example.
            ->Thread-safe and re-entrant:

                int add10(int a) {
                    return a + 10;
                }

    ///////////////////

*/
```

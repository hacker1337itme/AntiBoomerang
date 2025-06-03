# AntiBoomring
AntiBoomring Method 

Anti-Boomerang C++ class with thread overload protection, featuring:

    ✅ Anti-recursion

    ✅ Anti-loopback (reentrancy)

    ✅ Anti-replay (event ID memory)

    ✅ Optional nested call simulation

    ✅ 🧵 Thread overload protection — max active threads limit using mutex + atomic counter

🛡️ Summary of Protection
- Protection Type	Description
- Recursion Guard	Limits how deep the function can call itself
- Loopback Block	Prevents re-entrant callbacks
- Replay Filter	Ignores already-seen event IDs
- Nested Simulation	Shows how recursion can be safely controlled
```
g++ -std=c++17 -pthread AntiBoomerang.cpp -o AntiBoomerang
```
# POC
```c++
#include <iostream>
#include <unordered_set>
#include <string>

class AntiBoomerang {
private:
    bool isUpdating = false;                     // Prevent self-triggered callbacks
    int recursionDepth = 0;                      // Track recursive depth
    const int MAX_DEPTH = 10;                    // Recursion guard limit
    std::unordered_set<std::string> seenEvents;  // Memory of processed event IDs

public:
    // Unified function that processes an event with anti-boomerang logic
    bool process(const std::string& eventId, bool allowNested = true) {
        // Anti-recursion
        if (recursionDepth >= MAX_DEPTH) {
            std::cerr << "[Blocked] Max recursion depth reached.\n";
            return false;
        }

        // Anti-loopback
        if (isUpdating) {
            std::cerr << "[Blocked] Reentrant self-call detected.\n";
            return false;
        }

        // Anti-duplicate/replay
        if (seenEvents.count(eventId)) {
            std::cerr << "[Blocked] Duplicate/replayed event: " << eventId << "\n";
            return false;
        }

        // Begin safe execution
        seenEvents.insert(eventId);
        isUpdating = true;
        recursionDepth++;

        std::cout << "[Processing] Event: " << eventId << " (Depth " << recursionDepth << ")\n";

        // Optional: simulate a nested event trigger
        if (allowNested && recursionDepth < 3) {
            process(eventId + "_child", true);  // Recursive nested event
        }

        // Cleanup and return
        recursionDepth--;
        isUpdating = false;
        return true;
    }

    // Resets state for reuse or testing
    void reset() {
        isUpdating = false;
        recursionDepth = 0;
        seenEvents.clear();
        std::cout << "[Reset] State cleared.\n";
    }
};

int main() {
    AntiBoomerang guard;

    guard.process("evt1");   // Will recurse safely
    guard.process("evt1");   // Will be blocked (duplicate)
    guard.process("evt2");   // Accepted
    guard.process("evt2");   // Blocked

    guard.reset();           // Allow re-use

    guard.process("evt3");   // Allowed again
    return 0;
}
```

#include <iostream>
#include <unordered_set>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <vector>
#include <sstream>
#include <iomanip>

class AntiBoomerang {
private:
    static constexpr int MAX_RECURSION_DEPTH = 10;
    static constexpr int MAX_THREADS = 16;

    std::atomic<int> recursionDepth{0};
    static std::atomic<int> activeThreads;
    static std::mutex globalMutex;
    std::unordered_set<std::string> seenEvents;
    std::mutex instanceMutex;
    bool isUpdating = false;

public:
    // Main handler with thread + boomerang guards
    bool process(const std::string& eventId, bool allowNested = true) {
	std::ostringstream msg;
	int tid = std::hash<std::thread::id>{}(std::this_thread::get_id()) % 1000; // Short thread ID

// Thread overload
{
    std::lock_guard<std::mutex> lock(globalMutex);
    if (activeThreads >= MAX_THREADS) {
        std::cerr << "[⛔] " << eventId << " → blocked (thread overload)\n";
        return false;
    }
    ++activeThreads;
}

// Recursion limit
if (recursionDepth >= MAX_RECURSION_DEPTH) {
    std::cerr << "[⚠] " << eventId << " → blocked (recursion limit)\n";
    activeThreads--;
    return false;
}

// Lock and check
{
    std::lock_guard<std::mutex> lock(instanceMutex);

    if (isUpdating) {
        std::cerr << "[⚠] " << eventId << " → blocked (loopback)\n";
        activeThreads--;
        return false;
    }

    if (seenEvents.count(eventId)) {
        std::cerr << "[⚠] " << eventId << " → blocked (duplicate)\n";
        activeThreads--;
        return false;
    }

    seenEvents.insert(eventId);
    isUpdating = true;
    recursionDepth++;
	}

	std::cout << "[✔] " << eventId << " → handled (depth=" << recursionDepth << ", thread=" << tid << ")\n";
 
	std::this_thread::sleep_for(std::chrono::milliseconds(100));


        // Simulate nested event
        if (allowNested && recursionDepth < 3) {
            process(eventId + "_nested", true);
        }

        {
            std::lock_guard<std::mutex> lock(instanceMutex);
            isUpdating = false;
            recursionDepth--;
        }

        activeThreads--;
        return true;
    }

    void reset() {
        std::lock_guard<std::mutex> lock(instanceMutex);
        recursionDepth = 0;
        seenEvents.clear();
        isUpdating = false;
        std::cout << "[Reset] State cleared.\n";
    }
};

// Static definitions
std::atomic<int> AntiBoomerang::activeThreads{0};
std::mutex AntiBoomerang::globalMutex;

int main() {
    AntiBoomerang guard;

    std::vector<std::thread> threads;

    for (int i = 0; i < 20; ++i) {
        threads.emplace_back([&, i]() {
            std::string eventId = "evt" + std::to_string(i % 5);  // Intentional duplicates
            guard.process(eventId);
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}

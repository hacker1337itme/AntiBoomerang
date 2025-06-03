# AntiBoomring
AntiBoomring Method 

Anti-Boomerang C++ class with thread overload protection, featuring:

    ✅ Anti-recursion

    ✅ Anti-loopback (reentrancy)

    ✅ Anti-replay (event ID memory)

    ✅ Optional nested call simulation

    ✅ 🧵 Thread overload protection — max active threads limit using mutex + atomic counter

🛡️ Summary of Protection
Protection Type	Description
Recursion Guard	Limits how deep the function can call itself
Loopback Block	Prevents re-entrant callbacks
Replay Filter	Ignores already-seen event IDs
Nested Simulation	Shows how recursion can be safely controlled




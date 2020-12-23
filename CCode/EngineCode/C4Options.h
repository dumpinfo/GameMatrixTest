 

// C4 Engine compilation option switches
// ========================================

// C4SIMD controls whether intrinsic functions are used to explicit access vector
// instructions on the CPU. This should ordinarily be left enabled for best performance.

#define C4SIMD						1

// C4LEGACY controls whether support for earlier resource versions is compiled into
// the engine. This can be disabled to reduce code size and unpacking logic in the case
// that it is known that all resources are updated for the current version of the engine.

#define C4LEGACY					1

// C4STATS controls whether statistics counters are compiled into the engine. This can
// be disabled to remove the counter logic, possibly providing a slight performance
// benefit. If enabled, the statistics can be monitored using the "stat" console command.

#define C4STATS						1

// C4DIAGS controls whether various debug diagnostic rendering capabilities are compiled
// into the engine.

#define C4DIAGS						1

// C4LOG_FILE controls whether the engine writes a log file. If enabled, a file named
// "C4Log.html" is written to the user's local application data folder.

#define C4LOG_FILE					1

// C4DEBUG_MEMORY controls whether the debug memory features of the engine are enabled.
// This is ordinarily set to be enabled for debug builds and disabled for everything else.

#define C4DEBUG_MEMORY				C4DEBUG

// C4LEAK_DETECTION controls whether the memory leak detection feature is enabled.
// If enabled, a file named "Leaks.txt" is written when the engine exits, and it contains
// information about any unreleased memory blocks.

#define C4LEAK_DETECTION			0

// C4LOG_RESOURCES controls whether resources are logged when they are loaded from disk.
// If enabled, a file named "Resources.txt" is written as the engine runs, and it contains
// the names of all resources that have been loaded.

#define C4LOG_RESOURCES				0

// ZYUQURM

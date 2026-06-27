CSOPESY MO1 - OSQTS Command Line Emulator
===========================================

Group members:
- Kean Carvin
- Gideon Chua
- Daniel Pua
- Bea Uy

Entry point:
- The main() function is located in src/main.cpp.
- There is no separate "entry class" — this is a C++ console application,
  so main.cpp's main() is the program's entry point. It constructs the CLI
  console (src/cli/Console.cpp) which drives the rest of the emulator.

Requirements:
- Visual Studio 2022 with the "Desktop development with C++" workload
  (this provides MSVC + CMake), OR VS Build Tools + CMake installed separately.

How to build and run:
1. Clone the repository:
       git clone <repo-url>
       cd MO1-Opesy
2. Configure the build:
       cmake -S . -B out/build/x64-debug
3. Build the project:
       cmake --build out/build/x64-debug
4. Run the produced executable from out/build/x64-debug (e.g. the .exe
   matching the project name), or run it directly from Visual Studio by
   opening the folder/CMakeLists.txt and pressing Run/Debug.

Usage notes:
- On launch you'll be dropped into the command line shell. Use the
  "initialize" command first to load scheduler settings from config.txt.
- Use "scheduler-start" / "scheduler-stop" to control the test process
  generator, "screen -ls" to list processes, and "screen -r <name>" to
  attach to a running process's screen.
- Type "exit" to quit the emulator.

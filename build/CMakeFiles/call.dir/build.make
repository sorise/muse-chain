# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/remix/progress/clion-2022.3.3/bin/cmake/linux/x64/bin/cmake

# The command to remove a file.
RM = /home/remix/progress/clion-2022.3.3/bin/cmake/linux/x64/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/remix/coding/GIthub/muse-chain

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/remix/coding/GIthub/muse-chain/build

# Include any dependencies generated for this target.
include CMakeFiles/call.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/call.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/call.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/call.dir/flags.make

CMakeFiles/call.dir/bind.cpp.o: CMakeFiles/call.dir/flags.make
CMakeFiles/call.dir/bind.cpp.o: /home/remix/coding/GIthub/muse-chain/bind.cpp
CMakeFiles/call.dir/bind.cpp.o: CMakeFiles/call.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/remix/coding/GIthub/muse-chain/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/call.dir/bind.cpp.o"
	/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/call.dir/bind.cpp.o -MF CMakeFiles/call.dir/bind.cpp.o.d -o CMakeFiles/call.dir/bind.cpp.o -c /home/remix/coding/GIthub/muse-chain/bind.cpp

CMakeFiles/call.dir/bind.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/call.dir/bind.cpp.i"
	/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/remix/coding/GIthub/muse-chain/bind.cpp > CMakeFiles/call.dir/bind.cpp.i

CMakeFiles/call.dir/bind.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/call.dir/bind.cpp.s"
	/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/remix/coding/GIthub/muse-chain/bind.cpp -o CMakeFiles/call.dir/bind.cpp.s

# Object files for target call
call_OBJECTS = \
"CMakeFiles/call.dir/bind.cpp.o"

# External object files for target call
call_EXTERNAL_OBJECTS =

call: CMakeFiles/call.dir/bind.cpp.o
call: CMakeFiles/call.dir/build.make
call: librpc.so
call: libserializer.so
call: libmuseThreads.so
call: libtimer.so
call: CMakeFiles/call.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/remix/coding/GIthub/muse-chain/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable call"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/call.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/call.dir/build: call
.PHONY : CMakeFiles/call.dir/build

CMakeFiles/call.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/call.dir/cmake_clean.cmake
.PHONY : CMakeFiles/call.dir/clean

CMakeFiles/call.dir/depend:
	cd /home/remix/coding/GIthub/muse-chain/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/remix/coding/GIthub/muse-chain /home/remix/coding/GIthub/muse-chain /home/remix/coding/GIthub/muse-chain/build /home/remix/coding/GIthub/muse-chain/build /home/remix/coding/GIthub/muse-chain/build/CMakeFiles/call.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/call.dir/depend

# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.23

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
CMAKE_COMMAND = /Applications/CMake.app/Contents/bin/cmake

# The command to remove a file.
RM = /Applications/CMake.app/Contents/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/ray/Code/compiler

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/ray/Code/compiler/build

# Include any dependencies generated for this target.
include test/CMakeFiles/test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include test/CMakeFiles/test.dir/compiler_depend.make

# Include the progress variables for this target.
include test/CMakeFiles/test.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/test.dir/flags.make

test/CMakeFiles/test.dir/test.c.o: test/CMakeFiles/test.dir/flags.make
test/CMakeFiles/test.dir/test.c.o: ../test/test.c
test/CMakeFiles/test.dir/test.c.o: test/CMakeFiles/test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ray/Code/compiler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object test/CMakeFiles/test.dir/test.c.o"
	cd /Users/ray/Code/compiler/build/test && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT test/CMakeFiles/test.dir/test.c.o -MF CMakeFiles/test.dir/test.c.o.d -o CMakeFiles/test.dir/test.c.o -c /Users/ray/Code/compiler/test/test.c

test/CMakeFiles/test.dir/test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/test.dir/test.c.i"
	cd /Users/ray/Code/compiler/build/test && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/ray/Code/compiler/test/test.c > CMakeFiles/test.dir/test.c.i

test/CMakeFiles/test.dir/test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/test.dir/test.c.s"
	cd /Users/ray/Code/compiler/build/test && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/ray/Code/compiler/test/test.c -o CMakeFiles/test.dir/test.c.s

# Object files for target test
test_OBJECTS = \
"CMakeFiles/test.dir/test.c.o"

# External object files for target test
test_EXTERNAL_OBJECTS =

test/test: test/CMakeFiles/test.dir/test.c.o
test/test: test/CMakeFiles/test.dir/build.make
test/test: test/CMakeFiles/test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/ray/Code/compiler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable test"
	cd /Users/ray/Code/compiler/build/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/test.dir/build: test/test
.PHONY : test/CMakeFiles/test.dir/build

test/CMakeFiles/test.dir/clean:
	cd /Users/ray/Code/compiler/build/test && $(CMAKE_COMMAND) -P CMakeFiles/test.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/test.dir/clean

test/CMakeFiles/test.dir/depend:
	cd /Users/ray/Code/compiler/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/ray/Code/compiler /Users/ray/Code/compiler/test /Users/ray/Code/compiler/build /Users/ray/Code/compiler/build/test /Users/ray/Code/compiler/build/test/CMakeFiles/test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/test.dir/depend


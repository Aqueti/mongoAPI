# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/cameron/Documents/aware/dispcode_mongoapi

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/cameron/Documents/aware/dispcode_mongoapi/build

# Include any dependencies generated for this target.
include CMakeFiles/MongoInterfaceTest.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/MongoInterfaceTest.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/MongoInterfaceTest.dir/flags.make

CMakeFiles/MongoInterfaceTest.dir/MongoInterface_test.cpp.o: CMakeFiles/MongoInterfaceTest.dir/flags.make
CMakeFiles/MongoInterfaceTest.dir/MongoInterface_test.cpp.o: ../MongoInterface_test.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/cameron/Documents/aware/dispcode_mongoapi/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/MongoInterfaceTest.dir/MongoInterface_test.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/MongoInterfaceTest.dir/MongoInterface_test.cpp.o -c /home/cameron/Documents/aware/dispcode_mongoapi/MongoInterface_test.cpp

CMakeFiles/MongoInterfaceTest.dir/MongoInterface_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MongoInterfaceTest.dir/MongoInterface_test.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/cameron/Documents/aware/dispcode_mongoapi/MongoInterface_test.cpp > CMakeFiles/MongoInterfaceTest.dir/MongoInterface_test.cpp.i

CMakeFiles/MongoInterfaceTest.dir/MongoInterface_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MongoInterfaceTest.dir/MongoInterface_test.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/cameron/Documents/aware/dispcode_mongoapi/MongoInterface_test.cpp -o CMakeFiles/MongoInterfaceTest.dir/MongoInterface_test.cpp.s

CMakeFiles/MongoInterfaceTest.dir/MongoInterface_test.cpp.o.requires:
.PHONY : CMakeFiles/MongoInterfaceTest.dir/MongoInterface_test.cpp.o.requires

CMakeFiles/MongoInterfaceTest.dir/MongoInterface_test.cpp.o.provides: CMakeFiles/MongoInterfaceTest.dir/MongoInterface_test.cpp.o.requires
	$(MAKE) -f CMakeFiles/MongoInterfaceTest.dir/build.make CMakeFiles/MongoInterfaceTest.dir/MongoInterface_test.cpp.o.provides.build
.PHONY : CMakeFiles/MongoInterfaceTest.dir/MongoInterface_test.cpp.o.provides

CMakeFiles/MongoInterfaceTest.dir/MongoInterface_test.cpp.o.provides.build: CMakeFiles/MongoInterfaceTest.dir/MongoInterface_test.cpp.o

# Object files for target MongoInterfaceTest
MongoInterfaceTest_OBJECTS = \
"CMakeFiles/MongoInterfaceTest.dir/MongoInterface_test.cpp.o"

# External object files for target MongoInterfaceTest
MongoInterfaceTest_EXTERNAL_OBJECTS =

MongoInterfaceTest: CMakeFiles/MongoInterfaceTest.dir/MongoInterface_test.cpp.o
MongoInterfaceTest: CMakeFiles/MongoInterfaceTest.dir/build.make
MongoInterfaceTest: libMongoInterface.a
MongoInterfaceTest: /usr/local/lib/libJsonBox.a
MongoInterfaceTest: /usr/local/lib/libmongoclient.a
MongoInterfaceTest: /usr/lib/x86_64-linux-gnu/libboost_system.so
MongoInterfaceTest: /usr/lib/x86_64-linux-gnu/libboost_regex.so
MongoInterfaceTest: /usr/lib/x86_64-linux-gnu/libboost_thread.so
MongoInterfaceTest: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so
MongoInterfaceTest: /usr/lib/x86_64-linux-gnu/libboost_program_options.so
MongoInterfaceTest: /usr/lib/x86_64-linux-gnu/libpthread.so
MongoInterfaceTest: CMakeFiles/MongoInterfaceTest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable MongoInterfaceTest"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MongoInterfaceTest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/MongoInterfaceTest.dir/build: MongoInterfaceTest
.PHONY : CMakeFiles/MongoInterfaceTest.dir/build

CMakeFiles/MongoInterfaceTest.dir/requires: CMakeFiles/MongoInterfaceTest.dir/MongoInterface_test.cpp.o.requires
.PHONY : CMakeFiles/MongoInterfaceTest.dir/requires

CMakeFiles/MongoInterfaceTest.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/MongoInterfaceTest.dir/cmake_clean.cmake
.PHONY : CMakeFiles/MongoInterfaceTest.dir/clean

CMakeFiles/MongoInterfaceTest.dir/depend:
	cd /home/cameron/Documents/aware/dispcode_mongoapi/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/cameron/Documents/aware/dispcode_mongoapi /home/cameron/Documents/aware/dispcode_mongoapi /home/cameron/Documents/aware/dispcode_mongoapi/build /home/cameron/Documents/aware/dispcode_mongoapi/build /home/cameron/Documents/aware/dispcode_mongoapi/build/CMakeFiles/MongoInterfaceTest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/MongoInterfaceTest.dir/depend

# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ydkj/plc_test

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ydkj/plc_test/build

# Include any dependencies generated for this target.
include CMakeFiles/plctest.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/plctest.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/plctest.dir/flags.make

CMakeFiles/plctest.dir/src/plc/snap7.cpp.o: CMakeFiles/plctest.dir/flags.make
CMakeFiles/plctest.dir/src/plc/snap7.cpp.o: ../src/plc/snap7.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ydkj/plc_test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/plctest.dir/src/plc/snap7.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/plctest.dir/src/plc/snap7.cpp.o -c /home/ydkj/plc_test/src/plc/snap7.cpp

CMakeFiles/plctest.dir/src/plc/snap7.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/plctest.dir/src/plc/snap7.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ydkj/plc_test/src/plc/snap7.cpp > CMakeFiles/plctest.dir/src/plc/snap7.cpp.i

CMakeFiles/plctest.dir/src/plc/snap7.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/plctest.dir/src/plc/snap7.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ydkj/plc_test/src/plc/snap7.cpp -o CMakeFiles/plctest.dir/src/plc/snap7.cpp.s

CMakeFiles/plctest.dir/src/plc/plc_reader.cpp.o: CMakeFiles/plctest.dir/flags.make
CMakeFiles/plctest.dir/src/plc/plc_reader.cpp.o: ../src/plc/plc_reader.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ydkj/plc_test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/plctest.dir/src/plc/plc_reader.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/plctest.dir/src/plc/plc_reader.cpp.o -c /home/ydkj/plc_test/src/plc/plc_reader.cpp

CMakeFiles/plctest.dir/src/plc/plc_reader.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/plctest.dir/src/plc/plc_reader.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ydkj/plc_test/src/plc/plc_reader.cpp > CMakeFiles/plctest.dir/src/plc/plc_reader.cpp.i

CMakeFiles/plctest.dir/src/plc/plc_reader.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/plctest.dir/src/plc/plc_reader.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ydkj/plc_test/src/plc/plc_reader.cpp -o CMakeFiles/plctest.dir/src/plc/plc_reader.cpp.s

# Object files for target plctest
plctest_OBJECTS = \
"CMakeFiles/plctest.dir/src/plc/snap7.cpp.o" \
"CMakeFiles/plctest.dir/src/plc/plc_reader.cpp.o"

# External object files for target plctest
plctest_EXTERNAL_OBJECTS =

libplctest.a: CMakeFiles/plctest.dir/src/plc/snap7.cpp.o
libplctest.a: CMakeFiles/plctest.dir/src/plc/plc_reader.cpp.o
libplctest.a: CMakeFiles/plctest.dir/build.make
libplctest.a: CMakeFiles/plctest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ydkj/plc_test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libplctest.a"
	$(CMAKE_COMMAND) -P CMakeFiles/plctest.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/plctest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/plctest.dir/build: libplctest.a

.PHONY : CMakeFiles/plctest.dir/build

CMakeFiles/plctest.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/plctest.dir/cmake_clean.cmake
.PHONY : CMakeFiles/plctest.dir/clean

CMakeFiles/plctest.dir/depend:
	cd /home/ydkj/plc_test/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ydkj/plc_test /home/ydkj/plc_test /home/ydkj/plc_test/build /home/ydkj/plc_test/build /home/ydkj/plc_test/build/CMakeFiles/plctest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/plctest.dir/depend


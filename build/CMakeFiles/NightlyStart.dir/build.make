# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.29

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
CMAKE_COMMAND = /home/proger/.local/lib/python3.12/site-packages/cmake/data/bin/cmake

# The command to remove a file.
RM = /home/proger/.local/lib/python3.12/site-packages/cmake/data/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/9E4668B146688BB9/CProjects/voxel-engine

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/9E4668B146688BB9/CProjects/voxel-engine/build

# Utility rule file for NightlyStart.

# Include any custom commands dependencies for this target.
include CMakeFiles/NightlyStart.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/NightlyStart.dir/progress.make

CMakeFiles/NightlyStart:
	/home/proger/.local/lib/python3.12/site-packages/cmake/data/bin/ctest -D NightlyStart

NightlyStart: CMakeFiles/NightlyStart
NightlyStart: CMakeFiles/NightlyStart.dir/build.make
.PHONY : NightlyStart

# Rule to build all files generated by this target.
CMakeFiles/NightlyStart.dir/build: NightlyStart
.PHONY : CMakeFiles/NightlyStart.dir/build

CMakeFiles/NightlyStart.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/NightlyStart.dir/cmake_clean.cmake
.PHONY : CMakeFiles/NightlyStart.dir/clean

CMakeFiles/NightlyStart.dir/depend:
	cd /mnt/9E4668B146688BB9/CProjects/voxel-engine/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/9E4668B146688BB9/CProjects/voxel-engine /mnt/9E4668B146688BB9/CProjects/voxel-engine /mnt/9E4668B146688BB9/CProjects/voxel-engine/build /mnt/9E4668B146688BB9/CProjects/voxel-engine/build /mnt/9E4668B146688BB9/CProjects/voxel-engine/build/CMakeFiles/NightlyStart.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/NightlyStart.dir/depend


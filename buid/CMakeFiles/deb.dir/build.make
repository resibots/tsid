# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

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


# Produce verbose output by default.
VERBOSE = 1

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/user/tsid

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/user/tsid/buid

# Utility rule file for deb.

# Include the progress variables for this target.
include CMakeFiles/deb.dir/progress.make

CMakeFiles/deb:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/user/tsid/buid/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating Debian package..."
	cd /home/user/tsid && git-buildpackage --git-debian-branch=debian --git-builder="debuild\ -S\ -i.git\ -I.git"

deb: CMakeFiles/deb
deb: CMakeFiles/deb.dir/build.make

.PHONY : deb

# Rule to build all files generated by this target.
CMakeFiles/deb.dir/build: deb

.PHONY : CMakeFiles/deb.dir/build

CMakeFiles/deb.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/deb.dir/cmake_clean.cmake
.PHONY : CMakeFiles/deb.dir/clean

CMakeFiles/deb.dir/depend:
	cd /home/user/tsid/buid && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/user/tsid /home/user/tsid /home/user/tsid/buid /home/user/tsid/buid /home/user/tsid/buid/CMakeFiles/deb.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/deb.dir/depend

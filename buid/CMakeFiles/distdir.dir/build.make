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

# Utility rule file for distdir.

# Include the progress variables for this target.
include CMakeFiles/distdir.dir/progress.make

CMakeFiles/distdir:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/user/tsid/buid/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating dist directory..."
	cd /home/user/tsid && rm -f /tmp/tsid.tar && /home/user/tsid/cmake/git-archive-all.sh --prefix tsid-1.4.1/ tsid.tar && cd /home/user/tsid/buid/ && ( test -d tsid-1.4.1 && find tsid-1.4.1/ -type d -print0 | xargs -0 chmod a+w || true ) && rm -rf tsid-1.4.1/ && /bin/tar xf /home/user/tsid/tsid.tar && echo 1.4.1 > /home/user/tsid/buid/tsid-1.4.1/.version && /home/user/tsid/cmake/gitlog-to-changelog > /home/user/tsid/buid/tsid-1.4.1/ChangeLog && rm -f /home/user/tsid/tsid.tar

distdir: CMakeFiles/distdir
distdir: CMakeFiles/distdir.dir/build.make

.PHONY : distdir

# Rule to build all files generated by this target.
CMakeFiles/distdir.dir/build: distdir

.PHONY : CMakeFiles/distdir.dir/build

CMakeFiles/distdir.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/distdir.dir/cmake_clean.cmake
.PHONY : CMakeFiles/distdir.dir/clean

CMakeFiles/distdir.dir/depend:
	cd /home/user/tsid/buid && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/user/tsid /home/user/tsid /home/user/tsid/buid /home/user/tsid/buid /home/user/tsid/buid/CMakeFiles/distdir.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/distdir.dir/depend

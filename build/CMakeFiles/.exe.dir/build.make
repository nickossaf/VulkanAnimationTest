# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.21

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
CMAKE_COMMAND = /home/nicko/.local/lib/python3.6/site-packages/cmake/data/bin/cmake

# The command to remove a file.
RM = /home/nicko/.local/lib/python3.6/site-packages/cmake/data/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/c/Users/Dell/Desktop/programs/vmk/CG2/vulkan-ray/lin/RayTracing

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/c/Users/Dell/Desktop/programs/vmk/CG2/vulkan-ray/lin/RayTracing/build

# Include any dependencies generated for this target.
include CMakeFiles/.exe.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/.exe.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/.exe.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/.exe.dir/flags.make

CMakeFiles/.exe.dir/source/main.cpp.obj: CMakeFiles/.exe.dir/flags.make
CMakeFiles/.exe.dir/source/main.cpp.obj: CMakeFiles/.exe.dir/includes_CXX.rsp
CMakeFiles/.exe.dir/source/main.cpp.obj: ../source/main.cpp
CMakeFiles/.exe.dir/source/main.cpp.obj: CMakeFiles/.exe.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Users/Dell/Desktop/programs/vmk/CG2/vulkan-ray/lin/RayTracing/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/.exe.dir/source/main.cpp.obj"
	/usr/bin/x86_64-w64-mingw32-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/.exe.dir/source/main.cpp.obj -MF CMakeFiles/.exe.dir/source/main.cpp.obj.d -o CMakeFiles/.exe.dir/source/main.cpp.obj -c /mnt/c/Users/Dell/Desktop/programs/vmk/CG2/vulkan-ray/lin/RayTracing/source/main.cpp

CMakeFiles/.exe.dir/source/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/.exe.dir/source/main.cpp.i"
	/usr/bin/x86_64-w64-mingw32-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/c/Users/Dell/Desktop/programs/vmk/CG2/vulkan-ray/lin/RayTracing/source/main.cpp > CMakeFiles/.exe.dir/source/main.cpp.i

CMakeFiles/.exe.dir/source/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/.exe.dir/source/main.cpp.s"
	/usr/bin/x86_64-w64-mingw32-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/c/Users/Dell/Desktop/programs/vmk/CG2/vulkan-ray/lin/RayTracing/source/main.cpp -o CMakeFiles/.exe.dir/source/main.cpp.s

CMakeFiles/.exe.dir/source/VulkanApp.cpp.obj: CMakeFiles/.exe.dir/flags.make
CMakeFiles/.exe.dir/source/VulkanApp.cpp.obj: CMakeFiles/.exe.dir/includes_CXX.rsp
CMakeFiles/.exe.dir/source/VulkanApp.cpp.obj: ../source/VulkanApp.cpp
CMakeFiles/.exe.dir/source/VulkanApp.cpp.obj: CMakeFiles/.exe.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Users/Dell/Desktop/programs/vmk/CG2/vulkan-ray/lin/RayTracing/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/.exe.dir/source/VulkanApp.cpp.obj"
	/usr/bin/x86_64-w64-mingw32-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/.exe.dir/source/VulkanApp.cpp.obj -MF CMakeFiles/.exe.dir/source/VulkanApp.cpp.obj.d -o CMakeFiles/.exe.dir/source/VulkanApp.cpp.obj -c /mnt/c/Users/Dell/Desktop/programs/vmk/CG2/vulkan-ray/lin/RayTracing/source/VulkanApp.cpp

CMakeFiles/.exe.dir/source/VulkanApp.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/.exe.dir/source/VulkanApp.cpp.i"
	/usr/bin/x86_64-w64-mingw32-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/c/Users/Dell/Desktop/programs/vmk/CG2/vulkan-ray/lin/RayTracing/source/VulkanApp.cpp > CMakeFiles/.exe.dir/source/VulkanApp.cpp.i

CMakeFiles/.exe.dir/source/VulkanApp.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/.exe.dir/source/VulkanApp.cpp.s"
	/usr/bin/x86_64-w64-mingw32-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/c/Users/Dell/Desktop/programs/vmk/CG2/vulkan-ray/lin/RayTracing/source/VulkanApp.cpp -o CMakeFiles/.exe.dir/source/VulkanApp.cpp.s

# Object files for target .exe
_exe_OBJECTS = \
"CMakeFiles/.exe.dir/source/main.cpp.obj" \
"CMakeFiles/.exe.dir/source/VulkanApp.cpp.obj"

# External object files for target .exe
_exe_EXTERNAL_OBJECTS =

.exe.exe: CMakeFiles/.exe.dir/source/main.cpp.obj
.exe.exe: CMakeFiles/.exe.dir/source/VulkanApp.cpp.obj
.exe.exe: CMakeFiles/.exe.dir/build.make
.exe.exe: /mnt/c/WINDOWS/system32/vulkan-1.dll
.exe.exe: /mnt/c/WINDOWS/system32/vulkan-1.dll
.exe.exe: CMakeFiles/.exe.dir/linklibs.rsp
.exe.exe: CMakeFiles/.exe.dir/objects1.rsp
.exe.exe: CMakeFiles/.exe.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/c/Users/Dell/Desktop/programs/vmk/CG2/vulkan-ray/lin/RayTracing/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable .exe.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/.exe.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/.exe.dir/build: .exe.exe
.PHONY : CMakeFiles/.exe.dir/build

CMakeFiles/.exe.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/.exe.dir/cmake_clean.cmake
.PHONY : CMakeFiles/.exe.dir/clean

CMakeFiles/.exe.dir/depend:
	cd /mnt/c/Users/Dell/Desktop/programs/vmk/CG2/vulkan-ray/lin/RayTracing/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/c/Users/Dell/Desktop/programs/vmk/CG2/vulkan-ray/lin/RayTracing /mnt/c/Users/Dell/Desktop/programs/vmk/CG2/vulkan-ray/lin/RayTracing /mnt/c/Users/Dell/Desktop/programs/vmk/CG2/vulkan-ray/lin/RayTracing/build /mnt/c/Users/Dell/Desktop/programs/vmk/CG2/vulkan-ray/lin/RayTracing/build /mnt/c/Users/Dell/Desktop/programs/vmk/CG2/vulkan-ray/lin/RayTracing/build/CMakeFiles/.exe.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/.exe.dir/depend


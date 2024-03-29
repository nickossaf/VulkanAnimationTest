cmake_minimum_required (VERSION 3.8)
set(PROJECT_NAME ${BUILD_TYPE}.exe)

# to build on my computer cmake -DWIN32=on -DCMAKE_SOURCE_DIR=/mnt/c/WINDOWS/system32 ..

if(WIN32)
    set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)
    set(CMAKE_SYSTEM_NAME Windows)
    set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)  
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
    set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)
    set(CMAKE_RANLIB ${TOOLCHAIN_PREFIX}-ranlib)
    #set(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES /usr/lib/gcc/${TOOLCHAIN_PREFIX}/7.3-posix)
    #set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)
    #set(C_INCLUDE_PATH /usr/x86_64-w64-mingw32/include)
    #set(CPLUS_INCLUDE_PATH /usr/x86_64-w64-mingw32/include)
    #set(CPP_INCLUDE_PATH /usr/x86_64-w64-mingw32/include)
    #set(CXX_INCLUDE_PATH /usr/x86_64-w64-mingw32/include)
    set(CMAKE_FIND_ROOT_PATH  /usr/lib/gcc/${TOOLCHAIN_PREFIX}/7.3-win32/include/c++)
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER)
endif()


project(${PROJECT_NAME})

find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(Vulkan)
if(WIN32)
    set(Vulkan_INCLUDE_DIR /usr/include)
    set(Vulkan_LIBRARY /mnt/c/WINDOWS/system32/vulkan-1.dll)
    set(Vulkan_FOUND ON)


	#if(NOT Vulkan_FOUND)
	#	find_library(Vulkan_LIBRARY NAMES vulkan-1 vulkan PATHS ${CMAKE_SOURCE_DIR}/libs/vulkan)
	#	if(Vulkan_LIBRARY)
	#		set(Vulkan_FOUND ON)
	#		MESSAGE("Using bundled Vulkan library version")
    #   endif()
    #endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR")
endif()

if (NOT Vulkan_FOUND)
	message(FATAL_ERROR "Could not find Vulkan library!")
else()
	message(STATUS ${Vulkan_LIBRARY})
endif()

include_directories(${Vulkan_INCLUDE_DIR})
set(ALL_LIBS  ${Vulkan_LIBRARY})
link_libraries(${Vulkan_LIBRARY})
message(${Vulkan_LIBRARY})

set(ADDITIONAL_INCLUDE_DIRS include/include)
set(ADDITIONAL_LIBRARY_DIRS include/lib)   
set(ADDITIONAL_RUNTIME_LIBRARY_DIRS include/bin) 
include_directories(${ADDITIONAL_INCLUDE_DIRS})

# get rid of annoying MSVC warnings.
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

set (CMAKE_CXX_STANDARD 17)
if(WIN32)
    link_directories(${ADDITIONAL_LIBRARY_DIRS})
else()
    find_package(glfw3 REQUIRED)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX -D_USE_MATH_DEFINES")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -idirafter /usr/include/")
#uncomment this to detect broken memory problems via gcc sanitizers
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize-address-use-after-scope -fno-omit-frame-pointer -fsanitize=leak -fsanitize=undefined -fsanitize=bounds-strict")

add_executable(${PROJECT_NAME} source/main.cpp source/VulkanApp.h source/VulkanApp.cpp source/stb_image.h)

target_include_directories(${PROJECT_NAME} PRIVATE ${OPENGL_INCLUDE_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
#set_target_properties(${PROJECT_NAME} PROPERTIES LINK_LIBRARIES "%(AdditionalDependencies)")
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory include/bin build/)
target_link_libraries(${PROJECT_NAME} ${ALL_LIBS} ${OPENGL_LIBRARY} ${OPENGL_gl_LIBRARY} glfw3dll)

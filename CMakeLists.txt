cmake_minimum_required (VERSION 3.8)
set(PROJECT_NAME ${PROJECT}-${BUILD_TYPE}.exe)

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
    set(CMAKE_FIND_ROOT_PATH  /usr/lib/${TOOLCHAIN_PREFIX}/7.3-win32/include/c++)
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER)
endif()

project(${PROJECT_NAME})

#if (WIN32)
#    set(VULKAN_HEADERS_INSTALL_DIR "/mnt/c/VulkanSDK/1.2.170.0/Include/vulkan") 
#endif()

find_package(Vulkan)
if(WIN32)
	if(NOT Vulkan_FOUND)
		find_library(Vulkan_LIBRARY NAMES vulkan-1 vulkan PATHS ${CMAKE_SOURCE_DIR}/libs/vulkan)
		if(Vulkan_LIBRARY)
			set(Vulkan_FOUND ON)
			MESSAGE("Using bundled Vulkan library version")
        endif()
    endif()
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

# get rid of annoying MSVC warnings.
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

set (CMAKE_CXX_STANDARD 17)

if(WIN32)
    
    link_directories(${ADDITIONAL_LIBRARY_DIRS})
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX -D_USE_MATH_DEFINES")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -idirafter /usr/include/")
#uncomment this to detect broken memory problems via gcc sanitizers
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize-address-use-after-scope -fno-omit-frame-pointer -fsanitize=leak -fsanitize=undefined -fsanitize=bounds-strict")

add_executable(${PROJECT_NAME} source/main.cpp source/VulkanApp.h source/VulkanApp.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
#set_target_properties(${PROJECT_NAME} PROPERTIES LINK_LIBRARIES "%(AdditionalDependencies)")
target_link_libraries(${PROJECT_NAME} ${ALL_LIBS})

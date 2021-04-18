cmake_minimum_required (VERSION 3.8)

set(PROJECT_NAME ${PROJECT}-${BUILD_TYPE}.exe)
project(${PROJECT_NAME})
set(PROJECT_ROOT source)
message("project root is ${PROJECT_ROOT}")

find_package(Vulkan)
find_package(SDL2 REQUIRED)

include_directories(${Vulkan_INCLUDE_DIR})
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${PROJECT_ROOT})

# get rid of annoying MSVC warnings.
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

set(ALL_LIBS  ${Vulkan_LIBRARY} )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
if(BUILD_TYPE STREQUAL "dbg")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g -O0 -DDEBUG -fsanitize=address -fsanitize-address-use-after-scope -fno-omit-frame-pointer -fsanitize=leak -fsanitize=undefined -fsanitize=bounds-strict")
    message("DebugBuild")
elseif(BUILD_TYPE STREQUAL "dev")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -DDEBUG")
    message("DevelopBuild")
elseif(BUILD_TYPE STREQUAL "rel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -O3")
    message("ReleaseBuild")
endif()


#if(WIN32)
#  link_directories(${ADDITIONAL_LIBRARY_DIRS})
#else()
#  find_package(glfw3 REQUIRED)
#  include_directories(${GLFW_INCLUDE_DIRS})
#  set(ALL_LIBS ${ALL_LIBS} ${GLFW_LIBRARIES} )
#endif()

#uncomment this to detect broken memory problems via gcc sanitizers
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize-address-use-after-scope -fno-omit-frame-pointer -fsanitize=leak -fsanitize=undefined -fsanitize=bounds-strict")

file(GLOB_RECURSE PROJECT_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "${PROJECT_ROOT}/*.cpp")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

#set_target_properties(vulkan_minimal_graphics PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${CMAKE_DL_LIBS} ${ALL_LIBS})

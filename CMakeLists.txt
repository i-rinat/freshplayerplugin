project(freshplayerplugin)
cmake_minimum_required(VERSION 2.8.8)

# request pthreads
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99  -fPIC -fvisibility=hidden")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror=format-security")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=implicit-function-declaration")

# add missing NDEBUG in RelWithDebugInfo
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -DNDEBUG -g3")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -DNDEBUG -g3")

# generate more debug info for Debug build type
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g3")

# request pthreads
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

include_directories(3rdparty)

add_subdirectory(3rdparty)
add_subdirectory(src)
add_subdirectory(tests)

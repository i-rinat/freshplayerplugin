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

# find Ragel
find_program(RAGEL ragel)
if (${RAGEL} STREQUAL "RAGEL-NOTFOUND")
    message(FATAL_ERROR "Ragel State Machine Compiler not found")
endif()

# dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(REQ
    alsa
    glib-2.0
    x11
    gl
    glesv2
    libconfig
    libevent
    libevent_pthreads
    cairo
    pango
    pangocairo
    pangoft2
    freetype2
    gtk+-2.0
    REQUIRED)

pkg_check_modules(PULSEAUDIO libpulse)

if (${PULSEAUDIO_FOUND})
    add_definitions(-DUSE_PULSEAUDIO)
    set(USE_PULSEAUDIO 1)
endif()

include_directories(${REQ_INCLUDE_DIRS})
add_definitions(${REQ_CFLAGS_OTHER})
add_definitions(-DXP_UNIX)
link_directories(${REQ_LIBRARY_DIRS})


include_directories(3rdparty)

add_subdirectory(3rdparty)
add_subdirectory(src)
add_subdirectory(tests)

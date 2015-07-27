project(freshplayerplugin)
cmake_minimum_required(VERSION 2.8.8)

# setup compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -fPIC -fvisibility=hidden -pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror=format-security")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=implicit-function-declaration")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -fvisibility=hidden -pthread")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror=format-security")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=implicit-function-declaration")

# add missing NDEBUG in RelWithDebugInfo
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -DNDEBUG -g3")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -DNDEBUG -g3")

# generate more debug info for Debug build type
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g3")

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
    gio-2.0
    x11
    xrandr
    xrender
    xcursor
    gl
    libconfig
    libdrm
    libevent
    libevent_pthreads
    cairo
    pango
    pangocairo
    pangoft2
    freetype2
    openssl
    REQUIRED)

# libdl.so, librt.so, and libm.so
list(APPEND REQ_LIBRARIES dl rt m)

# select GTK+ version
set(WITH_GTK 2 CACHE INTEGER "select GTK+ major version to link with (2 or 3)")
if (WITH_GTK EQUAL 2)
    pkg_check_modules(GTK gtk+-2.0 REQUIRED)
elseif (WITH_GTK EQUAL 3)
    pkg_check_modules(GTK gtk+-3.0 REQUIRED)
else()
    message(FATAL_ERROR "Unsupported GTK+ version ${GTK_VERSION}")
endif()
list(APPEND REQ_LIBRARY_DIRS ${GTK_LIBRARY_DIRS})
list(APPEND REQ_INCLUDE_DIRS ${GTK_INCLUDE_DIRS})
list(APPEND REQ_LIBRARIES    ${GTK_LIBRARIES})

# OpenGL ES 2
set(WITH_GLES2 TRUE CACHE STRING "use system libGLESv2")
if (WITH_GLES2)
    pkg_check_modules(GLES2 glesv2 REQUIRED)
    add_definitions(-DHAVE_GLES2=1)
    list(APPEND REQ_LIBRARY_DIRS ${GLES2_LIBRARY_DIRS})
    list(APPEND REQ_INCLUDE_DIRS ${GLES2_INCLUDE_DIRS})
    list(APPEND REQ_LIBRARIES    ${GLES2_LIBRARIES})
else()
    # otherwise ANGLE will be used for shader translation
endif()

# optional dependencies
message(STATUS "checking for optional dependencies")

pkg_check_modules(PULSEAUDIO QUIET libpulse)
pkg_check_modules(JACK QUIET jack)
pkg_check_modules(SOXR QUIET soxr)
set(WITH_PULSEAUDIO TRUE CACHE STRING "enable PulseAudio support")
set(WITH_JACK TRUE CACHE STRING "enable JACK Audio Connection Kit")

if (PULSEAUDIO_FOUND AND WITH_PULSEAUDIO)
    add_definitions(-DHAVE_PULSEAUDIO=1)
    message(STATUS "  found libpulse, version ${PULSEAUDIO_VERSION} (optional)")
    list(APPEND REQ_LIBRARY_DIRS ${PULSEAUDIO_LIBRARY_DIRS})
    list(APPEND REQ_INCLUDE_DIRS ${PULSEAUDIO_INCLUDE_DIRS})
    list(APPEND REQ_LIBRARIES    ${PULSEAUDIO_LIBRARIES})
else()
    message(STATUS "  no libpulse found (optional)")
endif()

if (JACK_FOUND AND WITH_JACK)
    message(STATUS "  found jack, version ${JACK_VERSION} (optional)")
    if (SOXR_FOUND)
        message(STATUS "  found soxr, version ${SOXR_VERSION}")
        add_definitions(-DHAVE_JACK=1)
        list(APPEND REQ_LIBRARY_DIRS "${JACK_LIBRARY_DIRS}" "${SOXR_LIBRARY_DIRS}")
        list(APPEND REQ_INCLUDE_DIRS "${JACK_INCLUDE_DIRS}" "${SOXR_INCLUDE_DIRS}")
        list(APPEND REQ_LIBRARIES    "${JACK_LIBRARIES}" "${SOXR_LIBRARIES}")
    else()
        message(STATUS "  no soxr found, JACK output disabled")
    endif()
else()
    message(STATUS "  no jack found (optional)")
endif()

# hw accelerated decoding
set(WITH_HWDEC TRUE CACHE STRING "enable hw accelerated decoding")
if (WITH_HWDEC)
    pkg_check_modules(HWDEC libavcodec libavutil libva libva-x11 vdpau REQUIRED)
    list(APPEND REQ_LIBRARY_DIRS ${HWDEC_LIBRARY_DIRS})
    list(APPEND REQ_INCLUDE_DIRS ${HWDEC_INCLUDE_DIRS})
    list(APPEND REQ_LIBRARIES    ${HWDEC_LIBRARIES})
    add_subdirectory(cmake/ffmpeg-compat)
    include_directories("${CMAKE_BINARY_DIR}")
endif()

# using libv4l2 for colorspace conversion
set(WITH_LIBV4L2 TRUE CACHE STRING "use libv4l2 for colorspace conversion")
if (WITH_LIBV4L2)
    add_definitions(-DHAVE_LIBV4L2=1)
    pkg_check_modules(LIBV4L2 libv4l2 REQUIRED)
    list(APPEND REQ_LIBRARY_DIRS ${LIBV4L2_LIBRARY_DIRS})
    list(APPEND REQ_INCLUDE_DIRS ${LIBV4L2_INCLUDE_DIRS})
    list(APPEND REQ_LIBRARIES    ${LIBV4L2_LIBRARIES})
else()
    add_definitions(-DHAVE_LIBV4L2=0)
endif()

list(APPEND REQ_LIBRARIES img-resources)

include_directories(${REQ_INCLUDE_DIRS})
link_directories(${REQ_LIBRARY_DIRS})
add_definitions(${REQ_CFLAGS_OTHER})
add_definitions(-DXP_UNIX)

include_directories(3rdparty)
if (NOT WITH_GLES2)
    include_directories(3rdparty/angle/include)
    list(APPEND REQ_LIBRARIES shader_translator)
endif()

add_subdirectory(3rdparty)
add_subdirectory(resources)
add_subdirectory(src)
add_subdirectory(tests)

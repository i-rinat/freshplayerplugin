# FFmpeg (libav) interface changes over time. Something new arises, something getting deprecated.
# This script checks some essential parts

include(CheckSymbolExists)
include(CheckCSourceCompiles)

set(CMAKE_REQUIRED_LIBRARIES "${REQ_LIBRARIES}")

file(READ cfg_avpixelformat.c C_SRC)
CHECK_C_SOURCE_COMPILES("${C_SRC}" HAVE_AVPixelFormat)
if (NOT HAVE_AVPixelFormat)
    set(HAVE_AVPixelFormat 0)
endif()

file(READ cfg_avcodecid.c C_SRC)
CHECK_C_SOURCE_COMPILES("${C_SRC}" HAVE_AVCodecID)
if (NOT HAVE_AVCodecID)
    set(HAVE_AVCodecID 0)
endif()

file(READ cfg_avcodeccontext_get_buffer2.c C_SRC)
CHECK_C_SOURCE_COMPILES("${C_SRC}" HAVE_AVCodecContext_get_buffer2)
if (NOT HAVE_AVCodecContext_get_buffer2)
    set(HAVE_AVCodecContext_get_buffer2 0)
endif()


CHECK_SYMBOL_EXISTS(av_frame_alloc "libavcodec/avcodec.h" HAVE_av_frame_alloc)
if (NOT HAVE_av_frame_alloc)
    set(HAVE_av_frame_alloc 0)
endif()

CHECK_SYMBOL_EXISTS(av_frame_free "libavcodec/avcodec.h" HAVE_av_frame_free)
if (NOT HAVE_av_frame_free)
    set(HAVE_av_frame_free 0)
endif()

CHECK_SYMBOL_EXISTS(avcodec_free_context "libavcodec/avcodec.h" HAVE_avcodec_free_context)
if (NOT HAVE_avcodec_free_context)
    set(HAVE_avcodec_free_context 0)
endif()

configure_file("ffmpeg-compat.h.in" "${CMAKE_BINARY_DIR}/ffmpeg-compat.h" @ONLY)

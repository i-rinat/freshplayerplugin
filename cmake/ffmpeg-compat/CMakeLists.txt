# FFmpeg (libav) interface changes over time. Something new arises, something getting deprecated.
# This script checks some essential parts

include(CheckSymbolExists)
include(CheckCSourceCompiles)

set(CMAKE_REQUIRED_LIBRARIES "${REQ_LIBRARIES}")
set(CMAKE_REQUIRED_INCLUDES  "${REQ_INCLUDE_DIRS}")

macro(MACRO_C_SOURCE_CHECK FILE_NAME HAVE_variable)
    unset(${HAVE_variable} CACHE)
    file(READ ${FILE_NAME} C_SRC)
    CHECK_C_SOURCE_COMPILES("${C_SRC}" ${HAVE_variable})
    if (NOT ${HAVE_variable})
        set(${HAVE_variable} 0)
    endif()
endmacro()

macro(MACRO_SYMBOL_CHECK SYMBOL HEADER_FILE HAVE_variable)
    unset(${HAVE_variable} CACHE)
    CHECK_SYMBOL_EXISTS(${SYMBOL} ${HEADER_FILE} ${HAVE_variable})
    if (NOT ${HAVE_variable})
        set(${HAVE_variable} 0)
    endif()
endmacro()

MACRO_C_SOURCE_CHECK(cfg_avpixelformat.c HAVE_AVPixelFormat)
MACRO_C_SOURCE_CHECK(cfg_av_pix_fmt_vdpau.c HAVE_AV_PIX_FMT_VDPAU)
MACRO_C_SOURCE_CHECK(cfg_avcodecid.c HAVE_AVCodecID)
MACRO_C_SOURCE_CHECK(cfg_avcodeccontext_get_buffer2.c HAVE_AVCodecContext_get_buffer2)
MACRO_C_SOURCE_CHECK(cfg_AVVDPAUContext.c HAVE_AVVDPAUContext)

MACRO_SYMBOL_CHECK(av_frame_alloc "libavcodec/avcodec.h" HAVE_av_frame_alloc)
MACRO_SYMBOL_CHECK(av_frame_free "libavcodec/avcodec.h" HAVE_av_frame_free)
MACRO_SYMBOL_CHECK(avcodec_free_context "libavcodec/avcodec.h" HAVE_avcodec_free_context)

configure_file("ffmpeg-compat.h.in" "${CMAKE_BINARY_DIR}/ffmpeg-compat.h" @ONLY)

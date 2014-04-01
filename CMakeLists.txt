project(freshplayerplugin)
cmake_minimum_required(VERSION 2.6)
add_definitions(-std=gnu99 -Wall -fPIC -fvisibility=hidden)

# add missing NDEBUG in RelWithDebugInfo
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -DNDEBUG -ggdb3")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -DNDEBUG -ggdb3")

# generate more debug info for Debug build type
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb3")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -ggdb3")

find_package(PkgConfig REQUIRED)

pkg_check_modules(REQ
	alsa
	glib-2.0
	mozilla-plugin
	x11
	liburiparser
	cairo
	pango
	pangocairo
	pangoft2
	freetype2
	REQUIRED)

include_directories(include ${REQ_INCLUDE_DIRS})
add_definitions(${REQ_CFLAGS_OTHER})
link_directories(${REQ_LIBRARY_DIRS})

add_library(freshwrapper SHARED
	src/np_entry.c
	src/np_functions.c
	src/reverse_constant.c
	src/tables.c
	src/trace.c
	src/browser_object.c
	src/instance_object.c
	src/pp_interface.c
	src/pp_resource.c
	src/ppb_core.c
	src/ppb_audio_config.c
	src/ppb_audio_input_dev.c
	src/ppb_audio.c
	src/ppb_browser_font_trusted.c
	src/ppb_buffer_dev.c
	src/ppb_char_set_dev.c
	src/ppb_crypto_dev.c
	src/ppb_cursor_control_dev.c
	src/ppb_file_chooser_dev.c
	src/ppb_file_chooser_trusted.c
	src/ppb_file_ref.c
	src/ppb_flash_clipboard.c
	src/ppb_flash_file.c
	src/ppb_flash_file_modulelocal.c
	src/ppb_flash_font_file.c
	src/ppb_flash_fullscreen.c
	src/ppb_flash.c
	src/ppb_flash_menu.c
	src/ppb_font_dev.c
	src/ppb_graphics2d.c
	src/ppb_graphics3d.c
	src/ppb_image_data.c
	src/ppb_ime_input_event_dev.c
	src/ppb_input_event.c
	src/ppb_instance.c
	src/ppb_instance_private.c
	src/ppb_memory_dev.c
	src/ppb_net_address_private.c
	src/ppb_network_monitor.c
	src/ppb_opengles2_chromium_enable_feature.c
	src/ppb_opengles2_chromium_map_sub.c
	src/ppb_opengles2_framebuffer_blit.c
	src/ppb_opengles2_framebuffer_multisample.c
	src/ppb_opengles2_instanced_arrays.c
	src/ppb_opengles2_query.c
	src/ppb_opengles2.c
	src/ppb_printing_dev.c
	src/ppb_tcp_socket_private.c
	src/ppb_text_input_dev.c
	src/ppb_udp_socket_private.c
	src/ppb_url_loader.c
	src/ppb_url_loader_trusted.c
	src/ppb_url_request_info.c
	src/ppb_url_response_info.c
	src/ppb_url_util_dev.c
	src/ppb_var.c
	src/ppb_var_deprecated.c
	src/ppb_video_capture_dev.c
	src/ppb_view.c
)

target_link_libraries(freshwrapper
	-ldl
	${REQ_LIBRARIES}
)

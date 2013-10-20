project(freshplayerplugin)
cmake_minimum_required(VERSION 2.6)
add_definitions(-std=gnu99 -Wall -fPIC -fvisibility=hidden)

find_package(PkgConfig REQUIRED)

pkg_check_modules(REQ glib-2.0 mozilla-plugin x11 liburiparser REQUIRED)

include_directories(include ${REQ_INCLUDE_DIRS})
add_definitions(${REQ_CFLAGS_OTHER})
link_directories(${REQ_LIBRARY_DIRS})

add_library(freshwrapper SHARED
	src/np_entry.c
	src/np_functions.c
	src/globals.c
	src/reverse_constant.c
	src/tables.c
	src/trace.c
	src/pp_interface.c
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
	src/ppb_flash_file_file_ref.c
	src/ppb_flash_file_modulelocal.c
	src/ppb_flash_font_file.c
	src/ppb_flash_fullscreen.c
	src/ppb_flash.c
	src/ppb_flash_menu.c
	src/ppb_graphics2d.c
	src/ppb_graphics3d.c
	src/ppb_image_data.c
	src/ppb_ime_input_event_dev.c
	src/ppb_input_event.c
	src/ppb_instance.c
	src/ppb_instance_private.c
	src/ppb_memory_dev.c
	src/ppb_net_address_private.c
	src/ppb_opengles2_chromium_map_sub.c
	src/ppb_opengles2.c
	src/ppb_tcp_socket_private.c
	src/ppb_text_input_dev.c
	src/ppb_udp_socket_private.c
	src/ppb_url_loader.c
	src/ppb_url_loader_trusted.c
	src/ppb_url_request_info.c
	src/ppb_url_response_info.c
	src/ppb_url_util_dev.c
	src/ppb_var.c
	src/ppb_video_capture_dev.c
	src/ppb_view.c
)

target_link_libraries(freshwrapper
	-ldl
	${REQ_LIBRARIES}
)

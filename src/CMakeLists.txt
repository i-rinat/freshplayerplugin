set(TRACE_ALL 0 CACHE BOOLEAN "Enable tracing implemented functions")
if(${TRACE_ALL})
    add_definitions(-DTRACE_ALL)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(MOZPLUGIN_INSTALL_DIR "lib${LIB_SUFFIX}/mozilla/plugins/" CACHE STRING "plugin install directory")

add_subdirectory(uri_parser)
add_subdirectory(config_parser)

set(source_list
    async_network.c
    audio_thread.c
    audio_thread_alsa.c
    audio_thread_noaudio.c
    config.c
    compat.c
    encoding_alias.c
    font.c
    gtk_wrapper.c
    header_parser.c
    keycodeconvert.c
    np_asynccall.c
    np_entry.c
    np_functions.c
    main_thread.c
    reverse_constant.c
    tables.c
    trace_helpers.c
    trace_core.c
    n2p_proxy_class.c
    p2n_proxy_class.c
    pp_interface.c
    pp_resource.c
    ppb_core.c
    ppb_audio_config.c
    ppb_audio_input.c
    ppb_audio.c
    ppb_browser_font.c
    ppb_buffer.c
    ppb_char_set.c
    ppb_crypto.c
    ppb_cursor_control.c
    ppb_device_ref.c
    ppb_file_chooser.c
    ppb_file_io.c
    ppb_file_ref.c
    ppb_flash_clipboard.c
    ppb_flash_drm.c
    ppb_flash_file.c
    ppb_flash_font_file.c
    ppb_flash_fullscreen.c
    ppb_flash.c
    ppb_flash_menu.c
    ppb_flash_message_loop.c
    ppb_font.c
    ppb_graphics2d.c
    ppb_graphics3d.c
    ppb_host_resolver.c
    ppb_image_data.c
    ppb_input_event.c
    ppb_instance.c
    ppb_memory.c
    ppb_message_loop.c
    ppb_messaging.c
    ppb_net_address.c
    ppb_network_monitor.c
    ppb_opengles2.c
    ppb_pdf.c
    ppb_printing.c
    ppb_tcp_socket.c
    ppb_text_input_controller.c
    ppb_text_input.c
    ppb_udp_socket.c
    ppb_uma.c
    ppb_url_loader.c
    ppb_url_request_info.c
    ppb_url_response_info.c
    ppb_url_util.c
    ppb_var.c
    ppb_video_capture.c
    ppb_view.c
    ppb_x509_certificate.c
    screensaver_control.c
    x11_event_thread.c
)

if (PULSEAUDIO_FOUND AND WITH_PULSEAUDIO)
    list(APPEND source_list audio_thread_pulseaudio.c)
endif()

if (JACK_FOUND AND SOXR_FOUND AND WITH_JACK)
    list(APPEND source_list audio_thread_jack.c)
endif()

if (NOT WITH_GLES2)
    list(APPEND source_list shader_translator.cc)
endif()

if (WITH_HWDEC)
    add_definitions(-DHAVE_HWDEC=1)
    list(APPEND source_list ppb_video_decoder.c)
else()
    add_definitions(-DHAVE_HWDEC=0)
endif()

add_executable(link_check link_check.c)
target_link_libraries(link_check ${REQ_LIBRARIES})

add_library(freshwrapper-obj OBJECT ${source_list})
add_dependencies(freshwrapper-obj link_check)

# ensure libfreshwrapper-*.so are linked with all libraries they use
add_executable(dep_check dep_check.c)
link_directories(${CMAKE_CURRENT_BINARY_DIR})

set(COMMON_OBJ_LIST
    $<TARGET_OBJECTS:freshwrapper-obj>
    $<TARGET_OBJECTS:parson-obj>
    $<TARGET_OBJECTS:uri-parser-obj>
    $<TARGET_OBJECTS:config-parser-obj>
)

# wrapper for PepperFlash
set(WITH_PEPPERFLASH TRUE CACHE STRING "generate wrapper for PepperFlash")
if (WITH_PEPPERFLASH)
    add_library(freshwrapper-flashplayer SHARED ${COMMON_OBJ_LIST} config_pepperflash.c)
    target_link_libraries(freshwrapper-flashplayer ${SYMBOLMAP} ${REQ_LIBRARIES})
    target_link_libraries(dep_check freshwrapper-flashplayer)
    install(TARGETS freshwrapper-flashplayer DESTINATION ${MOZPLUGIN_INSTALL_DIR})
endif()

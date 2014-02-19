#ifndef FPP__INTERFACE_LIST_H
#define FPP__INTERFACE_LIST_H

#include <ppapi/c/ppb_audio_config.h>
#include <ppapi/c/ppb_audio.h>
#include <ppapi/c/ppb_core.h>
#include <ppapi/c/ppb_file_ref.h>
#include <ppapi/c/ppb_graphics_2d.h>
#include <ppapi/c/ppb_graphics_3d.h>
#include <ppapi/c/ppb.h>
#include <ppapi/c/ppb_image_data.h>
#include <ppapi/c/ppb_instance.h>
#include <ppapi/c/ppb_network_monitor.h>
#include <ppapi/c/ppb_opengles2.h>
#include <ppapi/c/ppb_url_loader.h>
#include <ppapi/c/ppb_url_request_info.h>
#include <ppapi/c/ppb_url_response_info.h>
#include <ppapi/c/ppb_var.h>
#include <ppapi/c/ppb_view.h>
#include <ppapi/c/pp_module.h>

#include <ppapi/c/dev/ppb_audio_input_dev.h>
#include <ppapi/c/dev/ppb_buffer_dev.h>
#include <ppapi/c/dev/ppb_char_set_dev.h>
#include <ppapi/c/dev/ppb_crypto_dev.h>
#include <ppapi/c/dev/ppb_cursor_control_dev.h>
#include <ppapi/c/dev/ppb_file_chooser_dev.h>
#include <ppapi/c/dev/ppb_ime_input_event_dev.h>
#include <ppapi/c/dev/ppb_memory_dev.h>
#include <ppapi/c/dev/ppb_text_input_dev.h>
#include <ppapi/c/dev/ppb_url_util_dev.h>
#include <ppapi/c/dev/ppb_video_capture_dev.h>

#include <ppapi/c/private/ppb_flash_clipboard.h>
#include <ppapi/c/private/ppb_flash_file.h>
#include <ppapi/c/private/ppb_flash_font_file.h>
#include <ppapi/c/private/ppb_flash_fullscreen.h>
#include <ppapi/c/private/ppb_flash.h>
#include <ppapi/c/private/ppb_flash_menu.h>
#include <ppapi/c/private/ppb_instance_private.h>
#include <ppapi/c/private/ppb_tcp_socket_private.h>
#include <ppapi/c/private/ppb_udp_socket_private.h>
#include <ppapi/c/trusted/ppb_browser_font_trusted.h>
#include <ppapi/c/trusted/ppb_file_chooser_trusted.h>
#include <ppapi/c/trusted/ppb_url_loader_trusted.h>

extern const struct PPB_Core_1_0                    ppb_core_interface_1_0;
extern const struct PPB_AudioConfig_1_1             ppb_audio_config_interface_1_1;
extern const struct PPB_AudioInput_Dev_0_3          ppb_audio_input_dev_interface_0_3;
extern const struct PPB_AudioInput_Dev_0_4          ppb_audio_input_dev_interface_0_4;
extern const struct PPB_Audio_1_0                   ppb_audio_interface_1_0;
extern const struct PPB_BrowserFont_Trusted_1_0     ppb_browser_font_trusted_interface_1_0;
extern const struct PPB_Buffer_Dev_0_4              ppb_buffer_dev_interface_0_4;
extern const struct PPB_CharSet_Dev_0_4             ppb_char_set_dev_interface_0_4;
extern const struct PPB_Crypto_Dev_0_1              ppb_crypto_dev_interface_0_1;
extern const struct PPB_CursorControl_Dev_0_4       ppb_cursor_control_dev_interface_0_4;
extern const struct PPB_FileChooser_Dev_0_6         ppb_file_chooser_dev_interface_0_6;
extern const struct PPB_FileChooserTrusted_0_6      ppb_file_chooser_trusted_interface_0_6;
extern const struct PPB_FileRef_1_0                 ppb_file_ref_interface_1_0;
extern const struct PPB_Flash_Clipboard_5_0         ppb_flash_clipboard_interface_5_0;
extern const struct PPB_Flash_File_FileRef          ppb_flash_file_file_ref_interface_2_0;
extern const struct PPB_Flash_File_ModuleLocal_3_0  ppb_flash_file_modulelocal_interface_3_0;
extern const struct PPB_Flash_FontFile_0_1          ppb_flash_font_file_interface_0_1;
extern const struct PPB_FlashFullscreen_1_0         ppb_flash_fullscreen_interface_1_0;
extern const struct PPB_Flash_13_0                  ppb_flash_interface_13_0;
extern const struct PPB_Flash_Menu_0_2              ppb_flash_menu_interface_0_2;
extern const struct PPB_Graphics2D_1_0              ppb_graphics2d_interface_1_0;
extern const struct PPB_Graphics3D_1_0              ppb_graphics3d_interface_1_0;
extern const struct PPB_ImageData_1_0               ppb_image_data_interface_1_0;
extern const struct PPB_IMEInputEvent_Dev_0_2       ppb_ime_input_event_dev_interface_0_2;
extern const struct PPB_InputEvent_1_0              ppb_input_event_interface_1_0;
extern const struct PPB_Instance_1_0                ppb_instance_interface_1_0;
extern const struct PPB_Instance_Private_0_1        ppb_instance_private_interface_0_1;
extern const struct PPB_Memory_Dev_0_1              ppb_memory_dev_interface_0_1;
extern const struct PPB_NetAddress_Private_1_1      ppb_net_address_private_interface_1_1;
extern const struct PPB_NetworkMonitor_1_0          ppb_network_monitor_interface_1_0;
extern const struct PPB_OpenGLES2ChromiumMapSub     ppb_opengles2_chromium_map_sub_interface_1_0;
extern const struct PPB_OpenGLES2                   ppb_opengles2_interface_1_0;
extern const struct PPB_TCPSocket_Private_0_4       ppb_tcp_socket_private_interface_0_4;
extern const struct PPB_TextInput_Dev_0_2           ppb_text_input_dev_interface_0_2;
extern const struct PPB_UDPSocket_Private_0_4       ppb_udp_socket_private_interface_0_4;
extern const struct PPB_URLLoader_1_0               ppb_url_loader_interface_1_0;
extern const struct PPB_URLLoaderTrusted_0_3        ppb_url_loader_trusted_interface_0_3;
extern const struct PPB_URLRequestInfo_1_0          ppb_url_request_info_interface_1_0;
extern const struct PPB_URLResponseInfo_1_0         ppb_url_response_info_interface_1_0;
extern const struct PPB_URLUtil_Dev_0_6             ppb_url_util_dev_interface_0_6;
extern const struct PPB_Var_1_0                     ppb_var_interface_1_0;
extern const struct PPB_Var_1_1                     ppb_var_interface_1_1;
extern const struct PPB_VideoCapture_Dev_0_3        ppb_video_capture_dev_interface_0_3;
extern const struct PPB_View_1_0                    ppb_view_interface_1_0;
extern const struct PPB_View_1_1                    ppb_view_interface_1_1;


PP_Resource
ppb_audio_config_create_stereo_16_bit(PP_Instance instance, PP_AudioSampleRate sample_rate,
                                      uint32_t sample_frame_count);

uint32_t
ppb_audio_config_recommend_sample_frame_count(PP_Instance instance, PP_AudioSampleRate sample_rate,
                                              uint32_t requested_sample_frame_count);

PP_Bool
ppb_audio_config_is_audio_config(PP_Resource resource);

PP_AudioSampleRate
ppb_audio_config_get_sample_rate(PP_Resource config);

uint32_t
ppb_audio_config_get_sample_frame_count(PP_Resource config);

PP_AudioSampleRate
ppb_audio_config_recommend_sample_rate(PP_Instance instance);

PP_Resource
ppb_audio_input_dev_create(PP_Instance instance);

PP_Bool
ppb_audio_input_dev_is_audio_input(PP_Resource resource);

int32_t
ppb_audio_input_dev_enumerate_devices(PP_Resource audio_input, struct PP_ArrayOutput output,
                                      struct PP_CompletionCallback callback);

int32_t
ppb_audio_input_dev_monitor_device_change(PP_Resource audio_input,
                                          PP_MonitorDeviceChangeCallback callback, void *user_data);

int32_t
ppb_audio_input_dev_open_0_3(PP_Resource audio_input, PP_Resource device_ref, PP_Resource config,
                             PPB_AudioInput_Callback_0_2 audio_input_callback, void *user_data,
                             struct PP_CompletionCallback callback);

int32_t
ppb_audio_input_dev_open(PP_Resource audio_input, PP_Resource device_ref, PP_Resource config,
                         PPB_AudioInput_Callback audio_input_callback, void *user_data,
                         struct PP_CompletionCallback callback);

PP_Resource
ppb_audio_input_dev_get_current_config(PP_Resource audio_input);

PP_Bool
ppb_audio_input_dev_start_capture(PP_Resource audio_input);

PP_Bool
ppb_audio_input_dev_stop_capture(PP_Resource audio_input);

void
ppb_audio_input_dev_close(PP_Resource audio_input);

void
ppb_core_add_ref_resource(PP_Resource resource);

void
ppb_core_release_resource(PP_Resource resource);

PP_Time
ppb_core_get_time(void);

PP_TimeTicks
ppb_core_get_time_ticks(void);

void
ppb_core_call_on_main_thread(int32_t delay_in_milliseconds, struct PP_CompletionCallback callback,
                             int32_t result);

PP_Bool
ppb_core_is_main_thread(void);

void
ppb_crypto_dev_get_random_bytes(char *buffer, uint32_t num_bytes);

void
ppb_flash_set_instance_always_on_top(PP_Instance instance, PP_Bool on_top);

PP_Bool
ppb_flash_draw_glyphs(PP_Instance instance, PP_Resource pp_image_data,
                      const struct PP_BrowserFont_Trusted_Description *font_desc,
                      uint32_t color, const struct PP_Point *position, const struct PP_Rect *clip,
                      const float transformation[3][3], PP_Bool allow_subpixel_aa,
                      uint32_t glyph_count, const uint16_t glyph_indices[],
                      const struct PP_Point glyph_advances[]);

struct PP_Var
ppb_flash_get_proxy_for_url(PP_Instance instance, const char *url);

int32_t
ppb_flash_navigate(PP_Resource request_info, const char *target, PP_Bool from_user_action);

double
ppb_flash_get_local_time_zone_offset(PP_Instance instance, PP_Time t);

struct PP_Var
ppb_flash_get_command_line_args(PP_Module module);

void
ppb_flash_preload_font_win(const void *logfontw);

PP_Bool
ppb_flash_is_rect_topmost(PP_Instance instance, const struct PP_Rect *rect);

void
ppb_flash_update_activity(PP_Instance instance);

struct PP_Var
ppb_flash_get_setting(PP_Instance instance, PP_FlashSetting setting);

PP_Bool
ppb_flash_set_crash_data(PP_Instance instance, PP_FlashCrashKey key, struct PP_Var value);

int32_t
ppb_flash_enumerate_video_capture_devices(PP_Instance instance, PP_Resource video_capture,
                                          struct PP_ArrayOutput devices);

PP_Bool
ppb_flash_file_modulelocal_create_thread_adapter_for_instance(PP_Instance instance);

void
ppb_flash_file_modulelocal_clear_thread_adapter_for_instance(PP_Instance instance);

int32_t
ppb_flash_file_modulelocal_open_file(PP_Instance instance, const char *path, int32_t mode,
                                     PP_FileHandle *file);

int32_t
ppb_flash_file_modulelocal_rename_file(PP_Instance instance, const char *path_from,
                                       const char *path_to);

int32_t
ppb_flash_file_modulelocal_delete_file_or_dir(PP_Instance instance, const char *path,
                                              PP_Bool recursive);

int32_t
ppb_flash_file_modulelocal_create_dir(PP_Instance instance, const char *path);

int32_t
ppb_flash_file_modulelocal_query_file(PP_Instance instance, const char *path,
                                      struct PP_FileInfo *info);

int32_t
ppb_flash_file_modulelocal_get_dir_contents(PP_Instance instance, const char *path,
                                            struct PP_DirContents_Dev **contents);

void
ppb_flash_file_modulelocal_free_dir_contents(PP_Instance instance,
                                             struct PP_DirContents_Dev *contents);

int32_t
ppb_flash_file_modulelocal_create_temporary_file(PP_Instance instance, PP_FileHandle *file);

PP_Bool
ppb_flash_fullscreen_is_fullscreen(PP_Instance instance);

PP_Bool
ppb_flash_fullscreen_set_fullscreen(PP_Instance instance, PP_Bool fullscreen);

PP_Bool
ppb_flash_fullscreen_get_screen_size(PP_Instance instance, struct PP_Size* size);

int32_t
ppb_input_event_request_input_events(PP_Instance instance, uint32_t event_classes);

int32_t
ppb_input_event_request_filtering_input_events(PP_Instance instance, uint32_t event_classes);

void
ppb_input_event_clear_input_event_request(PP_Instance instance, uint32_t event_classes);

PP_Bool
ppb_input_event_is_input_event(PP_Resource resource);

PP_InputEvent_Type
ppb_input_event_get_type(PP_Resource event);

PP_TimeTicks
ppb_input_event_get_time_stamp(PP_Resource event);

uint32_t
ppb_input_event_get_modifiers(PP_Resource event);

PP_Bool
ppb_instance_bind_graphics(PP_Instance instance, PP_Resource device);

PP_Bool
ppb_instance_is_full_frame(PP_Instance instance);

struct PP_Var
ppb_instance_private_get_window_object(PP_Instance instance);

struct PP_Var
ppb_instance_private_get_owner_element_object(PP_Instance instance);

struct PP_Var
ppb_instance_private_execute_script(PP_Instance instance, struct PP_Var script,
                                    struct PP_Var *exception);

PP_Resource
ppb_network_monitor_create(PP_Instance instance);

int32_t
ppb_network_monitor_update_network_list(PP_Resource network_monitor, PP_Resource* network_list,
                                        struct PP_CompletionCallback callback);

PP_Bool
ppb_network_monitor_is_network_monitor(PP_Resource resource);

void
ppb_text_input_dev_interface_set_text_input_type(PP_Instance instance, PP_TextInput_Type_Dev type);

void
ppb_text_input_dev_interface_update_caret_position(PP_Instance instance,
                                                   const struct PP_Rect *caret,
                                                   const struct PP_Rect *bounding_box);

void
ppb_text_input_dev_interface_cancel_composition_text(PP_Instance instance);

void
ppb_text_input_dev_interface_update_surrounding_text(PP_Instance instance, const char *text,
                                                     uint32_t caret, uint32_t anchor);

void
ppb_text_input_dev_interface_selection_changed(PP_Instance instance);

PP_Resource
ppb_url_loader_create(PP_Instance instance);

PP_Bool
ppb_url_loader_is_url_loader(PP_Resource resource);

int32_t
ppb_url_loader_open(PP_Resource loader, PP_Resource request_info,
                    struct PP_CompletionCallback callback);

int32_t
ppb_url_loader_follow_redirect(PP_Resource loader, struct PP_CompletionCallback callback);

PP_Bool
ppb_url_loader_get_upload_progress(PP_Resource loader, int64_t *bytes_sent,
                                   int64_t *total_bytes_to_be_sent);

PP_Bool
ppb_url_loader_get_download_progress(PP_Resource loader, int64_t *bytes_received,
                                     int64_t *total_bytes_to_be_received);

PP_Resource
ppb_url_loader_get_response_info(PP_Resource loader);

int32_t
ppb_url_loader_read_response_body(PP_Resource loader, void *buffer, int32_t bytes_to_read,
                                  struct PP_CompletionCallback callback);

int32_t
ppb_url_loader_finish_streaming_to_file(PP_Resource loader, struct PP_CompletionCallback callback);

void
ppb_url_loader_close(PP_Resource loader);

void
ppb_url_loader_trusted_grant_universal_access(PP_Resource loader);

void
ppb_url_loader_trusted_register_status_callback(PP_Resource loader,
                                                PP_URLLoaderTrusted_StatusCallback cb);

PP_Resource
ppb_url_request_info_create(PP_Instance instance);

PP_Bool
ppb_url_request_info_is_url_request_info(PP_Resource resource);

PP_Bool
ppb_url_request_info_set_property(PP_Resource request, PP_URLRequestProperty property,
                                  struct PP_Var value);

PP_Bool
ppb_url_request_info_append_data_to_body(PP_Resource request, const void *data, uint32_t len);

PP_Bool
ppb_url_request_info_append_file_to_body(PP_Resource request, PP_Resource file_ref,
                                         int64_t start_offset, int64_t number_of_bytes,
                                         PP_Time expected_last_modified_time);

PP_Bool
ppb_url_response_info_is_url_response_info(PP_Resource resource);

struct PP_Var
ppb_url_response_info_get_property(PP_Resource response, PP_URLResponseProperty property);

PP_Resource
ppb_url_response_info_get_body_as_file_ref(PP_Resource response);

struct PP_Var
ppb_url_util_dev_canonicalize(struct PP_Var url, struct PP_URLComponents_Dev *components);

struct PP_Var
ppb_url_util_dev_resolve_relative_to_url(struct PP_Var base_url, struct PP_Var relative_string,
                                         struct PP_URLComponents_Dev *components);

struct PP_Var
ppb_url_util_dev_resolve_relative_to_document(PP_Instance instance, struct PP_Var relative_string,
                                              struct PP_URLComponents_Dev *components);

PP_Bool
ppb_url_util_dev_is_same_security_origin(struct PP_Var url_a, struct PP_Var url_b);

PP_Bool
ppb_url_util_dev_document_can_request(PP_Instance instance, struct PP_Var url);

PP_Bool
ppb_url_util_dev_document_can_access_document(PP_Instance active, PP_Instance target);

struct PP_Var
ppb_url_util_dev_get_document_url(PP_Instance instance, struct PP_URLComponents_Dev *components);

struct PP_Var
ppb_url_util_dev_get_plugin_instance_url(PP_Instance instance,
                                         struct PP_URLComponents_Dev *components);

void
ppb_var_add_ref(struct PP_Var var);

void
ppb_var_release(struct PP_Var var);

struct PP_Var
ppb_var_var_from_utf8_1_1(const char *data, uint32_t len);

struct PP_Var
ppb_var_var_from_utf8_1_0(PP_Module module, const char *data, uint32_t len);

const char *
ppb_var_var_to_utf8(struct PP_Var var, uint32_t *len);

PP_Resource
ppb_video_capture_dev_create(PP_Instance instance);

PP_Bool
ppb_video_capture_dev_is_video_capture(PP_Resource video_capture);

int32_t
ppb_video_capture_dev_enumerate_devices(PP_Resource video_capture, struct PP_ArrayOutput output,
                                        struct PP_CompletionCallback callback);

int32_t
ppb_video_capture_dev_monitor_device_change(PP_Resource video_capture,
                                            PP_MonitorDeviceChangeCallback callback,
                                            void *user_data);

int32_t
ppb_video_capture_dev_open(PP_Resource video_capture, PP_Resource device_ref,
                           const struct PP_VideoCaptureDeviceInfo_Dev *requested_info,
                           uint32_t buffer_count, struct PP_CompletionCallback callback);

int32_t
ppb_video_capture_dev_start_capture(PP_Resource video_capture);

int32_t
ppb_video_capture_dev_reuse_buffer(PP_Resource video_capture, uint32_t buffer);

int32_t
ppb_video_capture_dev_stop_capture(PP_Resource video_capture);

void
ppb_video_capture_dev_close(PP_Resource video_capture);

PP_Bool
ppb_view_is_view(PP_Resource resource);

PP_Bool
ppb_view_get_rect(PP_Resource resource, struct PP_Rect* rect);

PP_Bool
ppb_view_is_fullscreen(PP_Resource resource);

PP_Bool
ppb_view_is_visible(PP_Resource resource);

PP_Bool
ppb_view_is_page_visible(PP_Resource resource);

PP_Bool
ppb_view_get_clip_rect(PP_Resource resource, struct PP_Rect* clip);

float
ppb_view_get_device_scale(PP_Resource resource);

float
ppb_view_get_css_scale(PP_Resource resource);


int32_t
ppb_graphics3d_get_attrib_max_value(PP_Resource instance, int32_t attribute, int32_t* value);

PP_Resource
ppb_graphics3d_create(PP_Instance instance, PP_Resource share_context, const int32_t attrib_list[]);

PP_Bool
ppb_graphics3d_is_graphics3d(PP_Resource resource);

int32_t
ppb_graphics3d_get_attribs(PP_Resource context, int32_t attrib_list[]);

int32_t
ppb_graphics3d_set_attribs(PP_Resource context, const int32_t attrib_list[]);

int32_t
ppb_graphics3d_get_error(PP_Resource context);

int32_t
ppb_graphics3d_resize_buffers(PP_Resource context, int32_t width, int32_t height);

int32_t
ppb_graphics3d_swap_buffers(PP_Resource context, struct PP_CompletionCallback callback);


void
ppb_opengles2_ActiveTexture(PP_Resource context, GLenum texture);

void
ppb_opengles2_AttachShader(PP_Resource context, GLuint program, GLuint shader);

void
ppb_opengles2_BindAttribLocation(PP_Resource context, GLuint program, GLuint index,
                                 const char* name);

void
ppb_opengles2_BindBuffer(PP_Resource context, GLenum target, GLuint buffer);

void
ppb_opengles2_BindFramebuffer(PP_Resource context, GLenum target, GLuint framebuffer);

void
ppb_opengles2_BindRenderbuffer(PP_Resource context, GLenum target, GLuint renderbuffer);

void
ppb_opengles2_BindTexture(PP_Resource context, GLenum target, GLuint texture);

void
ppb_opengles2_BlendColor(PP_Resource context, GLclampf red, GLclampf green, GLclampf blue,
                         GLclampf alpha);

void
ppb_opengles2_BlendEquation(PP_Resource context, GLenum mode);

void
ppb_opengles2_BlendEquationSeparate(PP_Resource context, GLenum modeRGB, GLenum modeAlpha);

void
ppb_opengles2_BlendFunc(PP_Resource context, GLenum sfactor, GLenum dfactor);

void
ppb_opengles2_BlendFuncSeparate(PP_Resource context, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha,
                                GLenum dstAlpha);

void
ppb_opengles2_BufferData(PP_Resource context, GLenum target, GLsizeiptr size, const void* data,
                         GLenum usage);

void
ppb_opengles2_BufferSubData(PP_Resource context, GLenum target, GLintptr offset, GLsizeiptr size,
                            const void* data);

GLenum
ppb_opengles2_CheckFramebufferStatus(PP_Resource context, GLenum target);

void
ppb_opengles2_Clear(PP_Resource context, GLbitfield mask);

void
ppb_opengles2_ClearColor(PP_Resource context, GLclampf red, GLclampf green, GLclampf blue,
                         GLclampf alpha);

void
ppb_opengles2_ClearDepthf(PP_Resource context, GLclampf depth);

void
ppb_opengles2_ClearStencil(PP_Resource context, GLint s);

void
ppb_opengles2_ColorMask(PP_Resource context, GLboolean red, GLboolean green, GLboolean blue,
                        GLboolean alpha);

void
ppb_opengles2_CompileShader(PP_Resource context, GLuint shader);

void
ppb_opengles2_CompressedTexImage2D(PP_Resource context, GLenum target, GLint level,
                                   GLenum internalformat, GLsizei width, GLsizei height,
                                   GLint border, GLsizei imageSize, const void* data);

void
ppb_opengles2_CompressedTexSubImage2D(PP_Resource context, GLenum target, GLint level,
                                      GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                      GLenum format, GLsizei imageSize, const void* data);

void
ppb_opengles2_CopyTexImage2D(PP_Resource context, GLenum target, GLint level, GLenum internalformat,
                             GLint x, GLint y, GLsizei width, GLsizei height, GLint border);

void
ppb_opengles2_CopyTexSubImage2D(PP_Resource context, GLenum target, GLint level, GLint xoffset,
                                GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);

GLuint
ppb_opengles2_CreateProgram(PP_Resource context);

GLuint
ppb_opengles2_CreateShader(PP_Resource context, GLenum type);

void
ppb_opengles2_CullFace(PP_Resource context, GLenum mode);

void
ppb_opengles2_DeleteBuffers(PP_Resource context, GLsizei n, const GLuint* buffers);

void
ppb_opengles2_DeleteFramebuffers(PP_Resource context, GLsizei n, const GLuint* framebuffers);

void
ppb_opengles2_DeleteProgram(PP_Resource context, GLuint program);

void
ppb_opengles2_DeleteRenderbuffers(PP_Resource context, GLsizei n, const GLuint* renderbuffers);

void
ppb_opengles2_DeleteShader(PP_Resource context, GLuint shader);

void
ppb_opengles2_DeleteTextures(PP_Resource context, GLsizei n, const GLuint* textures);

void
ppb_opengles2_DepthFunc(PP_Resource context, GLenum func);

void
ppb_opengles2_DepthMask(PP_Resource context, GLboolean flag);

void
ppb_opengles2_DepthRangef(PP_Resource context, GLclampf zNear, GLclampf zFar);

void
ppb_opengles2_DetachShader(PP_Resource context, GLuint program, GLuint shader);

void
ppb_opengles2_Disable(PP_Resource context, GLenum cap);

void
ppb_opengles2_DisableVertexAttribArray(PP_Resource context, GLuint index);

void
ppb_opengles2_DrawArrays(PP_Resource context, GLenum mode, GLint first, GLsizei count);

void
ppb_opengles2_DrawElements(PP_Resource context, GLenum mode, GLsizei count, GLenum type,
                           const void* indices);

void
ppb_opengles2_Enable(PP_Resource context, GLenum cap);

void
ppb_opengles2_EnableVertexAttribArray(PP_Resource context, GLuint index);

void
ppb_opengles2_Finish(PP_Resource context);

void
ppb_opengles2_Flush(PP_Resource context);

void
ppb_opengles2_FramebufferRenderbuffer(PP_Resource context, GLenum target, GLenum attachment,
                                      GLenum renderbuffertarget, GLuint renderbuffer);

void
ppb_opengles2_FramebufferTexture2D(PP_Resource context, GLenum target, GLenum attachment,
                                   GLenum textarget, GLuint texture, GLint level);

void
ppb_opengles2_FrontFace(PP_Resource context, GLenum mode);

void
ppb_opengles2_GenBuffers(PP_Resource context, GLsizei n, GLuint* buffers);

void
ppb_opengles2_GenerateMipmap(PP_Resource context, GLenum target);

void
ppb_opengles2_GenFramebuffers(PP_Resource context, GLsizei n, GLuint* framebuffers);

void
ppb_opengles2_GenRenderbuffers(PP_Resource context, GLsizei n, GLuint* renderbuffers);

void
ppb_opengles2_GenTextures(PP_Resource context, GLsizei n, GLuint* textures);

void
ppb_opengles2_GetActiveAttrib(PP_Resource context, GLuint program, GLuint index, GLsizei bufsize,
                              GLsizei* length, GLint* size, GLenum* type, char* name);

void
ppb_opengles2_GetActiveUniform(PP_Resource context, GLuint program, GLuint index, GLsizei bufsize,
                               GLsizei* length, GLint* size, GLenum* type, char* name);

void
ppb_opengles2_GetAttachedShaders(PP_Resource context, GLuint program, GLsizei maxcount,
                                 GLsizei* count, GLuint* shaders);

GLint
ppb_opengles2_GetAttribLocation(PP_Resource context, GLuint program, const char* name);

void
ppb_opengles2_GetBooleanv(PP_Resource context, GLenum pname, GLboolean* params);

void
ppb_opengles2_GetBufferParameteriv(PP_Resource context, GLenum target, GLenum pname, GLint* params);

GLenum
ppb_opengles2_GetError(PP_Resource context);

void
ppb_opengles2_GetFloatv(PP_Resource context, GLenum pname, GLfloat* params);

void
ppb_opengles2_GetFramebufferAttachmentParameteriv(PP_Resource context, GLenum target,
                                                  GLenum attachment, GLenum pname, GLint* params);

void
ppb_opengles2_GetIntegerv(PP_Resource context, GLenum pname, GLint* params);

void
ppb_opengles2_GetProgramiv(PP_Resource context, GLuint program, GLenum pname, GLint* params);

void
ppb_opengles2_GetProgramInfoLog(PP_Resource context, GLuint program, GLsizei bufsize,
                                GLsizei* length, char* infolog);

void
ppb_opengles2_GetRenderbufferParameteriv(PP_Resource context, GLenum target, GLenum pname,
                                         GLint* params);

void
ppb_opengles2_GetShaderiv(PP_Resource context, GLuint shader, GLenum pname, GLint* params);

void
ppb_opengles2_GetShaderInfoLog(PP_Resource context, GLuint shader, GLsizei bufsize, GLsizei* length,
                               char* infolog);

void
ppb_opengles2_GetShaderPrecisionFormat(PP_Resource context, GLenum shadertype, GLenum precisiontype,
                                       GLint* range, GLint* precision);

void
ppb_opengles2_GetShaderSource(PP_Resource context, GLuint shader, GLsizei bufsize, GLsizei* length,
                              char* source);

const GLubyte*
ppb_opengles2_GetString(PP_Resource context, GLenum name);

void
ppb_opengles2_GetTexParameterfv(PP_Resource context, GLenum target, GLenum pname, GLfloat* params);

void
ppb_opengles2_GetTexParameteriv(PP_Resource context, GLenum target, GLenum pname, GLint* params);

void
ppb_opengles2_GetUniformfv(PP_Resource context, GLuint program, GLint location, GLfloat* params);

void
ppb_opengles2_GetUniformiv(PP_Resource context, GLuint program, GLint location, GLint* params);

GLint
ppb_opengles2_GetUniformLocation(PP_Resource context, GLuint program, const char* name);

void
ppb_opengles2_GetVertexAttribfv(PP_Resource context, GLuint index, GLenum pname, GLfloat* params);

void
ppb_opengles2_GetVertexAttribiv(PP_Resource context, GLuint index, GLenum pname, GLint* params);

void
ppb_opengles2_GetVertexAttribPointerv(PP_Resource context, GLuint index, GLenum pname,
                                      void** pointer);

void
ppb_opengles2_Hint(PP_Resource context, GLenum target, GLenum mode);

GLboolean
ppb_opengles2_IsBuffer(PP_Resource context, GLuint buffer);

GLboolean
ppb_opengles2_IsEnabled(PP_Resource context, GLenum cap);

GLboolean
ppb_opengles2_IsFramebuffer(PP_Resource context, GLuint framebuffer);

GLboolean
ppb_opengles2_IsProgram(PP_Resource context, GLuint program);

GLboolean
ppb_opengles2_IsRenderbuffer(PP_Resource context, GLuint renderbuffer);

GLboolean
ppb_opengles2_IsShader(PP_Resource context, GLuint shader);

GLboolean
ppb_opengles2_IsTexture(PP_Resource context, GLuint texture);

void
ppb_opengles2_LineWidth(PP_Resource context, GLfloat width);

void
ppb_opengles2_LinkProgram(PP_Resource context, GLuint program);

void
ppb_opengles2_PixelStorei(PP_Resource context, GLenum pname, GLint param);

void
ppb_opengles2_PolygonOffset(PP_Resource context, GLfloat factor, GLfloat units);

void
ppb_opengles2_ReadPixels(PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height,
                         GLenum format, GLenum type, void* pixels);

void
ppb_opengles2_ReleaseShaderCompiler(PP_Resource context);

void
ppb_opengles2_RenderbufferStorage(PP_Resource context, GLenum target, GLenum internalformat,
                                  GLsizei width, GLsizei height);

void
ppb_opengles2_SampleCoverage(PP_Resource context, GLclampf value, GLboolean invert);

void
ppb_opengles2_Scissor(PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height);

void
ppb_opengles2_ShaderBinary(PP_Resource context, GLsizei n, const GLuint* shaders,
                           GLenum binaryformat, const void* binary, GLsizei length);

void
ppb_opengles2_ShaderSource(PP_Resource context, GLuint shader, GLsizei count, const char** str,
                           const GLint* length);

void
ppb_opengles2_StencilFunc(PP_Resource context, GLenum func, GLint ref, GLuint mask);

void
ppb_opengles2_StencilFuncSeparate(PP_Resource context, GLenum face, GLenum func, GLint ref,
                                  GLuint mask);

void
ppb_opengles2_StencilMask(PP_Resource context, GLuint mask);

void
ppb_opengles2_StencilMaskSeparate(PP_Resource context, GLenum face, GLuint mask);

void
ppb_opengles2_StencilOp(PP_Resource context, GLenum fail, GLenum zfail, GLenum zpass);

void
ppb_opengles2_StencilOpSeparate(PP_Resource context, GLenum face, GLenum fail, GLenum zfail,
                                GLenum zpass);

void
ppb_opengles2_TexImage2D(PP_Resource context, GLenum target, GLint level, GLint internalformat,
                         GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type,
                         const void* pixels);

void
ppb_opengles2_TexParameterf(PP_Resource context, GLenum target, GLenum pname, GLfloat param);

void
ppb_opengles2_TexParameterfv(PP_Resource context, GLenum target, GLenum pname,
                             const GLfloat* params);

void
ppb_opengles2_TexParameteri(PP_Resource context, GLenum target, GLenum pname, GLint param);

void
ppb_opengles2_TexParameteriv(PP_Resource context, GLenum target, GLenum pname, const GLint* params);

void
ppb_opengles2_TexSubImage2D(PP_Resource context, GLenum target, GLint level, GLint xoffset,
                            GLint yoffset, GLsizei width, GLsizei height, GLenum format,
                            GLenum type, const void* pixels);

void
ppb_opengles2_Uniform1f(PP_Resource context, GLint location, GLfloat x);

void
ppb_opengles2_Uniform1fv(PP_Resource context, GLint location, GLsizei count, const GLfloat* v);

void
ppb_opengles2_Uniform1i(PP_Resource context, GLint location, GLint x);

void
ppb_opengles2_Uniform1iv(PP_Resource context, GLint location, GLsizei count, const GLint* v);

void
ppb_opengles2_Uniform2f(PP_Resource context, GLint location, GLfloat x, GLfloat y);

void
ppb_opengles2_Uniform2fv(PP_Resource context, GLint location, GLsizei count, const GLfloat* v);

void
ppb_opengles2_Uniform2i(PP_Resource context, GLint location, GLint x, GLint y);

void
ppb_opengles2_Uniform2iv(PP_Resource context, GLint location, GLsizei count, const GLint* v);

void
ppb_opengles2_Uniform3f(PP_Resource context, GLint location, GLfloat x, GLfloat y, GLfloat z);

void
ppb_opengles2_Uniform3fv(PP_Resource context, GLint location, GLsizei count, const GLfloat* v);

void
ppb_opengles2_Uniform3i(PP_Resource context, GLint location, GLint x, GLint y, GLint z);

void
ppb_opengles2_Uniform3iv(PP_Resource context, GLint location, GLsizei count, const GLint* v);

void
ppb_opengles2_Uniform4f(PP_Resource context, GLint location, GLfloat x, GLfloat y, GLfloat z,
                        GLfloat w);

void
ppb_opengles2_Uniform4fv(PP_Resource context, GLint location, GLsizei count, const GLfloat* v);

void
ppb_opengles2_Uniform4i(PP_Resource context, GLint location, GLint x, GLint y, GLint z, GLint w);

void
ppb_opengles2_Uniform4iv(PP_Resource context, GLint location, GLsizei count, const GLint* v);

void
ppb_opengles2_UniformMatrix2fv(PP_Resource context, GLint location, GLsizei count,
                               GLboolean transpose, const GLfloat* value);

void
ppb_opengles2_UniformMatrix3fv(PP_Resource context, GLint location, GLsizei count,
                               GLboolean transpose, const GLfloat* value);

void
ppb_opengles2_UniformMatrix4fv(PP_Resource context, GLint location, GLsizei count,
                               GLboolean transpose, const GLfloat* value);

void
ppb_opengles2_UseProgram(PP_Resource context, GLuint program);

void
ppb_opengles2_ValidateProgram(PP_Resource context, GLuint program);

void
ppb_opengles2_VertexAttrib1f(PP_Resource context, GLuint indx, GLfloat x);

void
ppb_opengles2_VertexAttrib1fv(PP_Resource context, GLuint indx, const GLfloat* values);

void
ppb_opengles2_VertexAttrib2f(PP_Resource context, GLuint indx, GLfloat x, GLfloat y);

void
ppb_opengles2_VertexAttrib2fv(PP_Resource context, GLuint indx, const GLfloat* values);

void
ppb_opengles2_VertexAttrib3f(PP_Resource context, GLuint indx, GLfloat x, GLfloat y, GLfloat z);

void
ppb_opengles2_VertexAttrib3fv(PP_Resource context, GLuint indx, const GLfloat* values);

void
ppb_opengles2_VertexAttrib4f(PP_Resource context, GLuint indx, GLfloat x, GLfloat y, GLfloat z,
                             GLfloat w);

void
ppb_opengles2_VertexAttrib4fv(PP_Resource context, GLuint indx, const GLfloat* values);

void
ppb_opengles2_VertexAttribPointer(PP_Resource context, GLuint indx, GLint size, GLenum type,
                                  GLboolean normalized, GLsizei stride, const void* ptr);

void
ppb_opengles2_Viewport(PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height);


#endif // FPP__INTERFACE_LIST_H

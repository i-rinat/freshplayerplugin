#ifndef FPP__PPB_FLASH_MESSAGE_LOOP_H
#define FPP__PPB_FLASH_MESSAGE_LOOP_H

#include <ppapi/c/private/ppb_flash_message_loop.h>


PP_Resource
ppb_flash_message_loop_create(PP_Instance instance);

void
ppb_flash_message_loop_destroy(void *p);

PP_Bool
ppb_flash_message_loop_is_flash_message_loop(PP_Resource resource);

int32_t
ppb_flash_message_loop_run(PP_Resource flash_message_loop);

void
ppb_flash_message_loop_quit(PP_Resource flash_message_loop);

#endif // FPP__PPB_FLASH_MESSAGE_LOOP_H

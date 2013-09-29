#include <ppapi/c/ppb_audio.h>
#include <stddef.h>


const struct PPB_Audio_1_0 ppb_audio_interface_1_0 = {
    .Create = NULL,
    .IsAudio = NULL,
    .GetCurrentConfig = NULL,
    .StartPlayback = NULL,
    .StopPlayback = NULL
};

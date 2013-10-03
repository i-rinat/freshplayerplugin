#include <ppapi/c/ppb_audio.h>
#include <stddef.h>


const struct PPB_Audio_1_0 ppb_audio_interface_1_0 = {
    .Create = (void *)290,
    .IsAudio = (void *)291,
    .GetCurrentConfig = (void *)292,
    .StartPlayback = (void *)293,
    .StopPlayback = (void *)294,
};

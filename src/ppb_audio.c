#include <ppapi/c/ppb_audio.h>
#include <stddef.h>


const PPB_Audio ppb_audio = {
    .Create = NULL,
    .IsAudio = NULL,
    .GetCurrentConfig = NULL,
    .StartPlayback = NULL,
    .StopPlayback = NULL
};

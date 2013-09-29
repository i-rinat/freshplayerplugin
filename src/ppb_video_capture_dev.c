#include <ppapi/c/dev/ppb_video_capture_dev.h>
#include <stddef.h>


const PPB_VideoCapture_Dev ppb_video_capture_dev = {
    .Create = NULL,
    .IsVideoCapture = NULL,
    .EnumerateDevices = NULL,
    .MonitorDeviceChange = NULL,
    .Open = NULL,
    .StartCapture = NULL,
    .ReuseBuffer = NULL,
    .StopCapture = NULL,
    .Close = NULL
};

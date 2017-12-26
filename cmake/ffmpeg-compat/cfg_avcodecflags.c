#include <libavcodec/avcodec.h>
int main(void) {
    return AV_CODEC_FLAG_TRUNCATED + AV_CODEC_CAP_TRUNCATED;
}

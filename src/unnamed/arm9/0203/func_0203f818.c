#include "nitro/wm/wm_api.h"

extern int WM_MeasureChannel(void *a, int b, int c, int d, unsigned short e);
extern void func_0203f920(void *arg);

/* Measure the next channel from `arg` on that the allowed-channel mask
   (WM_GetAllowedChannel, read from the WM status) permits: 1 when none is left
   up to channel 14, 2 when the measure request (WM_MeasureChannel: WM command
   0x1e, the SDK's WM_APIID_MEASURE_CHANNEL, with ccaMode 3, edThreshold
   0x11 and a measure time of 0x1e) is refused, else 0. */
int func_0203f818(int arg) {
    unsigned int n = arg;
    unsigned int mask = WM_GetAllowedChannel();
    while (((1 << (n - 1)) & mask) == 0) {
        n = (unsigned short)(n + 1);
        if (n > 0xe)
            return 1;
    }
    if (WM_MeasureChannel((void *)func_0203f920, 3, 0x11, n, 0x1e) != 2)
        return 2;
    return 0;
}

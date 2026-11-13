#include "nitro/spi/tp.h"
#include "nitro/os/system.h"

extern TPCalibrateParam data_020a0de0;
extern volatile TPData data_020a0df8[];

/* Touch-panel setup at boot (func_0201a054): TP_Init, TP_GetUserInfo and
   TP_SetCalibrateParam on the calibration data_020a0de0, then the SDK's
   synchronous TP_RequestSetStability(3, 30) and TP_RequestAutoSamplingStart
   at V-count 0, four samples a frame, into the nine-entry ring
   data_020a0df8 that func_0203b9bc reads. An error from either crashes. */
void func_0203bbc0(void)
{
    TP_Init();
    TP_GetUserInfo(&data_020a0de0);
    TP_SetCalibrateParam(&data_020a0de0);
    if (TP_RequestSetStability(3, 30) != 0)
        _Z12OS_Terminatev();
    if (TP_RequestAutoSamplingStart(0, 4, (TPData *)data_020a0df8, 9) != 0)
        _Z12OS_Terminatev();
}

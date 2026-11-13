#include "types.h"

/* NitroSDK SND_util.c SND_CalcChannelVolume, as in pret's pokediamond: turn
 * decibels (-723..0) into a channel volume, the volume byte low and the
 * data shift (0..3) above it. */
extern const u8 data_02086484[724]; /* sVolumeTable */

u16 SND_CalcChannelVolume(s32 x) {
    u8 resultLo;
    int resultHi;
    if (x < -723)
        x = -723;
    else if (x > 0)
        x = 0;
    resultLo = data_02086484[x + 723];
    if (x < -240)
        resultHi = 3;
    else if (x < -120)
        resultHi = 2;
    else if (x < -60)
        resultHi = 1;
    else
        resultHi = 0;
    return (resultLo | (resultHi << 8));
}

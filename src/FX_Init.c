/* NitroSDK fx.c (pret pokediamond arm9/lib/NitroSDK/src/FX.c): the
 * fixed-point library's init hook, empty on this SDK. The boot sequence calls
 * it just before GX_Init. FX_Modf, defined below it in the SDK file, is
 * dead-stripped here. */

#include "nitro/fx/fx.h"

void FX_Init(void) {
    return;
}

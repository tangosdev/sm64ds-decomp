//cpp
/* dScene_c::SetSceneToSpawn(u32, u32) at 0x0202e36c, 0x2c bytes.
 *
 * Static: the mangled name says two u32 parameters, and the ROM reads them from r0
 * and r1. A non-static member would put them in r1 and r2 behind a `this`.
 *
 * Records the scene to switch to and reports whether it changed anything -- the
 * caller only pays for the fade if the answer is yes. */
#include "dScene_c.h"

extern "C" {
extern u16 data_02092664;   /* pending scene ID */
extern u32 data_0209f5b8;   /* its parameter word */
}

int dScene_c::SetSceneToSpawn(u32 sceneID, u32 param)
{
    if (sceneID != data_02092664) {
        data_02092664 = sceneID;
        data_0209f5b8 = param;
        return 1;
    }
    return 0;
}

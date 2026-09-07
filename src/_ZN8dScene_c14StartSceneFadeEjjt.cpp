//cpp
/* dScene_c::StartSceneFade(u32, u32, u16) at 0x0202e348, 0x24 bytes.
 *
 * Static; r0/r1/r2 carry the three declared parameters. Queues the scene change and,
 * only if that changed anything, sets the colour the fader will fade through.
 *
 * The `t` in the mangled name is `unsigned short`, and it is load-bearing: the
 * store is `strh`, so the field at FaderColor+0xc is two bytes wide. */
#include "dScene_c.h"
#include "FaderColor.h"

extern "C" {
extern FaderColor data_0209f5e8;
}

void dScene_c::StartSceneFade(u32 sceneID, u32 param, u16 fadeColor)
{
    if (SetSceneToSpawn(sceneID, param))
        data_0209f5e8.color = fadeColor;
}

//cpp
#include "types.h"
#include "nitro/gx/g3x.h"
#include "nitro/gx/g3imm_cmd.h"

void func_ov007_020b6c54(void) {
    G3X_AntiAlias(true);
    G3X_AlphaBlend(true);
    G3X_SetShading(GX_SHADING_TOON);
    G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
    G3X_SetClearColor(0, 0, 0x7fff, 0x3f, 0);
}

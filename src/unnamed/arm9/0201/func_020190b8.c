#include "types.h"
#include "nitro/gx/g3imm_cmd.h"

extern int func_02019018(void);
extern unsigned char data_0209d464;

/* Ask for this frame's 3D buffer swap, if the 3D engines are on
   (func_02019018): manual translucent sort, W-buffering, and raise
   data_0209d464. func_02019100 clears the flag at the end of the VBlank IRQ;
   dScMgD3DBase_c::OnGraphCallback2 waits on it. */
void func_020190b8(void) {
    if (func_02019018() == 0)
        return;
    G3_SwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_W);
    data_0209d464 = 1;
}

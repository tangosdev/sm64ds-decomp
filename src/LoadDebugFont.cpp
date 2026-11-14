//cpp
#include "types.h"
#include "nitro/gx/gx_load.h"

extern "C" {

extern void func_02023408(void);
}

extern u8 data_0208a178[];
extern const void *data_0208c178;

void LoadDebugFont(void) {
    GX_LoadBG1Char(data_0208a178, 0, 0x2000);
    GX_LoadBGPltt(&data_0208c178, 0x1a0, 0x60);
    func_02023408();
}

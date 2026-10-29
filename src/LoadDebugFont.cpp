//cpp
#include "types.h"
#include "GX.h"

extern "C" {

extern void func_02023408(void);

extern const void *data_0208a178;
extern const void *data_0208c178;

void LoadDebugFont(void) {
    GX::LoadBG1Char(&data_0208a178, 0, 0x2000);
    GX::LoadBGPltt(&data_0208c178, 0x1a0, 0x60);
    func_02023408();
}
}

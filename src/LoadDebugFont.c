#include "types.h"
extern void _ZN2GX11LoadBG1CharEPKvjj(const void *src, u32 offset, u32 size);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *src, u32 offset, u32 size);
extern void func_02023408(void);

extern const void *data_0208a178;
extern const void *data_0208c178;

void LoadDebugFont(void)
{
    _ZN2GX11LoadBG1CharEPKvjj(&data_0208a178, 0, 0x2000);
    _ZN2GX10LoadBGPlttEPKvjj(&data_0208c178, 0x1a0, 0x60);
    func_02023408();
}

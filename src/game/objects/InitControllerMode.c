#include "types.h"
struct Ctrl { u8 mode; char pad[0x17]; };   /* stride 0x18 */
struct Obj42 { char pad[0x42]; u8 f42; };

extern struct Ctrl data_0209f4ae[];
extern u8 data_020a0e40;
extern struct Obj42 data_0209caa0;

void InitControllerMode(int idx, u8 val)
{
    data_0209f4ae[idx].mode = val;
    if (idx == data_020a0e40)
        data_0209caa0.f42 = val;
}

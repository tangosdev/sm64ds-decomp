#include "types.h"
extern int SublevelToLevel(int i);
extern signed char data_02092110;
extern u8 data_0209f1f0;
extern u8 data_0209f2d8;

void SetNextStar(void)
{
    int level = SublevelToLevel(data_02092110);
    int b;
    if (data_0209f1f0 == 1) return;
    b = (int)(data_0209f2d8 == 1);
    if (b != 0) return;
    if (level != -1) {
        if (level <= 0xe) return;
        if (level == 0x13) return;
    }
    data_0209f1f0 = 1;
}

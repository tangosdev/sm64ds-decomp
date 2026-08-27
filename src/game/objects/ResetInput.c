#include "types.h"
extern u8 GetControllerMode(int a0);
extern void func_0205a588(void *a, int b, int c);
extern u8 data_0209f21c;

struct Ctrl {
    char pad[0x12];
    u8 f12;     /* 0x12 */
    char pad2[0x16 - 0x13];
    u8 f16;     /* 0x16 */
    char pad3[0x18 - 0x17];
};

extern struct Ctrl data_0209f498[];
extern u8 data_0209f2d8;

void ResetInput(void)
{
    struct Ctrl *p = data_0209f498;
    int i;
    for (i = 0; i < data_0209f21c; i++) {
        u8 mode;
        int m = (int)(data_0209f2d8 == 1);
        if (m == 0)
            mode = p->f16;
        else
            mode = GetControllerMode(i);
        func_0205a588(p, 0, 0x18);
        p->f16 = mode;
        p->f12 = 5;
        p++;
    }
}

#include "PlayerInput.h"
extern int func_0203da9c(void);

int TouchArea_Update(char *c, int a1)
{
    int ret;
    int flag;
    int dx, dy;
    int v;

    if (*(unsigned char *)(c + 0x11) == 0)
        return 0;
    if (a1 < 0)
        a1 = func_0203da9c();
    ret = 0;
    flag = 0;
    if (gTouchHeld[a1 * 4] != 0) {
        int i4 = a1 * 4;
        dx = gTouchX[i4] - *(short *)(c + 4);
        dy = gTouchY[i4] - *(short *)(c + 6);
        if (dx >= -*(short *)(c + 8) && dx <= *(short *)(c + 8)
            && dy >= -*(short *)(c + 0xa) && dy <= *(short *)(c + 0xa))
            flag = 1;
    }
    if (flag != 0) {
        v = *(int *)(c + 0x14);
        if ((v == 1 && gTouchEdge[(unsigned int)a1 * 4] != 0) || (v == 2 && *(unsigned char *)(c + 0x12) == 0))
            ret = 1;
    }
    *(unsigned char *)(c + 0x12) = (unsigned char)flag;
    return ret;
}

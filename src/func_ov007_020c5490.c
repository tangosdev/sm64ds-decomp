#include "types.h"
void func_ov007_020c5490(char* obj, int* m)
{
    int i;
    int j;
    int k;
    int* row;
    int b7c, b80;
    int* e;
    int x, y;
    int dx, dy;

    i = 0;
    if (*(int*)(obj + 8) <= 0)
        return;

    do {
        j = 0;
        if ((int)*(unsigned short*)((*(char***)(obj + 0x38))[i] + 8) > 0) {
            k = 0;
            do {
                row = (*(int***)(obj + 0x2c))[i];
                e = (int*)((char*)row + k);
                b7c = *(int*)(obj + 0x7c);
                x = e[0];
                b80 = *(int*)(obj + 0x80);
                y = e[1];
                dx = x - b7c;
                dy = y - b80;
                *(int*)((char*)row + k) =
                    ((int)(((s64)m[0] * dx + 0x800) >> 12)
                     + (int)(((s64)m[1] * dy + 0x800) >> 12)) + b7c;
                j++;
                e[1] =
                    ((int)(((s64)m[2] * dx + 0x800) >> 12)
                     + (int)(((s64)m[3] * dy + 0x800) >> 12)) + *(int*)(obj + 0x80);
                k += 0xc;
            } while (j < (int)*(unsigned short*)((*(char***)(obj + 0x38))[i] + 8));
        }
        i++;
    } while (i < *(int*)(obj + 8));
}

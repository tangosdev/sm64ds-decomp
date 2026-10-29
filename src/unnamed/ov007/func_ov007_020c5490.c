#include "types.h"
#include "nitro/fx/fx.h"

/* A committed stroke; names as in include/Ov007StrokePath.h, only the count. */
typedef struct Ov007StrokePath {
    char pad_00[8];
    unsigned short pointCount; /* 0x08 */
} Ov007StrokePath;

/* The 0xd0-byte touch picker func_ov007_020c690c builds; names as in that
 * file and func_ov007_020c684c, only the slots this function touches. */
typedef struct Ov007Picker {
    char pad_00[8];
    int groupCount; /* 0x08 */
    char pad_0c[0x20];
    int **groupPointCopies; /* 0x2c */
    char pad_30[8];
    Ov007StrokePath **groups; /* 0x38 */
    char pad_3c[0x40];
    Vector3 restPos; /* 0x7c */
} Ov007Picker;

/* Applies the 2x2 Q12 matrix m to every rest point's x/y about restPos. */
void func_ov007_020c5490(char* obj, int* m)
{
    Ov007Picker *self = (Ov007Picker *)obj;
    int i;
    int j;
    int k;
    int* row;
    int restX, restY;
    int* e;
    int x, y;
    int dx, dy;

    i = 0;
    if (self->groupCount <= 0)
        return;

    do {
        j = 0;
        if ((int)self->groups[i]->pointCount > 0) {
            k = 0;
            do {
                row = self->groupPointCopies[i];
                e = (int*)((char*)row + k);
                restX = self->restPos.x;
                x = e[0];
                restY = self->restPos.y;
                y = e[1];
                dx = x - restX;
                dy = y - restY;
                *(int *)((char *)row + k) = (FX_MUL(m[0], dx) + FX_MUL(m[1], dy)) + restX;
                j++;
                e[1] = (FX_MUL(m[2], dx) + FX_MUL(m[3], dy)) + self->restPos.y;
                k += 0xc;
            } while (j < (int)self->groups[i]->pointCount);
        }
        i++;
    } while (i < self->groupCount);
}

#include "types.h"

#define LAUNDER(p) (p)

/* AddVec3 still defines its vector as its own struct Vec3;
   declared as defined, the call casts. */
struct Vec3;
extern void AddVec3(struct Vec3 *a, struct Vec3 *b, struct Vec3 *c);

/* The 0xd0-byte touch picker func_ov007_020c690c builds; names as in
 * func_ov007_020c5a6c and func_ov007_020c684c, only the slots this function
 * touches. */
typedef struct Ov007Picker {
    int pad_00;
    int mode; /* 0x04: 0 move, 1 drag, 2 link (func_ov007_020c5a6c) */
    u8 pad_08[0x34];
    int **pointLists; /* 0x3c: per group (func_ov007_020c798c) */
    u8 pad_40[0x18];
    int hitGroup;      /* 0x58: picked group, -1 = none */
    int hitIndex;      /* 0x5c: picked point in that group, -1 = none */
    Vector3 hitOffset; /* 0x60 */
    int pad_6c;
    Vector3 pos; /* 0x70 */
    u8 pad_7c[0x1c];
    Vector3 anchorPos; /* 0x98 */
    u8 pad_a4[0x14];
    /* 0xb8/0xc4: the clamp rectangle (func_ov007_020c510c); mode 1 keeps
     * pos inside it on x and y */
    int boundsMin[2];
    int pad_c0;
    int boundsMax[2];
} Ov007Picker;

void func_ov007_020c5948(char *self, Vector3 *src) {
    Ov007Picker *s = (Ov007Picker *)self;
    int i;
    int j;
    int **pointLists;
    int *obj;
    int *vec;
    int mode;
    int *dst;
    int *min;
    int *max;

    i = s->hitGroup;
    if (i == -1)
        return;
    j = s->hitIndex;
    if (j == -1)
        return;

    pointLists = s->pointLists;
    obj = (*(int ***)pointLists[i])[j];
    vec = (int *)obj[0];

    obj[6] = 0;
    obj[5] = obj[6];
    obj[4] = obj[5];
    obj[3] = obj[4];
    obj[2] = obj[3];
    obj[1] = obj[2];

    vec[0] = src->x;
    vec[1] = src->y;

    mode = s->mode;
    switch (mode) {
    case 0:
        s->pos = *src;
        break;
    case 1:
        dst = (int *)(int)LAUNDER(&s->pos);
        min = (int *)(int)LAUNDER(s->boundsMin);
        max = (int *)(int)LAUNDER(s->boundsMax);
        AddVec3((struct Vec3 *)src, (struct Vec3 *)&s->hitOffset, (struct Vec3 *)dst);
        if (min[0] > dst[0])
            dst[0] = min[0];
        if (max[0] < dst[0])
            dst[0] = max[0];
        if (min[1] > dst[1])
            dst[1] = min[1];
        if (max[1] < dst[1])
            dst[1] = max[1];
        break;
    case 2:
        s->anchorPos = *src;
        break;
    }
}

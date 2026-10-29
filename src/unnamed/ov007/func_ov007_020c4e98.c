#include "Ov007StrokePath.h"

void func_ov007_020c4fcc(int r0, int r1);
void func_ov007_020c19cc(int r0, int r1, int r2, int r3);

/* The 0xd0-byte touch picker func_ov007_020c690c builds; names as in that
 * file and func_ov007_020c4dfc, only the slots this function reads. */
typedef struct Ov007Picker {
    char pad_00[8];
    int groupCount; /* 0x08 */
    char pad_0c[0x14];
    Ov007StrokePath *path; /* 0x20: the stroke being drawn (func_ov007_020c2c68) */
    Vector3 *pathPoints;   /* 0x24: its points (func_ov007_020c65bc) */
    Vector3 **groupPoints; /* 0x28: func_ov007_020c6e68, per group */
    char pad_2c[0xc];
    Ov007StrokePath **groups; /* 0x38: committed strokes */
} Ov007Picker;

typedef char Ov007Picker_size_must_be_0x3c[sizeof(Ov007Picker) == 0x3c ? 1 : -1];

/* Draws every committed stroke but group idx, and the stroke being drawn,
 * in full under the first style (func_ov007_020c4fcc(a2, a3)); then group
 * idx on top under the second (a5, a6). thiz keeps the int * spelling of
 * the definition; `self` is the typed view. */
void func_ov007_020c4e98(int *thiz, int a1, int a2, int a3, int idx,
                         unsigned short a5, int a6)
{
    Ov007Picker *self = (Ov007Picker *)thiz;
    int i;
    Vector3 **base;
    base = self->groupPoints;
    func_ov007_020c4fcc(a2, a3);
    for (i = 0; i < self->groupCount; i++) {
        if (i == idx) continue;
        func_ov007_020c19cc(self->groups[i]->pointCount, (int)base[i], a1, 0x1000);
    }
    func_ov007_020c19cc(self->path->pointCount, (int)self->pathPoints, a1, 0x1000);
    func_ov007_020c4fcc(a5, a6);
    func_ov007_020c19cc(self->groups[idx]->pointCount, (int)base[idx], a1, 0x1000);
}

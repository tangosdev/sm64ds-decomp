#include "Ov007StrokePath.h"
#include "Ov007Picker.h"

void func_ov007_020c4fcc(int r0, int r1);
void func_ov007_020c19cc(int count, Vector3 *pts, int half, int t);

/* Draws every committed stroke but group idx, and the stroke being drawn,
 * in full under the first style (func_ov007_020c4fcc(a2, a3)); then group
 * idx on top under the second (a5, a6). */
void func_ov007_020c4e98(Ov007Picker *self, int a1, int a2, int a3, int idx, unsigned short a5,
                         int a6) {
    int i;
    Vector3 **base;
    base = self->groupPoints;
    func_ov007_020c4fcc(a2, a3);
    for (i = 0; i < self->groupCount; i++) {
        if (i == idx) continue;
        func_ov007_020c19cc(self->groups[i]->pointCount, base[i], a1, 0x1000);
    }
    func_ov007_020c19cc(self->path->pointCount, self->pathPoints, a1, 0x1000);
    func_ov007_020c4fcc(a5, a6);
    func_ov007_020c19cc(self->groups[idx]->pointCount, base[idx], a1, 0x1000);
}

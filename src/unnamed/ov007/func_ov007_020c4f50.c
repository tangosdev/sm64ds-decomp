#include "Ov007Picker.h"

extern void func_ov007_020c4fcc(int r0, int r1);
/* The definition takes half as s16; declared that way, the call does not match. */
extern void func_ov007_020c19cc(int count, Vector3 *pts, int half, int t);

/* Draws every committed stroke and then the stroke being drawn, each in
 * full, through func_ov007_020c19cc. Both callers (func_ov007_020b91b4) pass
 * a fifth argument, 1, on the stack; this body never reads it. */
void func_ov007_020c4f50(Ov007Picker *self, int half, int r2, int r3, int unused) {
    int i;
    Vector3 **points = self->groupPoints;
    func_ov007_020c4fcc(r2, r3);
    for (i = 0; i < self->groupCount; i++) {
        func_ov007_020c19cc(self->groups[i]->pointCount, points[i], half, 0x1000);
    }
    func_ov007_020c19cc(self->path->pointCount, self->pathPoints, half, 0x1000);
}

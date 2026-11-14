#include "Ov007StrokePath.h"
#include "Ov007Picker.h"

extern Ov007StrokePath *func_ov007_020c2a04(Ov007StrokePath *src, int start, int end);
extern void func_ov007_020c281c(Ov007StrokePath *path, int minTurn);
extern void func_ov007_020c27b4(Ov007StrokePath *path);
extern void func_ov007_020c26fc(Ov007StrokePath *path);

/* Commits the stroke being drawn as group idx: frees what the slot held,
 * copies the path into it, and unless mergeAngle is negative straightens it
 * and recomputes its extrema before building the group's points
 * (func_ov007_020c6e68). */
void func_ov007_020c6d20(Ov007Picker *self, int idx) {
    Ov007StrokePath *src;
    Ov007StrokePath *d;
    int check;

    func_ov007_020c6d90(self, idx);

    src = self->path;
    self->groups[idx] = func_ov007_020c2a04(src, 0, src->pointCount - 1);

    d = self->groups[idx];
    check = self->params->mergeAngle;
    if (check >= 0) {
        func_ov007_020c281c(d, check);
        func_ov007_020c27b4(d);
        func_ov007_020c26fc(d);
    }

    func_ov007_020c6e68(self, idx);
}

#include "Ov007StrokePath.h"
#include "Ov007Picker.h"

extern void func_ov007_020c28ac(void *p);

/* Commits the stroke being drawn as the next group (func_ov007_020c6d20) and
 * clears the path. Returns 0 when there is nothing to commit or no free
 * group, 2 when this commit used the last group, else 1. */
int func_ov007_020c6550(Ov007Picker *self) {
    int status = 1;
    Ov007StrokePath *obj = self->path;
    if (obj->pointCount == 0 || self->groupCount >= self->maxGroups) {
        status = 0;
    } else {
        func_ov007_020c6d20(self, self->groupCount);
        self->groupCount++;
        if (self->groupCount >= self->maxGroups)
            status = 2;
        func_ov007_020c28ac(obj);
    }
    return status;
}

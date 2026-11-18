#include "Ov007Picker.h"
#include "Ov007Camera.h"

extern void func_ov007_020bfe4c(Ov007Camera *o, int x, int y, int z, VecFx32 *out);

/* Re-unprojects every committed stroke point through the camera, resets the
   live points to it, and puts pos back at its rest position. */
void func_ov007_020c5dec(Ov007Picker *c) {
    int i;
    for (i = 0; i < c->groupCount; i++) {
        int j;
        for (j = 0; j < c->groups[i]->pointCount; j++) {
            Ov007Camera *o = c->camera;
            Ov007StrokePath *group = c->groups[i];
            func_ov007_020bfe4c(o, group->xCoords[j], group->yCoords[j], -o->distance,
                                &c->groupPointCopies[i][j]);
            c->groupPoints[i][j] = c->groupPointCopies[i][j];
        }
    }
    c->pos = c->restPos;
    c->hitGroup = c->hitIndex = -1;
    c->hitOffset.x = c->hitOffset.y = c->hitOffset.z = 0;
}

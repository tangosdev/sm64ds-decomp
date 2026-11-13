//cpp
#include "Ov007Picker.h"
#include "Ov007Camera.h"

void func_ov007_020bfe4c(Ov007Camera *o, int x, int y, int z, void *out);

/* Vector3 declares a constructor and destructor under C++, which makes its
   assignment copy word by word. The ROM copies these with one ldm/stm, which
   C++ keeps for a struct with an array member, so the copies go through it. */
struct Vec3Words {
    int w[3];
};

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
            *(Vec3Words *)&c->groupPoints[i][j] = *(Vec3Words *)&c->groupPointCopies[i][j];
        }
    }
    *(Vec3Words *)&c->pos = *(Vec3Words *)&c->restPos;
    c->hitGroup = c->hitIndex = -1;
    c->hitOffset.x = c->hitOffset.y = c->hitOffset.z = 0;
}

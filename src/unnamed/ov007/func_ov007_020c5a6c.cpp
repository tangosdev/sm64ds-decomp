//cpp
#include "Ov007Picker.h"
#include "math/CalcLib.h"

/* Vector3 declares a constructor and destructor under C++, which makes its
   assignment copy word by word. The ROM copies these with one ldm/stm, which
   C++ keeps for a struct with an array member, so the copies go through it. */
struct Vec3Words {
    int w[3];
};

extern "C" {
int func_ov007_020c30ac(void *arr, int count, void *target, int arg4, int *out);
}

/* Picks the stroke point nearest target over every group (with flag set,
   only points before limitGroup/limitIndex). Mode 0 then moves pos onto it,
   mode 1 keeps the offset from it to pos, and mode 2 moves anchorPos onto
   it, keeping pos's z, and re-measures `link`. */
extern "C" void func_ov007_020c5a6c(Ov007Picker *self, void *target, int flag) {
    self->hitGroup = self->hitIndex = -1;

    int i;
    int carry = -1;
    int out;
    for (i = 0; i < self->groupCount; i++) {
        int best = func_ov007_020c30ac(self->groupPoints[i], self->groups[i]->pointCount, target,
                                       carry, &out);
        /* The ROM tests flag twice (cmp r7,#0 / beq twice), so the
           redundant `flag != 0` stays. */
        if (best != -1 &&
            (flag == 0 || (flag != 0 && (i < self->limitGroup ||
                                         (i == self->limitGroup && best < self->limitIndex))))) {
            self->hitGroup = i;
            self->hitIndex = best;
            carry = out;
        }
    }

    if (self->hitGroup == -1 || self->hitIndex == -1)
        return;

    Ov007Follower *hit = self->pointLists[self->hitGroup]->items[self->hitIndex];

    switch (self->mode) {
    case 0:
        *(Vec3Words *)&self->pos = *(Vec3Words *)hit->target;
        func_ov007_020c64c4(self, 1);
        break;
    case 1:
        VEC_Subtract(&self->pos, hit->target, &self->hitOffset);
        break;
    case 2: {
        self->pos.z = self->anchorPos.z;
        *(Vec3Words *)&self->anchorPos = *(Vec3Words *)hit->target;
        Ov007Link *link = self->link;
        VEC_Subtract(link->from->target, link->to->target, &link->dir);
        link->length = VEC_Mag(&link->dir);
        break;
    }
    }
}

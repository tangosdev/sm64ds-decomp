#include "Ov007Picker.h"

extern void func_ov007_020c4fcc(int color, int alpha);
extern void func_ov007_020c19cc(int count, VecFx32 *pts, int half, int t);

/* Draws the committed strokes up to the draw limit through
 * func_ov007_020c19cc, under the style func_ov007_020c4fcc(color, alpha)
 * sets: each whole group before limitGroup at full progress, then
 * limitGroup itself cut at limitIndex points and limitFraction.
 * The loop test reads limitGroup through an `int *` cast: plain, mwcc
 * reuses the loop's last load for the test after it and the function
 * compiles 4 bytes short (a while, a guarded do-while and a local copy of
 * limitGroup and groupCount do the same). Re-measured (#821 r17):
 * limitGroup re-read into a local after the loop, or the group's points
 * named in the body, are still 0x98 against 0x9c; the tail as a static
 * inline is 0x78. Re-measured (#821 r20): `limitGroup > i`, the tail test
 * as `groupCount > limitGroup`, an early return on `limitGroup >=
 * groupCount`, and an unsigned i are all 0x98. A `volatile` qualifier on
 * the cast changes nothing. */
void func_ov007_020c4dfc(Ov007Picker *self, int half, int color, int alpha) {
    int i;

    func_ov007_020c4fcc(color, alpha);
    for (i = 0; i < *(int *)&self->limitGroup; i++) {
        func_ov007_020c19cc(self->groups[i]->pointCount, self->groupPoints[i], half, 0x1000);
    }
    if (self->limitGroup < self->groupCount) {
        func_ov007_020c19cc(self->limitIndex, self->groupPoints[self->limitGroup], half,
                            self->limitFraction);
    }
}

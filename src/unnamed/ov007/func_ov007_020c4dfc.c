#include "Ov007Picker.h"

extern void func_ov007_020c4fcc(int color, int alpha);
extern void func_ov007_020c19cc(int count, VecFx32 *pts, int half, int t);

/* The group the draw limit stops in. Read through this accessor, the
 * loop test loads limitGroup on every pass and the tail test loads it
 * again, as the cartridge does. Measured: a plain read is 0x98 of 0x9c
 * (mwcc reuses the loop's last load for the tail test); so are a while,
 * a guarded do-while, local copies, `limitGroup > i`, the tail test
 * reversed, an early return and an unsigned i (r17, r20). */
static inline int GetLimitGroup(const Ov007Picker *picker) {
    return picker->limitGroup;
}

/* Draws the committed strokes up to the draw limit through
 * func_ov007_020c19cc, under the style func_ov007_020c4fcc(color, alpha)
 * sets: each whole group before limitGroup at full progress, then
 * limitGroup itself cut at limitIndex points and limitFraction. */
void func_ov007_020c4dfc(Ov007Picker *self, int half, int color, int alpha) {
    int i;

    func_ov007_020c4fcc(color, alpha);
    for (i = 0; i < GetLimitGroup(self); i++) {
        func_ov007_020c19cc(self->groups[i]->pointCount, self->groupPoints[i], half, 0x1000);
    }
    if (self->limitGroup < self->groupCount) {
        func_ov007_020c19cc(self->limitIndex, self->groupPoints[self->limitGroup], half,
                            self->limitFraction);
    }
}

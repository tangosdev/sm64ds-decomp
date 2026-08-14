//cpp
/* Slot 2. Calls the base's RestoreInitial (mCurrent = mInitial), then
 * exhaustively zeroes every field this class adds, in the ROM's own
 * per-index-then-next-field order for the two 3-element arrays (A[i].a,
 * A[i].b, B[i].a, B[i].b, C[i], D[i] before moving to i+1), followed by
 * the four trailing scalars in address order. This function is the
 * strongest evidence for the field list in the header. */
#include "cMgSmartball_slot_c.h"

void cMgSmartball_slot_c::RestoreInitial()
{
    cMgSmartball_object_c::RestoreInitial();
    for (int i = 0; i < 3; i++) {
        unk_034[i].a = 0;
        unk_034[i].b = 0;
        unk_04c[i].a = 0;
        unk_04c[i].b = 0;
        unk_064[i] = 0;
        unk_070[i] = 0;
    }
    unk_073 = 0;
    unk_074 = 0;
    unk_075 = 0;
    unk_078 = 0;
    unk_07c = 0;
    unk_080 = 0;
    unk_084 = 0;
}

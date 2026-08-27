//cpp
/* Slot 2. Unlike most of this family's slot 2s this is a real body, not a
 * tail-call veneer -- but it still OPENS with the qualified call to the base,
 * and the qualification is load-bearing exactly as it is in the veneers: an
 * unqualified RestoreInitial() here would dispatch through the vtable and
 * recurse.
 *
 * After that it refills the shared shuffle table data_ov006_02142c1c with the
 * identity 0..8, reshuffles it via func_ov006_0210f998, and then zeroes every
 * field this class owns. That zero pass is exhaustive and in address order,
 * which is why it -- not a union of the "obvious" accesses in the other two
 * slots -- is the authority for the layout in cMgSmartball_board_c.h.
 *
 * The nine cell bytes are reached as (&unk_031)[i] -- pointer arithmetic off
 * the BASE's first cell byte, running through unk_032/unk_033 and on into
 * this class's mCellsTail[6]. No member can name that array: it begins three
 * bytes inside the base and a member cannot straddle the boundary. This is
 * the only spelling that reproduces; five others were measured and every one
 * of them compiled the wrong addressing mode or let strength reduction build
 * an induction variable the ROM does not have. See the header. */
#include "cMgSmartball_board_c.h"

extern "C" int data_ov006_02142c1c[9];
extern "C" void func_ov006_0210f998(void);

void cMgSmartball_board_c::RestoreInitial()
{
    int i;
    cMgSmartball_object_c::RestoreInitial();
    for (i = 0; i < 9; i++)
        data_ov006_02142c1c[i] = i;
    func_ov006_0210f998();
    for (i = 0; i < 9; i++) {
        (&unk_031)[i] = 0;
        mCellFlipAngle[i] = 0;
        mCellHighlight[i] = 0;
    }
    for (i = 0; i < 8; i++) {
        mLineTimer[i] = 0;
        mLineScored[i] = 0;
    }
    mFanfareTimer = 0;
    mLinesScored = 0;
}

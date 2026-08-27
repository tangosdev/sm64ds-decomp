//cpp
// @symbol _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_
/* recovered: named members + shared header */
#include "dActor_c.h"
extern "C" {
/* Sets up the clip volume. All four arrive in position units; the three
   distances are stored in the clipper's own units, which are 1/8 of those, while
   the vertical offset stays in position units because BeforeBehavior adds it to
   mPosY before doing the shift itself.

   Spelt with scalar arguments rather than as a real method: the mangled name
   carries by-value Fix12<int>s, and CW homes those to the stack for +0x14.
   See notes/mwccarm-codegen.md 6az. */
void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(struct dActor_c *self, int offsetY,
                                               int radius, int clipDistance,
                                               int farDistance) {
self->mClipOffsetY=offsetY;
self->mClipRadius=radius>>3;
self->mClipDistance=clipDistance>>3;
self->mFarDistance=farDistance>>3;
}
}

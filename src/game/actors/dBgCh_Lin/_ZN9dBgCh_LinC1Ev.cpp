//cpp
// @symbol _ZN9dBgCh_LinC1Ev
/* recovered: real C++ constructor -- the first MI one in the tree
 *
 * The bases are declared by include/dBgCh_Lin.h (: dBgCh, dBgPi, dM3dGLin per
 * the ROM's own __vmi_class_type_info), so the compiler synthesises the base
 * steps (bl _ZN5dBgChC2Ev, bl _ZN5dBgPiC2Ev), both vptr stores (_ZTV9dBgCh_Lin
 * primary, VTable_dBgPi_dBgCh_LinThunk secondary) and the member step
 * (bl _ZN8dM3dGSphC1Ev for mBoundSphere at 0x64). Only the zeroing of
 * lineEnd/clsnDist is hand-written below, spelled as the value chain the ROM's
 * schedule shows: z first, then each word copied down from its neighbour.
 */
#include "dBgCh_Lin.h"

dBgCh_Lin::dBgCh_Lin()
{
    lineEnd.z = 0;
    lineEnd.y = lineEnd.z;
    lineEnd.x = lineEnd.y;
    clsnDist = 0;
}

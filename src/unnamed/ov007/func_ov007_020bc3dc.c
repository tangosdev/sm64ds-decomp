#include "Ov007Model.h"
#include "BCA_File.h"

/* The byte at +4 of the BCA header selects func_ov007_020c9214's loop mode
 * below: zero plays once, nonzero wraps. */

/* Unprototyped, as the tree has it: a prototype with the definition's short
 * parameters adds sign-extensions the ROM does not have. */
extern void func_ov007_020c9214();

/* func_ov007_020c9214's loopMode values used here (see func_ov007_020c9110). */
enum Ov007LoopMode { LOOP_ONCE, LOOP_WRAP };

/* Starts bone animation i at startFrame, at one frame per tick; does nothing
 * when the table has no file for i. */
void func_ov007_020bc3dc(Ov007Model *m, int i, int startFrame) {
    struct BCA_File *bca = m->boneAnims[i];
    enum Ov007LoopMode loopMode;
    if (bca == 0)
        return;
    loopMode = (bca->looping == 0) ? LOOP_ONCE : LOOP_WRAP;
    func_ov007_020c9214(m->boneAnim, i, loopMode, bca->numFrames, startFrame, 0x1000);
}

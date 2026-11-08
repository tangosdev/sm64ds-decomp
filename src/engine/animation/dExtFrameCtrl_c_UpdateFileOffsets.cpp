//cpp
/* dExtFrameCtrl_c::UpdateFileOffsets -- the BCA load-time fixup, arm9
 * 0x020469ac..0x020469e0. The four offsets in a freshly loaded BCA file
 * are file-relative; rebase them into pointers.
 *
 * The four words are the scale, rotation and translation value arrays and
 * the per-bone track table; see BCA_File.h for the evidence.
 */
#include "dExtFrameCtrl_c.h"
#include "BCA_File.h"

void dExtFrameCtrl_c::UpdateFileOffsets(BCA_File &file)
{
    file.scaleValues = (s32 *)((char *)&file + (int)file.scaleValues);
    file.rotationValues = (s16 *)((char *)&file + (int)file.rotationValues);
    file.translationValues = (s32 *)((char *)&file + (int)file.translationValues);
    file.boneTracks = (BCA_BoneTrack *)((char *)&file + (int)file.boneTracks);
}

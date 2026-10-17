//cpp
// @symbol _ZN15dExtFrameCtrl_c17UpdateFileOffsetsER8BCA_File
/* dExtFrameCtrl_c::UpdateFileOffsets -- the BCA load-time fixup, arm9
 * 0x020469ac..0x020469e0. The four offsets in a freshly loaded BCA file
 * are file-relative; rebase them into pointers.
 *
 * leftovers: unk_08..unk_14 stay placeholders -- nothing in the tree reads
 * what they point at (SetAnim only needs numFrames, spelled the way
 * ModelAnim.cpp and BlendModelAnim.cpp already type it).
 */
#include "dExtFrameCtrl_c.h"

struct BCA_File {
    u16 unk_00;
    u16 numFrames;           /* 0x02 - what SetAnim reads */
    u32 unk_04;
    char *unk_08;            /* 0x08 - file-relative, fixed up here */
    char *unk_0c;            /* 0x0c */
    char *unk_10;            /* 0x10 */
    char *unk_14;            /* 0x14 */
};

void dExtFrameCtrl_c::UpdateFileOffsets(BCA_File &file)
{
    file.unk_08 = (char *)&file + (int)file.unk_08;
    file.unk_0c = (char *)&file + (int)file.unk_0c;
    file.unk_10 = (char *)&file + (int)file.unk_10;
    file.unk_14 = (char *)&file + (int)file.unk_14;
}

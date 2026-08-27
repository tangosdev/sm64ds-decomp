//cpp
// @symbol _ZN9Animation17UpdateFileOffsetsER8BCA_File
#include "Animation.h"

/* The four offsets in a freshly loaded BCA file are file-relative; rebase
   them into pointers. Only the header words this touches are typed. */
struct BCA_File {
    u32 unk_00;
    u32 unk_04;
    char *unk_08;      /* 0x08 - file-relative, fixed up here */
    char *unk_0c;      /* 0x0c */
    char *unk_10;      /* 0x10 */
    char *unk_14;      /* 0x14 */
};

void Animation::UpdateFileOffsets(BCA_File &file)
{
    file.unk_08 = (char *)&file + (int)file.unk_08;
    file.unk_0c = (char *)&file + (int)file.unk_0c;
    file.unk_10 = (char *)&file + (int)file.unk_10;
    file.unk_14 = (char *)&file + (int)file.unk_14;
}

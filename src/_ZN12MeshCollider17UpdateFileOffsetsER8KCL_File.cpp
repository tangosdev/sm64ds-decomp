//cpp
// @symbol _ZN12MeshCollider17UpdateFileOffsetsER8KCL_File
#include "MeshCollider.h"

/* The four header words of a fresh KCL file are file-relative offsets;
   rebase them into pointers. */
struct KCL_File {
    char *unk_00;
    char *unk_04;
    char *unk_08;
    char *unk_0c;
};

void MeshCollider::UpdateFileOffsets(KCL_File &file)
{
    file.unk_00 = (char *)&file + (int)file.unk_00;
    file.unk_04 = (char *)&file + (int)file.unk_04;
    file.unk_08 = (char *)&file + (int)file.unk_08;
    file.unk_0c = (char *)&file + (int)file.unk_0c;
}

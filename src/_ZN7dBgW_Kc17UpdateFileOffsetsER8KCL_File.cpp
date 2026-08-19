//cpp
// @symbol _ZN12MeshCollider17UpdateFileOffsetsER8KCL_File
#include "MeshCollider.h"

/* The four header words of a fresh KCL file are file-relative offsets;
   rebase them into pointers. */
void MeshCollider::UpdateFileOffsets(KCL_File &file)
{
    file.positions = (s32 (*)[3])((char *)&file + (int)file.positions);
    file.normals = (s16 (*)[3])((char *)&file + (int)file.normals);
    file.tris = (KCL_Tri *)((char *)&file + (int)file.tris);
    file.unk_0c = (char *)&file + (int)file.unk_0c;
}

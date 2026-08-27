//cpp
// @symbol _ZN10ModelAnim24CopyERKS_Pcj
#include "ModelAnim2.h"

/* otherAnim.Copy(src) upcasts src to its Animation BASE (+0x50): the
   second animation is seeded from the first one's state. */
void ModelAnim2::Copy(const ModelAnim2 &src, char *newFile, u32 newOtherFile)
{
    ModelAnim::Copy(src, newFile);
    otherAnim.Copy(src);
    if (newOtherFile)
        otherFile = newOtherFile;
    else
        otherFile = src.otherFile;
}

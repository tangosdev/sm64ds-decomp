//cpp
// @symbol _ZN9ModelAnim4CopyERKS_Pc
#include "ModelAnim.h"

/* Both this and src adjust by +0x50 for the base call: the first real
   multiple-inheritance argument conversion in the tree. */
void ModelAnim::Copy(const ModelAnim &src, char *newFile)
{
    Animation::Copy(src);
    if (newFile != 0)
        file = (BCA_File *)newFile;
    else
        file = src.file;
}

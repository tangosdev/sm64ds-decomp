//cpp
#include "View.h"

void CopyToViewMat(const Matrix4x3 *mat);

s32 View::Render()
{
    CopyToViewMat(&viewMat);
    return 1;
}

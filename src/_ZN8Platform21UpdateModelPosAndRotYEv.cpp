//cpp
// @symbol _ZN8Platform21UpdateModelPosAndRotYEv
/* recovered: named members + shared header, real C++ method */
#include "Platform.h"
extern void Matrix4x3_FromRotationY(void *, int);

void Platform::UpdateModelPosAndRotY()
{
    Matrix4x3_FromRotationY(((char *)this) + 0xf0, unk_08e);
    unk_114 = mPosX >> 3;
    unk_118 = mPosY >> 3;
    unk_11c = mPosZ >> 3;
}

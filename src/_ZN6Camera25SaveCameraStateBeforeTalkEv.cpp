//cpp
// @symbol _ZN6Camera25SaveCameraStateBeforeTalkEv
/* recovered: named members + shared header, real C++ method */
#include "Camera.h"


void Camera::SaveCameraStateBeforeTalk()
{
    if (*(unsigned int *)((char *)&unk_154) & 0x4000U) return;
    *(unsigned int *)((char *)&unk_0b0) = *(unsigned int *)((char *)&unk_080);
    *(unsigned int *)((char *)&unk_0b4) = *(unsigned int *)((char *)&unk_084);
    *(unsigned int *)((char *)&unk_0b8) = *(unsigned int *)((char *)&unk_088);
    *(unsigned int *)((char *)&unk_0bc) = *(unsigned int *)((char *)&unk_08c);
    *(unsigned int *)((char *)&unk_0c0) = *(unsigned int *)((char *)&unk_090);
    *(unsigned int *)((char *)&unk_0c4) = *(unsigned int *)((char *)&unk_094);
    *(unsigned int *)((int *)(((int)((void *)this) + 0x154) & 0xFFFFFFFFFFFFFFFF)) |= 0x4000U;
}

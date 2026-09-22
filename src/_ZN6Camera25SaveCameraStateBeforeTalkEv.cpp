//cpp
// @symbol _ZN6Camera25SaveCameraStateBeforeTalkEv
/* recovered: named members + shared header, real C++ method */
#include "Camera.h"


void Camera::SaveCameraStateBeforeTalk()
{
    if (*(unsigned int *)((char *)&mFlags) & 0x4000U) return;
    *(unsigned int *)((char *)&savedLookAt.x) = *(unsigned int *)((char *)&lookAt.x);
    *(unsigned int *)((char *)&savedLookAt.y) = *(unsigned int *)((char *)&lookAt.y);
    *(unsigned int *)((char *)&savedLookAt.z) = *(unsigned int *)((char *)&lookAt.z);
    *(unsigned int *)((char *)&savedPos.x) = *(unsigned int *)((char *)&pos.x);
    *(unsigned int *)((char *)&savedPos.y) = *(unsigned int *)((char *)&pos.y);
    *(unsigned int *)((char *)&savedPos.z) = *(unsigned int *)((char *)&pos.z);
    *(unsigned int *)((int *)(((int)((void *)this) + 0x154))) |= 0x4000U;
}

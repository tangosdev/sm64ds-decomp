//cpp
// @symbol _ZN10DonutBlock8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "DonutBlock.h"
typedef int Fix12;
struct MeshColliderBase { bool IsEnabled(); void Disable(); };
struct Platform {
    void UpdateModelPosAndRotY();
    bool IsClsnInRange(Fix12 a, Fix12 b);
    void UpdateClsnPosAndRot();
};

int DonutBlock::Behavior()
{
    short *s = (short *)(((int)((char *)this) + 0x8e) & 0xFFFFFFFFFFFFFFFF);
    short *t = (short *)(((int)((char *)this) + 0x300) & 0xFFFFFFFFFFFFFFFF);
    *s = (short)(*s + t[0x1e / 2]);
    int b = (int)((unk_0b0 & 8) != 0);
    if (b != 0) {
        if (((MeshColliderBase *)((char *)&mMeshCollider))->IsEnabled())
            ((MeshColliderBase *)((char *)&mMeshCollider))->Disable();
        return 1;
    }
    Platform *p = (Platform *)((char *)this);
    p->UpdateModelPosAndRotY();
    if (p->IsClsnInRange(0, 0))
        p->UpdateClsnPosAndRot();
    return 1;
}

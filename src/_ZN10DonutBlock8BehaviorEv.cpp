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
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" bool _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *, Fix12 a, Fix12 b);


int DonutBlock::Behavior()
{
    short *s = (short *)(((int)((char *)this) + 0x8e));
    short *t = (short *)(((int)((char *)this) + 0x300));
    *s = (short)(*s + t[0x1e / 2]);
    int b = (int)((unk_0b0 & 8) != 0);
    if (b != 0) {
        if (((MeshColliderBase *)((char *)&mMeshCollider))->IsEnabled())
            ((MeshColliderBase *)((char *)&mMeshCollider))->Disable();
        return 1;
    }
    Platform *p = (Platform *)((char *)this);
    p->UpdateModelPosAndRotY();
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(p, 0, 0))
        p->UpdateClsnPosAndRot();
    return 1;
}

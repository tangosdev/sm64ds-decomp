//cpp
// @symbol _ZN8Platform19UpdateClsnPosAndRotEv
/* recovered: named members + shared header, real C++ method */
#include "Platform.h"
extern "C" {
struct M4 { int w[12]; };
struct MMC { char p[0x124]; };
struct Obj { char p[0x2ec]; M4 m; };
int _ZN18MovingMeshCollider9TransformERK9Matrix4x3s(MMC*, M4&, short);
}

void Platform::UpdateClsnPosAndRot()
{
    Obj* o = (Obj*)((char*)this);
    o->m = *(M4*)((char*)&unk_0f0);
    unk_310 = mPosX;
    unk_314 = mPosY;
    unk_318 = mPosZ;
    _ZN18MovingMeshCollider9TransformERK9Matrix4x3s((MMC*)((char*)&mMeshCollider), o->m, mAngleY);
}

//cpp
// @symbol _ZN19FirePiranhaPlantBig8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FirePiranhaPlantBig.h"
extern "C" {
extern void _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(char *thiz, char *clsn);
extern void _ZN12CylinderClsn5ClearEv(char *thiz);
extern void _ZN12CylinderClsn6UpdateEv(char *thiz);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(char *thiz, char *v);
}

int FirePiranhaPlantBig::Behavior()
{
    _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(((char *)this), ((char *)this) + 0x174);
    int b = (mFlags & 0x60000) != 0;
    if (b != 0) {
        func_ov084_0212d564(((char *)this));
        return 1;
    }
    mModelAnim.Advance();
    int s = mState;
    switch (s) {
    case 0:
        func_ov084_0212e4e0(((char *)this));
        break;
    case 1:
        func_ov084_0212e010(((char *)this));
        break;
    case 2:
        func_ov084_0212ddbc(((char *)this));
        break;
    case 3:
        func_ov084_0212dc30(((char *)this));
        break;
    case 4:
        break;
    }
    {
        char * p = ((char *)this) + 0x100;
        *(unsigned short *)p = *(unsigned short *)p + 1;
        if (s != mState)
            *(unsigned short *)p = 0;
    }
    func_ov084_0212d86c(((char *)this));
    func_ov084_0212d564(((char *)this));
    _ZN12CylinderClsn5ClearEv((char *)&mMovingCylinderClsn);
    mMovingCylinderClsn.radius = mScale * unk_208;
    mMovingCylinderClsn.height = mScale * unk_20c;
    _ZN12CylinderClsn6UpdateEv((char *)&mMovingCylinderClsn);
    _ZN12CylinderClsn5ClearEv((char *)&mMovingCylinderClsnWithPos);
    int b2 = actorID == 0xfc;
    if (b2 == 0
        && (unsigned int)(mState - 2) <= 1
        && mScale == unk_210) {
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(((char *)this) + 0x1a8, ((char *)this) + 0x1f8);
        _ZN12CylinderClsn6UpdateEv((char *)&mMovingCylinderClsnWithPos);
    }
    return 1;
}

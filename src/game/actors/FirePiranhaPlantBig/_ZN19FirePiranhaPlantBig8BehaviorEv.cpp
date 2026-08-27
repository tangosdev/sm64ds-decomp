//cpp
// @symbol _ZN19FirePiranhaPlantBig8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FirePiranhaPlantBig.h"
extern "C" {
extern void _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(char *thiz, char *clsn);
extern void _ZN5dCc_c5ClearEv(char *thiz);
extern void _ZN5dCc_c6UpdateEv(char *thiz);
extern void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(char *thiz, char *v);
}

int FirePiranhaPlantBig::Behavior()
{
    _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(((char *)this), (char *)&mdCcAc_c);
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
    _ZN5dCc_c5ClearEv((char *)&mdCcAc_c);
    mdCcAc_c.radius = mScale * mClsnRadiusFactor;
    mdCcAc_c.height = mScale * mClsnHeightFactor;
    _ZN5dCc_c6UpdateEv((char *)&mdCcAc_c);
    _ZN5dCc_c5ClearEv((char *)&mdCcAcPos_c);
    int b2 = actorID == 0xfc;
    if (b2 == 0
        && (unsigned int)(mState - 2) <= 1
        && mScale == mMaxScale) {
        _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3((char *)&mdCcAcPos_c, (char *)&mClsnOffset);
        _ZN5dCc_c6UpdateEv((char *)&mdCcAcPos_c);
    }
    return 1;
}

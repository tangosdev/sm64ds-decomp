//cpp
// @symbol _ZN7Tornado8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Tornado.h"
extern "C" int Vec3_Dist(void *a, void *b);
extern "C" void Matrix4x3_FromTranslation(void *m, int x, int y, int z);

int Tornado::Behavior()
{
    int s = mState;
    switch (s) {
    case 0: State0(); break;
    case 1: State1(); break;
    case 2: State2(); break;
    }
    {
        unsigned short *p = (unsigned short*)(((int)((char *)this) + 0x350));
        *p = *p + 1;
    }
    if (s != mState) {
        mStateTimer = 0;
        mTriggerCount = 0;
    }
    unsigned int id = mdCcAc_c.otherOwner;
    if (id != 0 && (mdCcAc_c.hitFlags & 0x400000) != 0) {
        dActor_c *o = dActor_c::FindWithID(id);
        if (o != 0) {
            dActor_c *closest = ClosestWithActorID(0x135);
            if (closest == 0 || Vec3_Dist(&o->mPosX, &closest->mPosX) > 0x118000) {
                if (func_ov002_020de33c((char*)o, (int)((char *)this)) != 0) {
                    mCaughtActor = o;
                }
            }
        }
    }
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    Matrix4x3_FromTranslation(((char *)this) + 0x2e0, mPosX >> 3, mPosY >> 3, mPosZ >> 3);
    mModelAnim.Animation::Advance();
    mTextureTransformer.Advance();
    return 1;
}

//cpp
// @symbol _ZN7Tornado8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Tornado.h"
extern "C" void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
extern "C" void *_ZN8dActor_c18ClosestWithActorIDEj(void *actor, unsigned int id);
extern "C" int Vec3_Dist(void *a, void *b);
extern "C" void _ZN5dCc_c5ClearEv(void *thiz);
extern "C" void _ZN5dCc_c6UpdateEv(void *thiz);
extern "C" void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern "C" void _ZN9Animation7AdvanceEv(void *thiz);

int Tornado::Behavior()
{
    int s = mState;
    switch (s) {
    case 0: func_ov096_021372c0(); break;
    case 1: func_ov096_02137088(); break;
    case 2: func_ov096_02136fd4(); break;
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
        void *o = _ZN8dActor_c10FindWithIDEj(id);
        if (o != 0) {
            void *closest = _ZN8dActor_c18ClosestWithActorIDEj(((char *)this), 0x135);
            if (closest == 0 || Vec3_Dist((char*)o + 0x5c, (char*)closest + 0x5c) > 0x118000) {
                if (func_ov002_020de33c((char*)o, (int)((char *)this)) != 0) {
                    *(void**)((char *)&mCaughtActor) = o;
                }
            }
        }
    }
    _ZN5dCc_c5ClearEv((char *)&mdCcAc_c);
    _ZN5dCc_c6UpdateEv((char *)&mdCcAc_c);
    Matrix4x3_FromTranslation(((char *)this) + 0x2e0, mPosX >> 3, mPosY >> 3, mPosZ >> 3);
    _ZN9Animation7AdvanceEv((char *)(Animation *)&mModelAnim);
    _ZN9Animation7AdvanceEv((char *)&mTextureTransformer);
    return 1;
}

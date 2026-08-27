//cpp
// @symbol _ZN7daKrb_c6RenderEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_dCapEnemy_c.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daKrb_c.h"
#pragma opt_common_subs off

struct Sub {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void m5(Vector3* v);
};

extern "C" {
extern void _ZN15MaterialChanger6UpdateER15ModelComponents(char* self, void* model);
}

int daKrb_c::Render()
{
    int locked;
    volatile Vector3 backup;

    locked = (mFlags & 0x40000) != 0;
    if (locked || mIsDormant != 0) return 1;

    backup.x = mScaleX;
    backup.y = mScaleY;
    backup.z = mScaleZ;

    if (mDeathState == 1) {
        mScaleX = (int)(((long long)mScaleX * data_ov084_02130258[mGoombaType] + 0x800) >> 12);
        mScaleY = (int)(((long long)mScaleY * data_ov084_02130258[mGoombaType] + 0x800) >> 12);
        mScaleZ = (int)(((long long)mScaleZ * data_ov084_02130258[mGoombaType] + 0x800) >> 12);
    }

    {
        Sub* b = (Sub*)((char*)&mModelAnim);
        b->m5((Vector3*)((char*)&mScaleX));
    }

    mScaleX = backup.x;
    mScaleY = backup.y;
    mScaleZ = backup.z;
    _ZN15MaterialChanger6UpdateER15ModelComponents(((char*)this) + 0x3fc, ((char*)this) + 0x378);
    _ZN11dCapEnemy_c14RenderCapModelEPK7Vector3(((char*)this), 0);
    return 1;
}

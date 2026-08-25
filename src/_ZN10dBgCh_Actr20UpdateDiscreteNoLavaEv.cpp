//cpp
// @symbol _ZN10dBgCh_Actr20UpdateDiscreteNoLavaEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_dBgCh_SphCrr.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "dBgCh_Actr.h"
extern "C" {


extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *self);
extern int func_02037938(int p);
extern void func_02038324(void *arg, int b, int c, int d);
extern void _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(void *self, struct Vector3 *v, int fix, void *actor);
extern void func_020356d4(char *self);
}

void dBgCh_Actr::UpdateDiscreteNoLava()
{
    int onGround;
    int sy;
    struct Vector3 v;
    struct Vector3 *p6c;
    char *obj = *(char **)((char *)&mActor);
    struct Vector3 *src = (struct Vector3 *)(obj + 0x5c);
    struct Vector3 *p68 = (struct Vector3 *)(obj + 0x68);

    if (_ZNK10dBgCh_Actr10IsOnGroundEv(((char *)this)) && func_020355a0(((char *)this))
        && _ZNK10dBgCh_Actr15ShouldUpdatePosEv(((char *)this))) {
        func_02038324((void *)func_02037938((int)((char *)&mSphereClsn)), (int)src,
                      mSphereClsn.unk_10c, unk_130);
    }
    onGround = _ZNK10dBgCh_Actr10IsOnGroundEv(((char *)this));
    _ZN10dBgCh_Actr19ClearAllGroundFlagsEv(((char *)this));
    v.x = src->x;
    sy = src->y;
    v.y = sy;
    v.z = src->z;
    v.y = sy + mHeight;
    _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(((char *)this) + 0x20, &v,
        mRadius, *(void **)((char *)&mActor));
    mSphereClsn.unk_108 = mScale;
    if (src->y - p68->y > 0) {
        *(unsigned char *)((char *)&mSphereClsn.flags) |= 0x20;
    }
    if (_ZN12dBgCh_SphCrr10DetectClsnEv((char *)&mSphereClsn)) {
        p6c = (struct Vector3 *)((char *)&mSphereClsn.disp);
        if (mSphereClsn.flags & 4) {
            func_020371b0(((char *)this), onGround);
        }
        if (_ZNK10dBgCh_Actr15ShouldUpdatePosEv(((char *)this))) {
            src->x += p6c->x;
            if (_ZNK10dBgCh_Actr16ShouldUpdatePosYEv(((char *)this))) {
                *(int *)((char *)src + 4) += p6c->y;
            }
            *(int *)((char *)src + 8) += p6c->z;
        }
    }
    if (onGround == 0)
        return;
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(((char *)this)))
        return;
    func_020356d4(((char *)this));
}

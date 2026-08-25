//cpp
// @symbol _ZN10dBgCh_Actr22UpdateDiscreteNoLava_2Ev
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "dBgCh_Actr.h"
extern "C" {


extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *self);
extern void _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(void *self, struct Vector3 *v, int fix, void *actor);
extern void func_020356d4(char *self);
}

void dBgCh_Actr::UpdateDiscreteNoLava_2()
{
    int onGround;
    int sy;
    struct Vector3 v;
    struct Vector3 *p6c;
    struct Vector3 *src = (struct Vector3 *)(*(char **)((char *)&mActor) + 0x5c);
    char *obj = *(char **)((char *)&mActor);

    onGround = _ZNK10dBgCh_Actr10IsOnGroundEv(((char *)this));
    _ZN10dBgCh_Actr19ClearAllGroundFlagsEv(((char *)this));
    v.x = *(int *)(src);
    sy = src->y;
    v.y = sy;
    v.z = src->z;
    v.y = sy + unk_01c;
    _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(((char *)this) + 0x20, &v,
        unk_018, *(void **)((char *)&mActor));
    mSphereClsn.unk_108 = unk_1b8;
    if (src->y - *(int *)(obj + 0x6c) > 0) {
        *(unsigned char *)((char *)&mSphereClsn.flags) |= 0x20;
    }
    if (func_02038a38((char *)&mSphereClsn)) {
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

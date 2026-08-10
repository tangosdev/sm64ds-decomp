//cpp
// @symbol _ZN12WithMeshClsn20UpdateDiscreteNoLavaEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_SphereClsn.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "WithMeshClsn.h"
extern "C" {


extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
extern int func_02037938(int p);
extern void func_02038324(void *arg, int b, int c, int d);
extern void _ZN10SphereClsn15SetObjAndSphereERK7Vector35Fix12IiEP5Actor(void *self, struct Vector3 *v, int fix, void *actor);
extern void func_020356d4(char *self);
}

void WithMeshClsn::UpdateDiscreteNoLava()
{
    int onGround;
    int sy;
    struct Vector3 v;
    struct Vector3 *p6c;
    char *obj = *(char **)((char *)&mActor);
    struct Vector3 *src = (struct Vector3 *)(obj + 0x5c);
    struct Vector3 *p68 = (struct Vector3 *)(obj + 0x68);

    if (_ZNK12WithMeshClsn10IsOnGroundEv(((char *)this)) && func_020355a0(((char *)this))
        && _ZNK12WithMeshClsn15ShouldUpdatePosEv(((char *)this))) {
        func_02038324((void *)func_02037938((int)((char *)&mSphereClsn)), (int)src,
                      unk_12c, unk_130);
    }
    onGround = _ZNK12WithMeshClsn10IsOnGroundEv(((char *)this));
    _ZN12WithMeshClsn19ClearAllGroundFlagsEv(((char *)this));
    v.x = src->x;
    sy = src->y;
    v.y = sy;
    v.z = src->z;
    v.y = sy + unk_01c;
    _ZN10SphereClsn15SetObjAndSphereERK7Vector35Fix12IiEP5Actor(((char *)this) + 0x20, &v,
        unk_018, *(void **)((char *)&mActor));
    unk_128 = unk_1b8;
    if (src->y - p68->y > 0) {
        *(unsigned char *)((char *)&mClsnFlags) |= 0x20;
    }
    if (_ZN10SphereClsn10DetectClsnEv((char *)&mSphereClsn)) {
        p6c = (struct Vector3 *)((char *)&unk_06c);
        if (mClsnFlags & 4) {
            func_020371b0(((char *)this), onGround);
        }
        if (_ZNK12WithMeshClsn15ShouldUpdatePosEv(((char *)this))) {
            src->x += p6c->x;
            if (_ZNK12WithMeshClsn16ShouldUpdatePosYEv(((char *)this))) {
                *(int *)((char *)src + 4) += p6c->y;
            }
            *(int *)((char *)src + 8) += p6c->z;
        }
    }
    if (onGround == 0)
        return;
    if (_ZNK12WithMeshClsn10IsOnGroundEv(((char *)this)))
        return;
    func_020356d4(((char *)this));
}

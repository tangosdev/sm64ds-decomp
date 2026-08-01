//cpp
// @symbol _ZN12WithMeshClsn22UpdateDiscreteNoLava_2Ev
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "WithMeshClsn.h"
extern "C" {


extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
extern void _ZN10SphereClsn15SetObjAndSphereERK7Vector35Fix12IiEP5Actor(void *self, struct Vector3 *v, int fix, void *actor);
extern void func_020356d4(char *self);
}

void WithMeshClsn::UpdateDiscreteNoLava_2()
{
    int onGround;
    int sy;
    struct Vector3 v;
    struct Vector3 *p6c;
    struct Vector3 *src = (struct Vector3 *)(*(char **)((char *)&mActor) + 0x5c);
    char *obj = *(char **)((char *)&mActor);

    onGround = _ZNK12WithMeshClsn10IsOnGroundEv(((char *)this));
    _ZN12WithMeshClsn19ClearAllGroundFlagsEv(((char *)this));
    v.x = *(int *)(((long long)(int)src));
    sy = src->y;
    v.y = sy;
    v.z = src->z;
    v.y = sy + unk_01c;
    _ZN10SphereClsn15SetObjAndSphereERK7Vector35Fix12IiEP5Actor(((char *)this) + 0x20, &v,
        unk_018, *(void **)((char *)&mActor));
    unk_128 = unk_1b8;
    if (src->y - *(int *)(obj + 0x6c) > 0) {
        *(unsigned char *)(((long long)(int)((char *)&mClsnFlags))) |= 0x20;
    }
    if (func_02038a38((char *)&mSphereClsn)) {
        p6c = (struct Vector3 *)((char *)&unk_06c);
        if (mClsnFlags & 4) {
            func_020371b0(((char *)this), onGround);
        }
        if (_ZNK12WithMeshClsn15ShouldUpdatePosEv(((char *)this))) {
            src->x += p6c->x;
            if (_ZNK12WithMeshClsn16ShouldUpdatePosYEv(((char *)this))) {
                *(int *)(((long long)(int)((char *)src + 4))) += p6c->y;
            }
            *(int *)(((long long)(int)((char *)src + 8))) += p6c->z;
        }
    }
    if (onGround == 0)
        return;
    if (_ZNK12WithMeshClsn10IsOnGroundEv(((char *)this)))
        return;
    func_020356d4(((char *)this));
}

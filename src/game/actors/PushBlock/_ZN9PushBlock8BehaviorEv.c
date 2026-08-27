// NOT MIGRATED to a real PushBlock:: method, measured 2026-08-22. The body
// converts cleanly (every instruction matches with `this` and named members --
// mPrevAngleY 0x94, mHorzSpeed 0x98), but the C++ form emits TWO EXTRA LITERAL
// POOL WORDS: 133 words against the ROM's 131, and every mismatch is a
// pc-relative pool offset or a branch displacement shifted by exactly 8 bytes,
// never an opcode. Ruled out by test: naming mPosX vs a raw (char*)this+0x5c,
// mPrevAngleY raw, mHorzSpeed raw -- all three still 133. Whatever adds the two
// constants is not the member naming. Its siblings CleanupResources and
// OnPushed migrated fine, so this is specific to this body.
// @symbol _ZN9PushBlock8BehaviorEv
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header
 *
 * PushBlock::Behavior -- vtable slot 6, ov002 0x020b8dd4. Attributed by the
 * vtable: _ZTV9PushBlock carries this address at slot 6, where
 * _ZTV10dBgActor_c carries fBase_c's generic 0x02043b24.
 *
 * Kept as an extern free function under the literal mangled name, reading the
 * flat C spelling of PushBlock (include/PushBlock.h's #else half) by hand
 * offset -- same shape as _ZN9PushBlock13InitResourcesEv.cpp. */
#include "decl_Platform.h"
#include "decl_common.h"
#include "PushBlock.h"
typedef short s16;

extern Matrix4x3 data_020a0e68;

extern void Matrix4x3_FromRotationY(Matrix4x3 *m, int angle);
extern void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *dst);
extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern void _ZN9dBgCh_GndC1Ev(char *thiz);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(char *thiz, Vector3 *v, void *actor);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(char *thiz);
extern void _ZN9dBgCh_GndD1Ev(char *thiz);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *thiz, void *clsn);
extern void dBgCh_Actr_UpdateContinuous_Veneer(void *p);
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *thiz);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *thiz);
extern void func_020393a4(int *p, int v);
extern void func_02039394(int *p, int v);
extern int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *thiz, int a, int b);
extern int Vec3_Dist(Vector3 *a, Vector3 *b);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int c, Vector3 *pos, unsigned int e);
extern int Vec3_HorzDist(Vector3 *a, Vector3 *b);

int _ZN9PushBlock8BehaviorEv(char *c)
{
    struct PushBlock *self = (struct PushBlock *)(void *)c;
    Vector3 v;
    Vector3 dst;
    volatile Vector3 pos;
    char ray[0x50];
    char *q;

    if (_ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(c, 0x1800, 0, 0, 0x96000) != 0) {
        return 1;
    }

    if (self->mHorzSpeed != 0) {
        v.x = 0;
        dst.x = 0;
        dst.y = 0;
        dst.z = 0;
        v.y = 0x96000;
        v.z = 0x96000;
        Matrix4x3_FromRotationY(&data_020a0e68, self->mPrevAngleY);
        MulVec3Mat4x3(&v, &data_020a0e68, &dst);
        AddVec3(&dst, (Vector3 *)&self->mPosX, &dst);
        _ZN9dBgCh_GndC1Ev(ray);
        _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(ray, &dst, 0);
        if (_ZN9dBgCh_Gnd10DetectClsnEv(ray) != 0) {
            if (self->mGroundY != *(int *)(ray + 0x44)) {
                self->mHorzSpeed = 0;
            }
        } else {
            self->mHorzSpeed = 0;
        }
        _ZN8dActor_c9UpdatePosEP5dCc_c(c, 0);
        dBgCh_Actr_UpdateContinuous_Veneer(&self->mWithMeshClsn);
        _ZN10dBgActor_c21UpdateModelPosAndRotYEv(c);
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
        _ZN9dBgCh_GndD1Ev(ray);
    }

    func_020393a4((int *)&self->mMeshCollider, 0x12c000);
    func_02039394((int *)&self->mMeshCollider, 0x96000);
    _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(c, 0x3e8000, 0x96000);

    if (self->mHorzSpeed != 0 && Vec3_Dist((Vector3 *)&self->mPosX, (Vector3 *)&self->mPrevPosX) != 0) {
        self->mSlideSound = _ZN5Sound8PlayLongEjjjRK7Vector3s(self->mSlideSound, 3, 0x97, (Vector3 *)&self->mCamSpacePosX, 0);
    }

    if (Vec3_HorzDist((Vector3 *)&self->mHomePosX, (Vector3 *)&self->mPosX) >= 0x12c000) {
        if (self->mLinkedActor != 0) {
            if (*(unsigned short *)(self->mLinkedActor + 0xc) == 0x149) {
                pos.x = self->mHomePosX;
                pos.y = self->mHomePosY;
                pos.z = self->mHomePosZ;
                pos.y = self->mHomePosY + 0x96000;
                /* This ONE read of mLinkedActor stays raw: spelling it
                   `(char *)self->mLinkedActor` like the three sites around it
                   costs the function its size -- measured, per site. */
                q = *(char **)(c + 0x4f0);
                *(int *)(q + 0x5c) = self->mHomePosX;
                *(int *)(q + 0x60) = pos.y;
                *(int *)(q + 0x64) = pos.z;
                func_ov002_020f0438((void *)self->mLinkedActor);
            }
            self->mLinkedActor = 0;
        }
    }

    self->mHorzSpeed = 0;
    return 1;
}

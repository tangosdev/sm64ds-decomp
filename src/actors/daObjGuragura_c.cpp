//cpp
/* The tilting platforms' shared base -- ov002/daObjGuragura_c.
 * Abstract: slots 0 (InitResources) and 3 (CleanupResources) are pure.
 * daObjKm2_Gura_c (ov045) and daObjFl_Gura_c (ov064) call
 * func_ov002_020b6244 / func_ov002_020b60fc with their own file tables.
 *
 * The slab tilts mTilt toward mTiltTarget, arms mSettleDelay on a landing,
 * and reloads the rest pose from data_02092768 when that countdown hits 0.
 *
 * deslop
 * Leftover: func_ov002_020b6244 / 020b60fc keep ROM address names (slots 0
 *   and 3 are pure virtual; the two leaves call these). 020b6374 / 020b62cc
 *   / 020b6074 are TU-local; nothing in the image names them.
 * Leftover: 020b6374 keeps #pragma long_calls -- the ROM veneer is the
 *   pooled `ldr ip,[pc,#8]; bx ip` absolute tail-call (size 0x14); a near
 *   `b` to 020b62cc in this same TU is 0xc.
 * Leftover: dBgW_KcMbg::SetFile and dBgActor_c::IsClsnInRange stay mangled
 *   (Fix12<int> by value, wall 6az).
 * Leftover: func_020393c4 is a 4-byte store into dBgW+0x1c. Naming belongs
 *   with dBgW in arm9.
 * Leftover: Quaternion_SLerp / FromVector3 / Normalize stay untyped (mTilt
 *   is s32[4]).
 * Leftover: rest-pose data_02092768 and scratch matrix data_020a0e68 (arm9).
 * Leftover: toucher position is (Vector3 *)(arg + 0x5c); the callback arg
 *   is not a typed actor here.
 * Leftover: abstract class -- no factory, no g_profile row (S14).
 */

#include "daObjGuragura_c.h"
#include "SharedFilePtr.h"

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern "C" {
extern Matrix4x3 data_020a0e68;
extern int data_02092768[4];

void InvMat4x3(Matrix4x3 *d, Matrix4x3 *s);
void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *out);
void Quaternion_FromVector3(int *q, Vector3 *from, Vector3 *to);
void Quaternion_Normalize(int *q);
void Quaternion_SLerp(char *out, char *a, int t, char *b);
void Matrix4x3_FromQuaternion(void *q, Matrix4x3 *m);
void Vec3_Asr(Vector3 *d, Vector3 *s, int sh);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void MulMat4x3Mat4x3(Matrix4x3 *a, void *b, void *c);
void Matrix4x3_ApplyInPlaceToRotationY(void *m, short a);
unsigned char DecIfAbove0_Byte(unsigned char *p);
void func_020393c4(dBgW_KcMbg *clsn, void *fn);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *thiz, int x, int z);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mtx, int scale,
    s16 angleY, CLPS_Block *clps);

void func_ov002_020b62cc(daObjGuragura_c *self, unsigned char *arg);
void func_ov002_020b6374(void *collider, daObjGuragura_c *self, unsigned char *arg);
int func_ov002_020b6244(daObjGuragura_c *self, ResourceDescriptor *descriptor);
int func_ov002_020b60fc(daObjGuragura_c *self, ResourceDescriptor *descriptor);
void func_ov002_020b6074(daObjGuragura_c *self);
}

/* ROM ordinal 6 -- func_ov002_020b6374, 0x020b6374, size 0x14.
 * dBgW callback 020b6244 installs: drops the collider and forwards the
 * other two into 020b62cc. long_calls is the ROM's pooled absolute tail-call. */
// @symbol func_ov002_020b6374
extern "C" {
#pragma long_calls on
void func_ov002_020b6374(void *collider, daObjGuragura_c *self, unsigned char *arg)
{
    func_ov002_020b62cc(self, arg);
}
#pragma long_calls off
}

/* ROM ordinal 5 -- func_ov002_020b62cc, 0x020b62cc, size 0xa8.
 * Landing on the slab: flag mBumped, invert mClsnMat through the arm9
 * scratch matrix, double the toucher's vertical component in slab space,
 * and make mTiltTarget the rotation that carries +Y onto it. Arms
 * mSettleDelay with 10 frames. */
// @symbol func_ov002_020b62cc
extern "C" {
void func_ov002_020b62cc(daObjGuragura_c *self, unsigned char *arg)
{
    Vector3 v;
    Vector3 axis;
    self->mBumped = 1;
    data_020a0e68 = self->mClsnMat;
    InvMat4x3(&data_020a0e68, &data_020a0e68);
    MulVec3Mat4x3((Vector3 *)(arg + 0x5c), &data_020a0e68, &v);
    v.y = v.y << 1;
    axis.x = 0;
    axis.z = 0;
    axis.y = 0x1000;
    Quaternion_FromVector3(self->mTiltTarget, &axis, &v);
    Quaternion_Normalize(self->mTiltTarget);
    self->mSettleDelay = 0xa;
}
}

/* ROM ordinal 4 -- func_ov002_020b6244, 0x020b6244, size 0x88.
 * Shared resource setup both leaves call. Slot 0 is Model::LoadFile, slot 1
 * is dBgW_Kc::LoadFile, slot 2 is CLPS into SetFile; then install 020b6374
 * as the collider callback. */
// @symbol func_ov002_020b6244
extern "C" {
int func_ov002_020b6244(daObjGuragura_c *self, ResourceDescriptor *descriptor)
{
    self->mModel.SetFile((BMD_File *)Model::LoadFile(*descriptor->model), 1, -1);
    self->UpdateModelPosAndRotY();
    self->UpdateClsnPosAndRot();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &self->mMeshCollider,
        (KCL_File *)dBgW_Kc::LoadFile(*descriptor->collision),
        &self->mClsnMat, 0x1000, self->mAngleY, descriptor->clps);
    func_020393c4(&self->mMeshCollider, (void *)&func_ov002_020b6374);
    return 1;
}
}

/* ROM ordinal 3 -- vtable slot 6, ov002 0x020b616c.
 * The (int)((mFlags & 8) != 0) != 0 widening is kept: in C++ the inner
 * != 0 is a bool and the cast width steers the compare. */
// @symbol _ZN15daObjGuragura_c8BehaviorEv
s32 daObjGuragura_c::Behavior()
{
    if ((int)((mFlags & 8) != 0) != 0) {
        if (mMeshCollider.IsEnabled()) {
            mMeshCollider.Disable();
        }
        return 1;
    }
    if (DecIfAbove0_Byte(&mSettleDelay) == 0) {
        mTiltTarget[0] = data_02092768[0];
        mTiltTarget[1] = data_02092768[1];
        mTiltTarget[2] = data_02092768[2];
        mTiltTarget[3] = data_02092768[3];
    }
    Quaternion_SLerp((char *)mTilt, (char *)mTiltTarget, 0x199, (char *)mTilt);
    func_ov002_020b6074(this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0)) {
        UpdateClsnPosAndRot();
    }
    mBumped = 0;
    return 1;
}

/* ROM ordinal 2 -- vtable slot 9, ov002 0x020b6144. Key function. */
// @symbol _ZN15daObjGuragura_c6RenderEv
s32 daObjGuragura_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* ROM ordinal 1 -- func_ov002_020b60fc, 0x020b60fc, size 0x48.
 * Teardown half both leaves call: disable the mesh if live, then release
 * the leaf's two SharedFilePtrs. */
// @symbol func_ov002_020b60fc
extern "C" {
int func_ov002_020b60fc(daObjGuragura_c *self, ResourceDescriptor *descriptor)
{
    if (self->mMeshCollider.IsEnabled())
        self->mMeshCollider.Disable();
    descriptor->model->Release();
    descriptor->collision->Release();
    return 1;
}
}

/* ROM ordinal 0 -- func_ov002_020b6074, 0x020b6074, size 0x88.
 * Per-frame tilt: live quaternion to a matrix, offset by position >> 3,
 * apply mAngleY, publish as the model matrix. */
// @symbol func_ov002_020b6074
extern "C" {
void func_ov002_020b6074(daObjGuragura_c *self)
{
    Matrix4x3 q;
    Vector3 v;
    Matrix4x3_FromQuaternion(self->mTilt, &q);
    Vec3_Asr(&v, (Vector3 *)&self->mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    MulMat4x3Mat4x3(&q, &data_020a0e68, &data_020a0e68);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, self->mAngleY);
    self->mModel.mat4x3 = data_020a0e68;
}
}

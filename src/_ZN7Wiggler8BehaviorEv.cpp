//cpp
// @symbol _ZN7Wiggler8BehaviorEv
/* recovered: real C++ method against include/Wiggler.h; the segment arrays and the
   0x6f8/0x8c4 tail fields are header members now (unk_6f8, unk_702, unk_8d0..unk_8e4).

   Wiggler::Behavior -- vtable slot 6, ov034 0x02112b5c, 0x6e0 bytes.

   Rewritten from the cartridge disassembly rather than from the flat-offset draft
   that sat here before. Credit for that draft stays with it: it was banked as a
   NONMATCHING draft by lunavyqo, and its structure, extern set and frame layout are
   what made this address readable enough to attack. Two things the flat draft had
   to force by hand fall out of the real shape: strength reduction is left ON and
   the five induction pointers of the segment loop (mTextureSequences[i],
   mModelAnims[i], unk_408[i].y as a store base and as a call argument) colour
   r7/r6/r5/r4 with r8 as the counter by themselves, and the second loop's counter
   gets its own block-scoped `j` so it colours r6 above the two collider pointers.

   The face-frame block is the one place the source shape is load-bearing: the ROM
   keeps TWO hoisted zero slots ([sp+0x10] for the clamp, [sp+0x14] for the flag)
   and `1` in fp. `bool ok = false;` declared before the clamp merges both zeros
   into one register; assigning `ok = false;` AFTER `if (n >= 3) n = 0;` keeps them
   distinct, which is what the cartridge did. */
#include "Wiggler.h"

extern "C" {
extern void _ZN7fBase_c18MarkForDestructionEv(void *self);
extern void func_ov034_02112604(void *c);
extern int _Z14ApproachLinearRsss(s16 *p, s16 a, s16 b);
extern void func_0201267c(int a, void *p);
extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(s32 x, s32 y, s32 z);
extern int DecIfAbove0_Byte(u8 *p);
extern int _Z14ApproachLinearRiii(s32 *p, s32 a, s32 b);
extern void func_ov034_02112874(void *c);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 a, u32 b, s32 x, s32 y, s32 z, const void *v, void *cb);
extern void func_ov034_021129ec(void *c);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void MulVec3Mat4x3(void *in, void *mtx, void *out);
extern void Vec3_Add(void *out, void *a, void *b);
extern void func_020092c4(void *arg0, void *out, void *target);

extern void *data_ov034_02113874[];
extern void *data_ov034_02113888[];
extern void *data_ov034_0211389c[];
extern void *data_ov034_021138b0[];
extern u32 data_ov034_02113828[];
extern s16 data_02082214[];
extern void *data_0209f318;
extern s32 data_020a0e68[];
}

int Wiggler::Behavior()
{
    void *animFile;
    s32 i;
    Vector3 head;
    Vector3 part;
    Vector3 camA;
    Vector3 camB;
    Vector3 scratch;
    Vector3 t0;
    Vector3 t1;

    if (unk_8df != 0) {
        if (unk_8d0 + mPosY < -0x1f4000)
            _ZN7fBase_c18MarkForDestructionEv(this);
    }

    func_ov034_02112604(this);

    if (mVertSpeed <= 0) {
        _Z14ApproachLinearRsss(&unk_444[0].y, unk_8d8, (s16)((mHorzSpeed / 0x1000) * 0x1e));
    }

    if (mWithMeshClsn.JustHitGround() != 0) {
        mVertAccel = -0x1000;
        LandingDust(true);
    }

    if (mModelAnims[0].file == *(BCA_File **)((char *)data_ov034_02113888[0] + 4) ||
        mModelAnims[0].file == *(BCA_File **)((char *)data_ov034_02113874[0] + 4) ||
        mModelAnims[0].file == *(BCA_File **)((char *)data_ov034_0211389c[0] + 4)) {
        if (mWithMeshClsn.IsOnGround() != 0) {
            if ((((u32)mModelAnims[0].currFrame << 4) >> 16) == 6) {
                if (unk_8db == 2)
                    func_0201267c(0x10a, &mCamSpacePosX);
                else
                    func_0201267c(0x11d, &mCamSpacePosX);
            }
            if (unk_8db == 2) {
                _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(mPosX, mPosY, mPosZ);
            }
        }
    }

    animFile = data_ov034_021138b0[0];
    if (mModelAnims[0].file == *(BCA_File **)((char *)animFile + 4) &&
        (((u32)mModelAnims[0].currFrame << 4) >> 16) == 0xf) {
        func_0201267c(0x11e, &mCamSpacePosX);
    }

    for (i = 0; i < 5; i++) {
        u16 n = 4 - unk_8db;
        bool ok;
        if (n >= 3) n = 0;
        ok = false;
        if (n <= 2) ok = true;
        if (ok)
            mTextureSequences[i].currFrame = n << 12;

        if (unk_8df == 0)
            mModelAnims[i].Advance();

        if (unk_8e0 == 0) {
            if (DecIfAbove0_Byte(&unk_702[i]) != 0) {
                unk_6f8[i] += 0x1200;
                unk_408[i].y = ((s32)(((s64)data_02082214[(((u16)unk_6f8[i] >> 4) << 1) + 1] * 0x3000 + 0x800) >> 12) + 0xc000) / 10;
            } else {
                _Z14ApproachLinearRiii(&unk_408[i].y, 0x1000, 0x199);
            }
        }
    }

    if (unk_8db == 4) {
        if (unk_8de != 0)
            mTextureSequences[0].currFrame = 0x1000;
        else
            mTextureSequences[0].currFrame = 0;
    }

    mPrevAngleY = unk_444[0].y;
    UpdatePos(0);

    if (unk_8df == 0)
        UpdateWMClsn(mWithMeshClsn, 0);

    unk_3cc[0].x = mPosX;
    unk_3cc[0].y = mPosY;
    unk_3cc[0].z = mPosZ;
    func_ov034_02112874(this);

    unk_3cc[0].y = unk_3cc[1].y + 0x1000;
    head = unk_3cc[0];
    head.x += (s32)(((s64)data_02082214[((u16)unk_444[0].y >> 4) << 1] * 0x64000 + 0x800) >> 12);
    head.z += (s32)(((s64)data_02082214[(((u16)unk_444[0].y >> 4) << 1) + 1] * 0x64000 + 0x800) >> 12);
    mdCc_cs1[0].pos = head;
    mdCc_cs2[0].pos = head;

    if (mModelAnims[0].file != *(BCA_File **)((char *)animFile + 4)) {
        u32 n = (u8)(4 - unk_8db);
        if (n < 3) {
            part = unk_3cc[0];
            part.x += (s32)(((s64)data_02082214[((u16)unk_444[0].y >> 4) << 1] * 0x23000 + 0x800) >> 12);
            part.z += (s32)(((s64)data_02082214[(((u16)unk_444[0].y >> 4) << 1) + 1] * 0x23000 + 0x800) >> 12);
            if (n == 0)
                part.y += 0x89000;
            else
                part.y += 0xb9000;
            if (unk_8de != 0) {
                unk_8e4 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                    (u32)unk_8e4, data_ov034_02113828[n], part.x, part.y, part.z, 0, 0);
            }
        }
    }

    func_ov034_021129ec(this);

    {
        int j;
        for (j = 0; j < 5; j++) {
            mdCc_cs1[j].Clear();
            mdCc_cs1[j].Update();
            mdCc_cs2[j].Clear();
            mdCc_cs2[j].Update();
        }
    }

    if (unk_8e1 != 0) {
        void *cam = data_0209f318;
        camA.x = 0; camA.y = 0x80000; camA.z = 0;
        camB.x = 0; camB.y = 0x80000; camB.z = 0x1f4000;
        scratch.x = 0; scratch.y = 0; scratch.z = 0;
        Matrix4x3_FromRotationY(data_020a0e68, unk_444[0].y);
        MulVec3Mat4x3(&camA, data_020a0e68, &scratch);
        Vec3_Add(&t0, &mPosX, &scratch);
        camA = t0;
        scratch.x = 0; scratch.y = 0; scratch.z = 0;
        MulVec3Mat4x3(&camB, data_020a0e68, &scratch);
        Vec3_Add(&t1, &mPosX, &scratch);
        camB = t1;
        func_020092c4(cam, (char *)cam + 0x80, &camA);
        func_020092c4(cam, (char *)cam + 0x8c, &camB);
    }

    return 1;
}

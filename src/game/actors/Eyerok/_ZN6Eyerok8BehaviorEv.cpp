//cpp
// @symbol _ZN6Eyerok8BehaviorEv
/* Eyerok::Behavior -- vtable slot 6. Real C++ method over the shared header.
 * EVec3 is a local plain-int triple (stack temps); callees whose ROM symbols
 * carry by-value/ref class parameters keep their literal mangled extern "C"
 * spellings. */
#pragma opt_common_subs off
#pragma opt_strength_reduction off

#include "Eyerok.h"

struct EVec3 { int x, y, z; };

struct C;
typedef int (C::*PMF)();
struct State {
    char pad[8];
    PMF fn;
};

extern "C" {
extern u16 DecIfAbove0_Short(u16 *p);
extern int RandomIntInternal(int *seed);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void MulVec3Mat4x3(void *src, void *m, void *dst);
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 a, u32 b, int x, int y, int z, const void *v, void *cb);
extern void func_ov066_021194fc(char *c);
extern int _ZN4dBgW9IsEnabledEv(char *c);
extern void func_ov066_021194a4(char *c);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(char *c, void *clsn);
extern void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(char *c, const void *v);
extern void _ZN5dCc_c5ClearEv(char *c);
extern void _ZN5dCc_c6UpdateEv(char *c);
extern void _ZN14BlendModelAnim7AdvanceEv(char *c);
extern void _ZN9Animation7AdvanceEv(char *c);

extern int data_0209e650;
extern char data_ov066_0211b07c;
extern char data_020a0e68[];
extern int data_ov066_0211ad18[];
}

int Eyerok::Behavior()
{
    char *c = (char *)this;

    DecIfAbove0_Short(&mTimer1);
    DecIfAbove0_Short(&mTimer2);

    {
        State *st = *(State **)&mState;
        if (*(int *)((char *)st + 8) != 0)
            (((C *)c)->*(st->fn))();
    }

    if (mDustCounter != 0) {
        if ((mDustCounter & 1) == 0) {
            int rnd = RandomIntInternal(&data_0209e650);
            int off = (mDustCounter >> 1) * 0xc;
            int base_dc = 0x4dc;
            int base_e4 = 0x4e4;
            char *bx = c + base_dc;
            char *bz = c + base_e4;
            char *by = c + 0x4e0;
            int *px;
            int *pz;
            int *py;
            int zero;
            EVec3 vin;
            EVec3 vout;
            *(int *)(bx + off) = mPosX;
            *(int *)(by + off) = mPosY;
            *(int *)(bz + off) = mPosZ;
            px = (int *)(bx + off);
            py = (int *)(by + off);
            pz = (int *)(bz + off);
            zero = 0;
            vin.x = zero;
            vin.y = zero;
            vin.z = zero;
            vout.x = zero;
            vout.y = zero;
            vout.z = zero;
            if (mState != (void *)&data_ov066_0211b07c) {
                if (mAngleY != 0) {
                    vin.z = (0x7e - (((rnd >> 8) & 0x3f) << 2)) << 12;
                    Matrix4x3_FromRotationY(data_020a0e68, (s16)(mAngleY - 0x4000));
                    MulVec3Mat4x3(&vin, data_020a0e68, &vout);
                    *px += vout.x;
                    *pz += vout.z;
                } else {
                    if (((rnd >> 16) & 1) == 0)
                        *px += (((rnd >> 8) & 3) * 0x28) << 12;
                    else
                        *px -= (((rnd >> 8) & 3) * 0x28) << 12;
                    *pz += 0x19000;
                }
                *py += ((mDustCounter * 0xa) + 0x23) << 12;
            } else {
                if (mAngleY != 0) {
                    vin.z = (0x7e - (((rnd >> 8) & 0x3f) << 2)) << 12;
                    Matrix4x3_FromRotationY(data_020a0e68, (s16)(mAngleY - 0x4000));
                    MulVec3Mat4x3(&vin, data_020a0e68, &vout);
                    *px += vout.x;
                    *pz += vout.z;
                } else {
                    int a = ((rnd >> 24) & 7) * 0x1e;
                    int b = ((rnd >> 16) & 7) * 0x1e;
                    *pz -= 0x64000;
                    *px += (0x69 - a) << 12;
                    *pz += (0x69 - b) << 12;
                }
                *py += 0x96000;
            }
        }

        {
            int i = 0;
            char *cur = c;
            u32 id0 = 0x13a;
            u32 id1 = 0x13b;
            int z0 = 0;
            for (; i < 0x14; i++) {
                if (*(int *)(cur + 0x4dc) != 0 || *(int *)(cur + 0x4e0) != 0 || *(int *)(cur + 0x4e4) != 0) {
                    mDustParticle1[i] =
                        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                            mDustParticle1[i], id0,
                            *(int *)(cur + 0x4dc), *(int *)(cur + 0x4e0), *(int *)(cur + 0x4e4),
                            (void *)z0, (void *)z0);
                    mDustParticle2[i] =
                        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                            mDustParticle2[i], id1,
                            *(int *)(cur + 0x4dc), *(int *)(cur + 0x4e0), *(int *)(cur + 0x4e4),
                            (void *)z0, (void *)z0);
                }
                cur += 0xc;
            }
        }

        {
            /* c400 + 0xd4 is mDustCounter reached the long way round -- the
               ROM materialises c + 0x400 first and offsets from it, and
               spelling that step away is not free. */
            int o4d4 = 0x4d4;
            u16 *p = &mDustCounter;
            u16 v = *p;
            char *c400 = c + 0x400;
            *p = (u16)(v + 1);
            if (*(u16 *)(c400 + 0xd4) > 0x26) {
                int j = 0;
                char *q = c;
                *(u16 *)(c400 + 0xd4) = (u16)j;
                for (; j < 0x14; j++) {
                    *(int *)(q + 0x4dc) = 0;
                    *(int *)(q + 0x4e0) = 0;
                    *(int *)(q + 0x4e4) = 0;
                    q += 0xc;
                }
            }
        }
    }

    if (mPartIdx == 0) {
        func_ov066_021194fc(c);
        if (_ZN4dBgW9IsEnabledEv((char *)&mMeshCollider2) != 0)
            func_ov066_021194a4(c);
        return 1;
    }

    {
        EVec3 vrel;
        mRestPosY = mSpawnPosY + 0x8000;
        _ZN8dActor_c9UpdatePosEP5dCc_c(c, 0);
        mdCcAcPos_c.pos.x = mPosX;
        mdCcAcPos_c.pos.y = mPosY;
        mdCcAcPos_c.pos.z = mPosZ;
        vrel.x = data_ov066_0211ad18[0];
        vrel.y = data_ov066_0211ad18[1];
        vrel.z = data_ov066_0211ad18[2];
        _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3((char *)&mdCcAcPos_c, &vrel);
        func_ov066_021194fc(c);
        if (_ZN4dBgW9IsEnabledEv((char *)&mMeshCollider2) != 0)
            func_ov066_021194a4(c);
        _ZN5dCc_c5ClearEv((char *)&mdCcAcPos_c);
        _ZN5dCc_c6UpdateEv((char *)&mdCcAcPos_c);
        _ZN14BlendModelAnim7AdvanceEv((char *)&mBlendModelAnim);
        _ZN9Animation7AdvanceEv((char *)&mTextureSequence);
    }
    return 1;
}

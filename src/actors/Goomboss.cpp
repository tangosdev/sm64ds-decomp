//cpp
/* Goomboss, the goomba-king boss. ROM RTTI is daKuriKing_c
 * (_ZTI12daKuriKing_c at ov074:0x02122e30); this tree keeps the coined name.
 * Partial TU: the unmatched draft func_ov074_02121380 (size 0x374, no
 * delinks entry) holes the run, so this file is only the lower side, the
 * licensed run 0x0211f154..0x02121380 (36 functions). D1 is the first
 * byte of ov074 .text, so there is no room below it for the forcing
 * scaffold; D1 and D0 stay in their own shards and this TU emits no vtable.
 *
 * Source is REVERSE of ROM order (highest address first). Do not reorder.
 *
 * Leftover: the func_ov074 helpers keep linker names. State dispatch is
 * still a pointer-to-member on a non-virtual stand-in, because Goomboss's
 * own member pointer is a different size. The matrix at 0x37c stays a cast;
 * a Matrix4x3 member would run ~Vector3 from ~Goomboss.
 */

/* common.h BEFORE Goomboss.h: with the structured Matrix4x3,
 * func_ov074_02121270/02121300 go long on whole-matrix assignment. */
#include "common.h"
#include "Goomboss.h"
#include "types.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "MaterialChanger.h"
#include "TextureSequence.h"
#include "dBgCh_Gnd.h"
#include "Player.h"
#include "daKrb_c.h"

#define AT(p,off) ((void*)(int)((char*)(p)+(off)))
#define LI(v) ((int)((long long)(v)))

extern "C" {
extern void _ZN6Camera9SetFlag_3Ev(void* cam);
extern void Matrix4x3_FromRotationY(void* m, short ang);
extern void MulVec3Mat4x3(void* a, void* m, void* b);
extern char* data_0209f318;
extern "C" int RandomIntInternal(int *seed);
extern "C" int data_0209e650;
extern u16 DecIfAbove0_Short(u16* p);
extern s16 Vec3_VertAngle(const struct Vector3* v1, const struct Vector3* v0);
extern s16 data_02082214[];
extern void func_02012694(u32 a, void* b);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, int x, int y, int z);
extern void _ZN5Sound22StopLoadedMusic_Layer3Ev();
extern void func_02011cfc();
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(u32 a, int f);
extern int func_01ffa344(int);
extern unsigned short data_ov074_02122e04[];
extern int ApproachAngle(void*,int,int,int,int);
void func_ov084_021296b0(void* a, void* b);
int __aeabi_idiv(int a, int b);
extern int AngleDiff(int a, int b);
extern void _ZN5Sound22LoadAndSetMusic_Layer3Ej(unsigned int id);
extern int func_01ffa594(int a, int b);
extern int func_01ffa4bc(int a);
extern int func_01ff9378(int a, int b);
extern int _ZN4cstd4fdivEii(int a, int b);
extern int Math_Function_0203b14c(int* p, int a, int b, int d, int e);
extern unsigned short data_ov074_02122dfc[];
extern int data_ov074_0212291c[];
extern void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
extern s32 Vec3_HorzLen(const Vector3 *v);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(char *p, const struct Vector3 *v, unsigned int a, int b, unsigned int c, unsigned int d, unsigned int e);
extern s16 Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
extern void MulMat4x3Mat4x3(void* out, void* a, void* b);
extern void Vec3_LslInPlace(void* v, int n);
extern struct Matrix4x3 data_020a0e68;
extern struct Matrix4x3 IDENTITY_MATRIX4X3;
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* anim, void* file, int a, int b, unsigned int u);
void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void* tt, void* file, int a, int b, unsigned int u);
extern char data_ov074_02122f3c[];
extern char data_ov074_02122f38[];
void UnloadKeyModels(int i);
extern char data_ov002_0210da30;
extern char data_ov084_02130cf8;
extern char data_ov074_02123000;
extern char data_ov074_02123040;
void LoadKeyModels(int idx);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
void *self, void *actor, void *pos, s32 fx, s32 fy, u32 a, u32 b);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, void *f, int a, s32 fix, u32 c);
void _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(void *self, void *f, int a, s32 fix, u32 c);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
void *self, void *actor, s32 fa, s32 fb, void *v0, void *v1);
extern void *data_ov074_0212292c[];
extern void *data_ov074_02122948[];
extern void *data_ov074_02123030;
extern void *func_021123f4;
extern void *data_ov074_02122e5c[];
extern void *func_021124ac;
extern void Vec3_Asr(Vector3* d, Vector3* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
}


/* Forward declarations for every member of this TU: mwccarm lays .text down
 * in reverse source order, so this file is written descending-ROM and nearly
 * every intra-TU call is a forward reference. */
extern "C" {
void func_ov074_02121300(char* c);
void func_ov074_02121270(char* r4, Goomboss* r8, int r7);
void func_ov074_02120d74(char *a);
void func_ov074_02120bb8(char *self, char *p1, char *p2, int mode);
void func_ov074_02120b90(Goomboss *c);
int func_ov074_02120b24(Goomboss* c);
void func_ov074_0212087c(int* out, Goomboss* self, int n_);
void func_ov074_02120808(Goomboss* c);
int func_ov074_021207b8(Goomboss* self);
int func_ov074_021206c8(Goomboss *c);
int func_ov074_021204c0(Goomboss* c);
int func_ov074_02120474(Goomboss* c);
void func_ov074_0212042c(char *cc);
void func_ov074_021203e4(char *cc, int i);
void func_ov074_021203e0(void);
void func_ov074_021201f0(Goomboss* c);
void func_ov074_021201ec(void);
void func_ov074_0212018c(Goomboss *c);
void func_ov074_0212016c(Goomboss *c);
void func_ov074_02120080(Goomboss* c);
void func_ov074_0212007c(void);
void func_ov074_0211ffcc(Goomboss* c);
void func_ov074_0211ffac(Goomboss* c);
void func_ov074_0211fd74(Goomboss* self);
void func_ov074_0211fd48(Goomboss *c);
void func_ov074_0211fc38(Goomboss* c);
void func_ov074_0211fc34(void);
int func_ov074_0211fbd0(Goomboss* c);
void func_ov074_0211fb84(Goomboss* c);
void func_ov074_0211fa74(Goomboss* c);
void func_ov074_0211fa08(Goomboss* c);
void func_ov074_0211f5b8(Goomboss* c);
int func_ov074_0211f38c(Goomboss* c);
void func_ov074_0211f344(Goomboss* self, unsigned char value);
void func_ov074_0211f244(Goomboss *self);
void func_ov074_0211f154(Goomboss* c);
extern int func_ov074_02121a20(void* c, int idx);
extern int func_ov074_021216f4(void* c);
}

// @symbol func_ov074_02121300
#include "common.h"
#include "Camera.h"
#include "Message.h"
extern "C" {


extern void Matrix4x3_ApplyInPlaceToRotationY(void* m, short a);
extern void Matrix4x3_ApplyInPlaceToScale(void* m, int x, int y, int z);
extern struct Matrix4x3 data_020a0e68;
void func_ov074_02121300(char* raw){
  Goomboss* c = (Goomboss*)raw;
  struct Vector3 v;
  Vec3_Asr(&v, (struct Vector3*)&c->mPosX, 3);
  Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
  Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, c->mAngleY);
  Matrix4x3_ApplyInPlaceToScale(&data_020a0e68, c->mScaleX, c->mScaleY, c->mScaleZ);
  *(struct Matrix4x3*)((char*)c + 0x37c) = data_020a0e68;
}
}

// @symbol func_ov074_02121270
extern "C" {
void func_ov074_02121270(char* r4, Goomboss* r8, int r7){
  char* dst;
  *(int*)r4 = 0;
  *(int*)(r4 + 4) = 0;
  *(int*)(r4 + 8) = 0;
  data_020a0e68 = *(struct Matrix4x3*)((char*)r8 + 0x37c);
  dst = (char*)r8->mModelAnim.data.transforms + r7 * 0x30;
  MulMat4x3Mat4x3(dst, &data_020a0e68, &data_020a0e68);
  *(int*)r4 = *(int*)((char*)&data_020a0e68 + 0x24);
  *(int*)(r4 + 4) = *(int*)((char*)&data_020a0e68 + 0x28);
  *(int*)(r4 + 8) = *(int*)((char*)&data_020a0e68 + 0x2c);
  Vec3_LslInPlace(r4, 3);
}
}

// @symbol func_ov074_02120d74
extern "C" {
void func_ov074_02120d74(char *raw)
{
    Goomboss *a = (Goomboss *)raw;
    struct Vector3 v0;
    struct Vector3 v1;
    struct Vector3 v2;
    struct Vector3 v3;
    int i;
    dActor_c *player;
    dCcAcPos_c *cur;
    dCcAcPos_c *p;
    int hit;
    int dir;
    int dir2;
    u32 s;
    u32 flags;
    u16 type;
    int b;
    int b2;

    DecIfAbove0_Byte(&a->mHurtTimer);
    s = a->mState;
    if (s == 1)
        return;
    if (s == 2)
        return;
    if (s - 5 <= 3)
        return;

    player = 0;
    i = 0;
    p = a->mdCc_cs;
    do {
        cur = p;
        if (cur->otherOwner != 0)
            player = (dActor_c *)dActor_c::FindWithID(cur->otherOwner);
        if (player != 0)
            break;
        i++;
        p += 1;
    } while (i < 4);
    if (player == 0)
        return;

    flags = cur->hitFlags;
    hit = 0;
    if (flags & 0x2000) {
        hit = 1;
    } else {
        type = player->actorID;
        b = (int)(type == 0xbf);
        if (b != 0) {
            Player *pl = (Player *)player;
            if (a->mHurtTimer == 0) {
                if (!(flags & 0x26fe0) && *(u8 *)&pl->mIsTakingDamage == 0) {
                    if (cur == &a->mdCc_cs[2]) {
                        if (a->mWalkSpeed != 0) {
                            ((Player *)((char *)pl))->Unk_020c6a10(2);
                            a->mHurtTimer = 0x1e;
                        } else {
                            v1.x = a->mPosX;
                            v1.y = a->mPosY;
                            v1.z = a->mPosZ;
                            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj((char *)pl, &v1, 1, 0x5000, 1, 0, 1);
                        }
                    } else if (cur == &a->mdCc_cs[3]) {
                        if (a->mWalkSpeed != 0) {
                            ((Player *)((char *)pl))->Unk_020c6a10(2);
                            a->mHurtTimer = 0x1e;
                        } else {
                            v2.x = a->mPosX;
                            v2.y = a->mPosY;
                            v2.z = a->mPosZ;
                            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj((char *)pl, &v2, 1, 0x5000, 1, 0, 1);
                        }
                    } else {
                        v3.x = a->mPosX;
                        v3.y = a->mPosY;
                        v3.z = a->mPosZ;
                        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj((char *)pl, &v3, 1, 0x5000, 1, 0, 1);
                        a->mHurtTimer = 0x10;
                    }
                }
                if (pl->mIsMega != 0) {
                    if (cur->hitFlags & 0x3c0) {
                        hit = 1;
                    } else {
                        int ang = Vec3_HorzAngle((struct Vector3 *)&a->mPosX, (struct Vector3 *)&pl->mPosX);
                        dir = 1;
                        if (AngleDiff(ang, a->mAngleY) > 0x4000)
                            dir = -1;
                        func_ov074_02121a4c((char *)a, 7);
                        func_ov074_021203e4((char*)a, 8);
                        func_ov074_02120bb8((char*)a, (char*)pl, (char*)cur, 0);
                        func_02012694(0x15a, &a->mCamSpacePosX);
                        a->mWalkSpeed = dir * (a->mDirection * -0xc8);
                    }
                } else {
                    if (cur == &a->mdCc_cs[1]) {
                        if (cur->hitFlags & 0x80) {
                            if (a->mState == 4)
                                func_ov074_021203e4((char*)a, 2);
                            else
                                func_ov074_021203e4((char*)a, 4);
                            if ((int)pl->param1 == 0) {
                                v0.x = a->mPosX;
                                v0.y = a->mPosY;
                                v0.z = a->mPosZ;
                                if (a->mMegaMushroomID == 0 && dActor_c::FindWithID(a->mMegaMushroomID) == 0) {
                                    int k = ((u16)(s16)(a->mDirection * ((s16)((a->mScaleX / 0x1000) * 0x350 + 0x500) + 0x600) + (a->mWalkAngle + a->mMaxWalkSpeed))) >> 4;
                                    v0.x = (int)(((long long)data_02082214[k * 2] * 0x546000LL + 0x800) >> 12);
                                    v0.y += 0x32000;
                                    v0.z = (int)(((long long)data_02082214[k * 2 + 1] * 0x546000LL + 0x800) >> 12);
                                    a->mMegaMushroomID = ((fBase_c *)dActor_c::Spawn(0x115, 0, v0, 0, a->mAreaId, -1))->uniqueID;
                                }
                            }
                        }
                    }
                }
            }
        } else {
            b2 = (int)(type == 0xc8);
            if (b2 != 0) {
                if (flags & 0x22000)
                    hit = 1;
            }
        }
    }
    if (hit == 0)
        return;
    func_02012694(0x15b, &a->mCamSpacePosX);
    {
    int ang2 = Vec3_HorzAngle((struct Vector3 *)&a->mPosX, (struct Vector3 *)&player->mPosX);
    dir2 = 1;
    if (AngleDiff(ang2, a->mAngleY) > 0x4000) {
        func_ov074_02121a4c((char *)a, dir2);
        dir2 = -1;
    } else {
        func_ov074_02121a4c((char *)a, 2);
    }
    }
    a->mWalkSpeed = dir2 * (a->mDirection * -0x190);
    func_ov074_021203e4((char*)a, 5);
    func_ov074_02120bb8((char*)a, (char*)player, (char*)cur, 1);
}
}

// @symbol func_ov074_02120bb8
extern "C" {
void func_ov074_02120bb8(char *raw, char *p1raw, char *p2raw, int mode)
{
    Goomboss *self = (Goomboss *)raw;
    dActor_c *p1 = (dActor_c *)p1raw;
    dCcAcPos_c *p2 = (dCcAcPos_c *)p2raw;
    Vector3 v, dst, src, delta;
    s32 aX, aY;
    s32 iX, iY;
    s32 s, w;

    dst.x = self->mPosX;
    dst.y = self->mPosY;
    dst.z = self->mPosZ;
    {
        Vector3 *pv = (Vector3*)&p1->mPosX;
        src.x = pv->x;
        src.y = pv->y;
        src.z = pv->z;
    }
    {
        s32 dy = p2->height;
        w = p2->radius;
        dst.y += dy;
    }
    src.y += 0x46000;
    Vec3_Sub(&delta, &src, &dst);

    v.x = delta.x;
    v.y = delta.y;
    v.z = delta.z;
    aX = _ZN4cstd5atan2E5Fix12IiES1_(v.x, v.z);
    aY = _ZN4cstd5atan2E5Fix12IiES1_(v.y, Vec3_HorzLen(&v));

    iY = (u16)aY >> 4;
    iX = (u16)aX >> 4;

    s = (s32)(((long long)w * data_02082214[iY * 2 + 1] + 0x800) >> 12);
    dst.x += (s32)(((long long)s * data_02082214[iX * 2] + 0x800) >> 12);
    dst.y += (s32)(((long long)w * data_02082214[iY * 2] + 0x800) >> 12);
    dst.z += (s32)(((long long)s * data_02082214[iX * 2 + 1] + 0x800) >> 12);

    switch (mode) {
    case 0:
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xf6, dst.x, dst.y, dst.z);
        break;
    case 1:
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, dst.x, dst.y, dst.z);
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, dst.x, dst.y, dst.z);
        break;
    }
}
}

// @symbol func_ov074_02120b90
extern "C" {
void func_ov074_02120b90(Goomboss *c) {
    int idx = c->mSizeIndex;
    int base = c->mDirection;
    unsigned short scale = data_ov074_02122dfc[idx];
    c->mMaxWalkSpeed = base * scale;
}
}

// @symbol func_ov074_02120b24
extern "C" {
int func_ov074_02120b24(Goomboss* c)
{
    int num = c->mWalkSpeed << 12;
    int den;
    int r;
    if (num < 0) num = -num;
    den = data_ov074_0212291c[c->mSizeIndex] + (data_ov074_02122dfc[3] << 12) + 1;
    r = _ZN4cstd4fdivEii(num, den);
    if (r < 0x1000) return 0x1000;
    if (r > 0x3000) return 0x3000;
    return r;
}
}

// @symbol func_ov074_0212087c
extern "C" {
void func_ov074_0212087c(int* out, Goomboss* self, int n_)
{
    int n;

    n = LI(n_);

    out[0] = self->mPosX;
    out[1] = self->mPosY;
    out[2] = self->mPosZ;

    if (*(int*)((char *)self->ClosestPlayer() + 8) == 3) {
        int rem;
        int quo;
        int scale;
        int q;
        int sum;
        int prod;
        int diff;
        int idx;
        u8 mode;

        rem = n % 3;
        quo = LI(n) / 3;
        mode = self->mSizeIndex;
        if (mode == 3) {
            scale = 0x546000;
        } else if (mode == 2) {
            scale = quo * 0xb4000 + 0x4ec000;
        } else {
            scale = quo * 0xb4000 + 0x438000;
        }

        q = self->mScaleX / 0x1000;
        sum = (s16)(q * 0x350 + 0x500) + (s16)(rem * 0x600);
        prod = self->mDirection * sum;
        diff = (self->mWalkAngle + self->mMaxWalkSpeed) - prod;
        idx = ((u16)(s16)diff >> 4) * 2;

        out[0] = (int)(((s64)scale * data_02082214[idx] + 0x800) >> 12);
        *(int*)AT(out, 4) += 0x32000;
        out[2] = (int)(((s64)scale * data_02082214[idx + 1] + 0x800) >> 12);
    } else {
        int lim;
        int one;
        int sign;
        int rem2;
        int scale2;
        int q2;
        int sum2;
        int prod2;
        int prod3;
        int diff2;
        int idx2;

        lim = 3;
        if (self->mSizeIndex == 1)
            lim = 5;

        if (n < lim) {
            one = 1;
            sign = -1;
            rem2 = n % lim;
            scale2 = 0x4e2000 + (rem2 * 0x64 << 12);
        } else {
            one = 1;
            sign = one;
            rem2 = n % lim;
            scale2 = 0x4e2000 + (rem2 * 0x64 << 12);
        }

        q2 = self->mScaleX / 0x1000;
        sum2 = (s16)(q2 * 0x350 + 0x250) + (s16)(one * 0x600);
        prod2 = self->mDirection * sum2;
        prod3 = sign * prod2;
        diff2 = (self->mWalkAngle + self->mWalkSpeed) - prod3;
        idx2 = ((u16)(s16)diff2 >> 4) * 2;

        out[0] = (int)(((s64)scale2 * data_02082214[idx2] + 0x800) >> 12);
        *(int*)AT(out, 4) += 0x32000;
        out[2] = (int)(((s64)scale2 * data_02082214[idx2 + 1] + 0x800) >> 12);
    }
}
}

// @symbol func_ov074_02120808
extern "C" void func_ov074_02120808(Goomboss* c)
{
    int ang = (short)(c->mWalkAngle + c->mDirection * 0x4500);
    if (AngleDiff(ang, ((dActor_c *)((char*)c))->HorzAngleToCPlayer()) <= 0x4000)
        return;
    func_ov074_02121a4c((char*)c, 6);
    c->mDirection *= -1;
}

// @symbol func_ov074_021207b8
/* If anim 6 is current and mModelAnim has finished, flip mAngleY by half
 * a turn and return 1. Otherwise return 0.
 */
extern "C" {
int func_ov074_021207b8(Goomboss* self)
{
    if (func_ov074_02121a20(self, 6)) {
        if (self->mModelAnim.Finished()) {
            self->mAngleY += 0x8000;
            return 1;
        }
    }
    return 0;
}
}

// @symbol func_ov074_021206c8
extern "C" {
int func_ov074_021206c8(Goomboss *c) {
    if (func_ov074_02121a20(c, 8) != 0) {
        if (ApproachAngle(&c->mWalkSpeed, 0, 0x1e, 0x20, 1) == 0) {
            if ((unsigned short)(c->mModelAnim.currFrame >> 12) == 0) {
                func_ov074_02121a4c((char*)c, 9);
            }
            goto ret0;
        }
        func_ov074_021216f4(c);
        goto ret0;
    }
    if (func_ov074_02121a20(c, 0xa) != 0) {
        func_ov074_02121a4c((char*)c, 9);
        goto ret0;
    }
    if (func_ov074_02121a20(c, 9) != 0) {
        if (c->mModelAnim.Finished() != 0) {
            return 1;
        }
        goto ret0;
    }
    return 1;
ret0:
    return 0;
}
}

// @symbol func_ov074_021204c0
/* recovered: Goomboss (daKuriKing_c) shrink/grow interpolation tick.
 *
 * Reached from func_ov074_0211f860 and func_ov074_0211fb44. Picks a pair of
 * float sizes out of data_ov074_02122e4c (12288.0f, 9557.0f, 6826.0f,
 * 4096.0f -- Fix12 scales stored as IEEE singles), divides their difference
 * by the remaining step count in mScaleSteps, and steps mTargetScale toward
 * the next one. Math_Function_0203b14c eases mCurrentScale toward that target;
 * while it is still moving, the scale triple is driven from a sin/cos pair
 * out of data_02082214 indexed by mScalePhase, which advances 0x1230 per
 * frame. Every step below the last also spits a 0xb1 particle a fixed Fix12
 * offset above the boss.
 *
 * The float arithmetic goes through the ITCM soft-float block by hand, the
 * way the rest of this tree calls it: func_01ffa594 (single add/sub),
 * func_01ffa344 (float to int), func_01ffa4bc (int to float) and
 * func_01ff9378, with cstd::fdiv doing the Fix12 divide in between.
 *
 * Shape notes: the particle position temp must stay MEMORY-HOMED -- reading
 * its words back through the address (the ((int *)&v)[n] form) is what keeps
 * the three dead stores the ROM emits; plain member reads scalarize the
 * struct away and cost four instructions. And k has to be computed BEFORE d,
 * which is worth ten words of register identity in the tail: with d first,
 * the index web takes r2 and the mCurrentScale web takes r3, exactly reversed
 * from the ROM.
 *
 * Matched byte-for-byte with mwccarm 2004/b56 (ov074); linkcheck VERIFIED.
 */
extern "C" {
int func_ov074_021204c0(Goomboss* c) {
    struct Vector3 v;
    int d, k;

    c->mTargetScale = func_01ffa344(func_01ff9378(
        (int)data_ov074_02122e4c[c->mSizeIndex + 1],
        func_01ffa4bc(_ZN4cstd4fdivEii(
            func_01ffa344(func_01ffa594((int)data_ov074_02122e4c[c->mSizeIndex],
                                        (int)data_ov074_02122e4c[c->mSizeIndex + 1])),
            c->mScaleSteps << 12))));

    if (Math_Function_0203b14c(&c->mCurrentScale, c->mTargetScale, 0x78, 0x100, 0x20) == 0) {
        if (c->mScalePhase == 0) {
            c->mScaleX = c->mCurrentScale;
            c->mScaleY = c->mCurrentScale;
            c->mScaleZ = c->mCurrentScale;
        }
        if (c->mScaleSteps <= 1) return 1;
        c->mScaleSteps--;
        if (c->mState == 6) {
            v.x = c->mPosX;
            v.y = c->mPosY;
            v.z = c->mPosZ;
            v.y += (int)(((long long)c->mCurrentScale * (long long)0x190000 + 0x800) >> 12);
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb1, ((int*)&v)[0], ((int*)&v)[1], ((int*)&v)[2]);
        }
    }
    k = ((u16)c->mScalePhase >> 4) * 2;
    d = c->mTargetScale - c->mCurrentScale + 0x11e;
    c->mScaleY = c->mCurrentScale + (int)(((long long)d * data_02082214[k] + 0x800) >> 12);
    c->mScaleX = c->mCurrentScale;
    c->mScaleX = c->mCurrentScale + (int)(((long long)d * data_02082214[k + 1] + 0x800) >> 12);
    c->mScaleZ = c->mScaleX;
    c->mScalePhase += 0x1230;
    return 0;
}
}

// @symbol func_ov074_02120474
extern "C" {
int func_ov074_02120474(Goomboss* c) {
    c->mMaterialChanger.Advance();
    unsigned char idx = c->mSizeIndex;
    unsigned int val = c->mMaterialChanger.currFrame;
    unsigned short tbl = data_ov074_02122e04[idx];
    unsigned int v = (val << 4) >> 0x10;
    if (v >= tbl) {
        c->mMaterialChanger.currFrame = (unsigned int)tbl << 0xc;
        return 1;
    }
    return 0;
}
}

// @symbol func_ov074_0212042c
/* data_ov074_021230f8 is copied as 8-byte records (__sinit_ov074). Goomboss
 * has virtuals, so a Goomboss::* is wider than that; the call stays on this
 * stand-in. The word it reads is mState. */
struct GoombossPmf;
typedef void (GoombossPmf::*GoombossPMF)();
struct GoombossPmfEntry { GoombossPMF pmf[2]; };
extern GoombossPmfEntry data_ov074_021230f8[];
struct GoombossPmf { char pad[0x5cc]; s32 mState; };
extern "C" void func_ov074_0212042c(char *raw) {
    Goomboss *self = (Goomboss *)raw;
    GoombossPmf *c = (GoombossPmf *)self;
    int j = self->mState;
    (c->*data_ov074_021230f8[j].pmf[1])();
}

// @symbol func_ov074_021203e4
extern "C" void func_ov074_021203e4(char *raw, int i) {
    Goomboss *self = (Goomboss *)raw;
    self->mState = i;
    int j = self->mState;
    GoombossPmf *c = (GoombossPmf *)self;
    (c->*data_ov074_021230f8[j].pmf[0])();
}

// @symbol func_ov074_021203e0
extern "C" {
void func_ov074_021203e0(void)
{
}
}

// @symbol func_ov074_021201f0
/* recovered: Goomboss (daKuriKing_c) state-0 tick -- the intro cutscene.
 *
 * Frames the boss for the opening conversation: pins the camera to a fixed
 * look-at/eye pair derived from the boss position, turns the closest player
 * around to face him (angle + 0x7fff), snaps the boss's own facing to
 * mDirection * 0x4500 + mWalkAngle, then runs the talk handshake. Player::StartTalk
 * and Player::ShowMessage each gate the rest of the tick, so a refused talk
 * simply retries next frame; on success the music layer flips once (guarded
 * by mMusicStarted) and the state advances to 1.
 *
 * Shape notes: the angle temp is an Ang16 STRUCT COPY, not three field
 * assignments -- mwccarm scalarizes the field form away and drops the
 * memory home the ROM keeps at sp+8. msgpos[0] is filled through a base
 * pointer (the ov073 idiom) while msgpos[1] uses direct offsets; that
 * difference is in the ROM, not an accident. msgpos[0] is dead, kept alive
 * only because &msgpos[1] homes the whole array.
 *
 * Matched byte-for-byte with mwccarm 2004/b56 (ov074); linkcheck VERIFIED.
 */
extern "C" {
void func_ov074_021201f0(Goomboss* c) {
    struct Ang16 { short v[3]; };
    struct Ang16 ang;
    struct Vector3 look, pos;
    struct Vector3 msgpos[2];
    Player* player;
    void* cam;
    short msg;

    player = (Player*)c->ClosestPlayer();
    cam = data_0209f318;
    _ZN6Camera9SetFlag_3Ev(cam);

    look.x = c->mPosX;
    look.y = c->mPosY;
    look.z = c->mPosZ;
    pos.x = c->mPosX;
    pos.y = c->mPosY;
    pos.z = c->mPosZ;
    look.x += 0x130000;
    look.y = 0x1cd000;
    look.z -= 0x2a0000;
    pos.x -= 0x2f0000;
    pos.y += 0x20000;
    pos.z += 0x254000;
    ((Camera *)cam)->SetLookAt(look);
    ((Camera *)cam)->SetPos(pos);

    ang = *(struct Ang16*)&c->mAngleX;
    ang.v[1] += 0x7fff;
    player->mAngleX = ang.v[0];
    player->mAngleY = ang.v[1];
    player->mAngleZ = ang.v[2];

    {
        int pv = (int)&player->mPosX;
        msgpos[0].x = *(int*)pv;
        msgpos[0].y = *(int*)(pv + 4);
        msgpos[0].z = *(int*)(pv + 8);
    }
    msgpos[1].x = c->mPosX;
    msgpos[1].y = c->mPosY;
    msgpos[1].z = c->mPosZ;
    msgpos[1].y += 0x78000;

    c->mAngleY = c->mDirection * 0x4500 + c->mWalkAngle;
    c->mTalkPlayer = player;
    if (player->StartTalk(*c, 1) == 0) return;
    Message::PrepareTalk();
    if (c->mMusicStarted == 0) {
        _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2c);
        c->mMusicStarted = 1;
    }
    msg = (unsigned short)((int)player->param1 + 0xd3);
    if (player->ShowMessage(*c, msg, &msgpos[1], 0, 2) == 0) return;
    func_02012694(0x15a, &c->mCamSpacePosX);
    func_ov074_021203e4((char*)c, 1);
}
}

// @symbol func_ov074_021201ec
extern "C" {
void func_ov074_021201ec(void)
{
}
}

// @symbol func_ov074_0212018c
extern "C" {
extern void func_02011d20(void);
extern char *data_0209f318;
void func_ov074_0212018c(Goomboss *c)
{
    if (((Player *)c->mTalkPlayer)->GetTalkState() != -1)
        return;
    (*(int *)(((int)data_0209f318 + 0x154))) &= ~8;
    _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2d);
    func_02011d20();
    Message::EndTalk();
    func_ov074_021203e4((char*)c, 2);
}
}

// @symbol func_ov074_0212016c
extern "C" {
void func_ov074_0212016c(Goomboss *c) {
    func_ov074_02121a4c((char*)c, 0xa);
    func_ov074_02120b90(c);
}
}

// @symbol func_ov074_02120080
extern "C" {
void func_ov074_02120080(Goomboss* c){
  if(c->mSpawnedCount == 0){
    func_ov074_021203e4((char*)c, 2);
    return;
  }
  if(func_ov074_02121a20(c, 0xa) != 0){
    if(c->mModelAnim.Finished() == 0) return;
    func_ov074_02121a4c((char*)c, 8);
    return;
  }
  ApproachAngle(&c->mWalkSpeed, c->mMaxWalkSpeed, 0xa, 0x20, 1);
  func_ov074_021216f4(c);
  if(((dActor_c *)((char*)c))->DistToCPlayer() >= 0x4b0000) return;
  if(AngleDiff(((dActor_c *)((char*)c))->HorzAngleToCPlayer(), c->mAngleY) <= 0x4000) return;
  func_ov074_021203e4((char*)c, 4);
}
}

// @symbol func_ov074_0212007c
extern "C" {
void func_ov074_0212007c(void)
{
}
}

// @symbol func_ov074_0211ffcc
extern "C" {
void func_ov074_0211ffcc(Goomboss* c) {
  if (func_ov074_02121a20(c, 5) == 0) {
    if (func_ov074_021206c8(c) == 0) return;
    func_ov074_02121a4c((char*)c, 5);
    return;
  }
  if (((dActor_c *)((char*)c))->DistToCPlayer() >= 0x4b0000) {
    func_ov074_021203e4((char*)c, 3);
  } else {
    if (AngleDiff(((dActor_c *)((char*)c))->HorzAngleToCPlayer(), c->mAngleY) < 0x3000) {
      func_ov074_021203e4((char*)c, 3);
      return;
    }
  }
  if (c->mSpawnedCount != 0) return;
  func_ov074_021203e4((char*)c, 2);
}
}

// @symbol func_ov074_0211ffac
extern "C" {
void func_ov074_0211ffac(Goomboss* c) {
    func_ov074_02120808(c);
    c->mTimer = 0;
}
}

// @symbol func_ov074_0211fd74
extern "C" void func_ov074_0211fd74(Goomboss* self)
{
    if (func_ov074_02121a20(self, 6) != 0) {
        if (func_ov074_021207b8(self) == 0) return;
        func_ov074_02121a4c((char*)self, 0xb);
        return;
    } else {
        if (func_ov074_02121a20(self, 0xb) != 0) goto L90;
        if (func_ov074_021206c8(self) == 0) return;
        func_ov074_02121a4c((char*)self, 0xb);
        return;
    }

L90:;
    int spawnTarget = data_ov074_02122d80[self->mSizeIndex];
    Player* pl = (Player*)self->ClosestPlayer();
    if (self->mSizeIndex == 1 && (int)pl->param1 != 3) spawnTarget = spawnTarget + 1;

    if (self->mModelAnim.Finished() == 0) return;
    if (DecIfAbove0_Short(&self->mTimer) != 0) return;

    Vector3 pos;
    func_ov074_0212087c((int*)&pos, self, self->mSpawnedCount);
    Vector3 pp;
    pp.x = pos.x;
    pp.y = pos.y;
    pp.z = pos.z;
    self->DisappearPoofDustAt(pp);

    u32 sid = 0xeeee;
    if ((int)pl->param1 != 3) {
        if (self->mSpawnedCount % 2 == 0) sid = 0xeeef;
    }

    daKrb_c* sp2 = (daKrb_c*)dActor_c::Spawn(
        0xc8, sid, pos, (const struct Vector3_16*)&self->mAngleX, self->mAreaId, -1);
    sp2->mTargetUniqueID = self->uniqueID;
    Vector3 v2;
    v2.x = self->mSpawnPosX;
    v2.y = self->mSpawnPosY;
    v2.z = self->mSpawnPosZ;
    func_ov084_021296b0(sp2, &v2);

    *(u8*)((char*)sp2 + 0x474) = self->mSpawnedCount;
    /* Same pool-address increment as mSubState: 0x602 is not an ARM immediate. */
    u8* p602 = (u8*)(((int)&self->mSpawnedCount));
    *p602 = (u8)(*p602 + 1);
    self->mTimer = 2;

    Player* pl2 = (Player*)self->ClosestPlayer();
    int dv = 3;
    if ((int)pl2->param1 != 3) {
        if (self->mSizeIndex == 1) dv = 5;
    }
    if (self->mSpawnedCount % dv == 0) self->mModelAnim.currFrame = 0;

    if (self->mSpawnedCount < spawnTarget) return;
    func_ov074_021203e4((char*)self, 3);
}

// @symbol func_ov074_0211fd48
extern "C" {
void func_ov074_0211fd48(Goomboss *c)
{
    unsigned char value;

    c->mSizeIndex--;
    value = c->mSizeIndex;
    if (value > 5)
        value = 5;
    c->mSizeIndex = value;
}
}

// @symbol func_ov074_0211fc38
extern "C" {
void func_ov074_0211fc38(Goomboss* c)
{
    if (ApproachAngle(&c->mWalkSpeed, 0, 0xa, 0x100, 1) != 0) goto reset;
    if (c->mModelAnim.Finished() == 0) return;
    if (c->mSizeIndex != 0) goto detox;
    {
        Player* player = (Player*)c->ClosestPlayer();
        if (player->StartTalk(*c, 1) == 0) return;
        Message::PrepareTalk();
        if (func_ov074_02121a20(c, 1) != 0 || func_ov074_02121a20(c, 2) != 0) {
            func_ov074_02121a4c((char*)c, 0xc);
            return;
        }
        func_ov074_021203e4((char*)c, 7);
        c->mTalkPlayer = player;
        return;
    }
detox:
    func_ov074_021203e4((char*)c, 6);
    return;
reset:
    func_ov074_021216f4(c);
}
}

// @symbol func_ov074_0211fc34
extern "C" {
void func_ov074_0211fc34(void)
{
}
}

// @symbol func_ov074_0211fbd0
extern "C" {
int func_ov074_0211fbd0(Goomboss* c){
    if(ApproachAngle(&c->mWalkSpeed, 0, 0xa, 0x100, 1)==0)
        return ((int (*)(char*, int))func_ov074_021203e4)((char*)c, 3);
    return func_ov074_021216f4(c);
}
}

// @symbol func_ov074_0211fb84
extern "C" {
void func_ov074_0211fb84(Goomboss* c){
  func_ov074_02120808(c);
  c->mScalePhase = 0;
  c->mSubState = 0;
  unsigned short t=3-c->mSizeIndex;
  if(t>2)t=2;
  c->mTextureSequence.currFrame = t<<12;
  c->mScaleSteps = 3;
}
}

// @symbol func_ov074_0211fa74
extern "C" {
void func_ov074_0211fa74(Goomboss* c)
{
    /* 0x603 does not fit an ARM immediate, so the increment is an address
     * materialized from the pool. A plain mSubState++ drops that and comes
     * out 0x14 short. */
    Goomboss* a = c;
    int b;
    switch (a->mSubState) {
    case 0:
        if (func_ov074_02121a20(a, 6) != 0) {
            unsigned char* p;
            b = func_ov074_021207b8(a);
            if (b == 0) return;
            p = (unsigned char*)(((int)&a->mSubState));
            *p = *p + 1;
            func_ov074_02121a4c((char*)a, 0);
            func_02012694(0x15c, &a->mCamSpacePosX);
            return;
        }
        {
            unsigned char* p = (unsigned char*)(((int)&a->mSubState));
            *p = *p + 1;
        }
        func_ov074_02121a4c((char*)a, 0);
        func_02012694(0x15c, &a->mCamSpacePosX);
        return;
    case 1:
        {
            b = func_ov074_02120474(a);
            if (func_ov074_021204c0(a) == 0) return;
            if (b == 0) return;
            func_ov074_021203e4((char*)a, 2);
        }
        return;
    }
}
}

// @symbol func_ov074_0211fa08
extern "C" {
void func_ov074_0211fa08(Goomboss* c){
  c->mSizeIndex = 0;
  int v=func_01ffa344((int)data_ov074_02122e4c[1]);
  c->mScaleX = v;
  c->mScaleY = v;
  c->mScaleZ = v;
  c->mTextureSequence.currFrame = 0x2000;
  c->mMaterialChanger.currFrame = ((int)data_ov074_02122e04[1])<<0xc;
  c->mSubState = 0;
  ((void (*)(char*, int, int))func_ov074_02121a4c)((char*)c, 3, 0);
  c->mScaleSteps = 3;
}
}

// @symbol func_ov074_0211f5b8
extern "C" void func_ov074_0211f5b8(Goomboss* c)
{
    Vector3 vzero;
    Vector3 vmsg;
    Player* player;
    s32 state;
    void* data0209f318;

    vzero.x = 0;
    vzero.y = 0;
    vzero.z = 0;
    vmsg.x = c->mPosX;
    vmsg.y = c->mPosY;
    vmsg.z = c->mPosZ;
    vmsg.y = vmsg.y + 0x78000;
    data0209f318 = data_0209f318;
    player = c->mTalkPlayer;

    if (c->mModelAnim.Finished() != 0) {
        func_ov074_02121a4c((char*)c, 4);
    }

    switch (c->mSubState) {
    case 0:
        func_ov074_0211f154(c);
        {
            u16 msgId = (u16)((s32)player->param1 + 0xd7);
            state = player->GetTalkState();
            if (state != 0) {
                if (state == 1)
                    return;
                if (state == 2)
                    goto inc603;
                return;
            }
            if (player->ShowMessage(*c, (s16)msgId, &vmsg, 1, 2) == 0)
                return;
            func_0201277c(0x15a);
        }
        return;
    inc603:
        c->mSubState += 1;
        return;
    case 1:
        if (DecIfAbove0_Short(&c->mTimer) == 0) {
            dActor_c* actor = (dActor_c*)dActor_c::FindWithActorID(0xc8, 0);
            if (actor != 0) {
                u32 id = 0xc8;
                while (1) {
                    s32 flag = (actor->mFlags & 0x40000) ? 1 : 0;
                    if (flag == 0) {
                        Vector3 v2;
                        s32* src = &actor->mPosX;
                        v2.x = src[0];
                        v2.y = src[1];
                        v2.z = src[2];
                        c->PoofDustAt(v2);
                        actor->fBase_c::MarkForDestruction();
                        c->mTimer = 2;
                        return;
                    }
                    actor = (dActor_c*)dActor_c::FindWithActorID(id, actor);
                    if (actor == 0)
                        break;
                }
            }
        }
        if (c->mSpawnedCount == 1) {
            dActor_c* a2 = (dActor_c*)dActor_c::FindWithActorID(0xc8, 0);
            if (a2 != 0) {
                s32 flag2;
                if (a2->mFlags & 0x40000)
                    flag2 = 1;
                else
                    flag2 = 0;
                if (flag2 != 0) {
                    c->mSpawnedCount -= 1;
                }
            }
        }
        if (c->mSpawnedCount != 0)
            return;
        func_02012694(0x15c, &c->mCamSpacePosX);
        c->mSubState += 1;
        c->mSpawnedCount = 0;
        return;
    case 2:
        if (c->mWaveFinished == 0 && func_ov074_0211f38c(c) != 0) {
            c->mWaveFinished = 1;
        }
        {
            s32 r4 = func_ov074_02120474(c);
            if (func_ov074_021204c0(c) == 0)
                return;
            if (r4 == 0)
                return;
        }
        c->mSpawnedCount = 0;
        c->mSubState += 1;
        return;
    case 3:
        if (player->HasFinishedTalking() == 0)
            return;
        c->mShouldRender = 0;
        vzero.x = c->mPosX;
        vzero.y = c->mPosY;
        vzero.z = c->mPosZ;
        vzero.y = vzero.y + 0x190000;
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x91, vzero.x, vzero.y, vzero.z);
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x92, vzero.x, vzero.y, vzero.z);
        func_02012694(0x15f, &c->mCamSpacePosX);
        func_ov074_0211f344(c, 1);
        func_ov074_0211f244(c);
        *(s32*)AT(data0209f318, 0x154) &= ~8;
        Message::EndTalk();
        _ZN5Sound22StopLoadedMusic_Layer3Ev();
        func_02011cfc();
        _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x15666);
        c->mNoShadow = 1;
        c->mTimer = 0xf;
        c->mSubState += 1;
        return;
    case 4:
        vzero.x = c->mPosX;
        vzero.y = c->mPosY;
        vzero.z = c->mPosZ;
        vzero.y = vzero.y + 0x12c000;
        dActor_c::Spawn(0x11a, 2, vzero, 0, c->mAreaId, -1);
        func_02012694(0xbb, &c->mCamSpacePosX);
        c->fBase_c::MarkForDestruction();
        return;
    default:
        return;
    }
}

/* Release one Goomba on a timer, and report whether the whole wave has been
   released.  The wave is built in three tiers: the first seven Goombas ring
   the base, the next five sit higher on a narrower ring, the next three higher
   and narrower still, and the sixteenth onward spawns dead centre, highest of
   all.  The spawn count needed is a table lookup on mSizeIndex, with one
   extra on size 1 unless the closest player's param1 reads 3. */
// @symbol func_ov074_0211f38c
extern "C" {
int func_ov074_0211f38c(Goomboss* c){
    struct Vector3 spawnPos, basePos;
    struct Vector3_16 spawnAngle;
    int radius;
    int spawnTarget;
    Player* player;
    spawnPos.x=0; spawnPos.y=0; spawnPos.z=0;
    spawnAngle.x=0; spawnAngle.y=0; spawnAngle.z=0;
    spawnPos.x=c->mPosX;
    spawnPos.y=c->mPosY;
    spawnPos.z=c->mPosZ;
    spawnPos.y+=0xe4000;
    basePos.x=spawnPos.x;
    basePos.y=spawnPos.y;
    basePos.z=spawnPos.z;
    spawnAngle.y=(short)c->mAngleY;
    if(DecIfAbove0_Short(&c->mTimer)==0){
        u8 spawned=c->mSpawnedCount;
        if(spawned<=6){
            spawnAngle.y=(short)(spawned*0x2492+spawnAngle.y);
            radius=0x18c000;
        } else if(spawned<=0xb){
            spawnPos.y+=0xc4000;
            spawnAngle.y=(short)((c->mSpawnedCount-7)*0x3333+spawnAngle.y);
            radius=0x108000;
        } else if(spawned>0xe){
            spawnPos.y+=0x24c000;
            radius=0;
        } else {
            spawnPos.y+=0x188000;
            spawnAngle.y=(short)((c->mSpawnedCount-0xc)*0x5555+spawnAngle.y);
            radius=0x84000;
        }
        {
            int sinIdx=((u16)spawnAngle.y>>4)<<1;
            spawnPos.x += (int)(((s64)radius * data_02082214[sinIdx] + 0x800) >> 0xc);
            spawnPos.z += (int)(((s64)radius * data_02082214[sinIdx+1] + 0x800) >> 0xc);
        }
        spawnAngle.x=Vec3_VertAngle(&basePos, &spawnPos);
        dActor_c::Spawn(0xc7, 0x1111, spawnPos, &spawnAngle, c->mAreaId, -1);
        c->mSpawnedCount += 1;
        c->mTimer = 2;
    }
    spawnTarget=data_ov074_02122d80[c->mSizeIndex];
    player=(Player*)c->ClosestPlayer();
    if(c->mSizeIndex==1){ if((int)player->param1!=3) spawnTarget+=1; }
    return c->mSpawnedCount>=spawnTarget ? 1 : 0;
}
}

// @symbol func_ov074_0211f344
extern "C" {
void func_ov074_0211f344(Goomboss* self, unsigned char value) {
  Goomboss* other = (Goomboss*)dActor_c::FindWithActorID(0xc7, 0);
  while (other) {
    if (other != self) other->mShouldRender = value;
    other = (Goomboss*)dActor_c::FindWithActorID(0xc7, other);
  }
}
}

// @symbol func_ov074_0211f244
extern "C" void func_ov074_0211f244(Goomboss *self)
{
    Goomboss *a = (Goomboss *)dActor_c::FindWithActorID(0xc7, 0);
    if (a == 0) return;
    do {
        if (a != self) {
            Goomboss *t = a;
            t->mVertAccel = -0xa000;
            int rv = (unsigned int)RandomIntInternal(&data_0209e650) >> 8;
            t->mTimer = 0x30;
            t->mHorzSpeed = ((rv & 0xf) << 0xc) + 0x16000;
            t->unk_0a4 = 0;
            t->mVertSpeed = ((rv & 0x1f) << 0xc) + 0x60000;
            t->unk_0ac = 0;
            Vector3_16 pos = *(Vector3_16*)&t->mPrevAngleX;
            rv &= 0x1000;
            pos.y = (short)(pos.y + rv);
            short x = pos.x + 0;
            t->mPrevAngleX = x;
            short y = pos.y + 0;
            t->mPrevAngleY = y;
            short z = pos.z + 0;
            t->mPrevAngleZ = z;
            t->mAngleX = x;
            t->mAngleY = y;
            t->mAngleZ = z;
            t->mLaunched = 1;
        }
        a = (Goomboss *)dActor_c::FindWithActorID(0xc7, a);
    } while (a != 0);
}

// @symbol func_ov074_0211f154
extern "C" {
void func_ov074_0211f154(Goomboss* c) {
    struct Vector3 look, pos, in, out;
    void* cam;
    cam = data_0209f318;
    _ZN6Camera9SetFlag_3Ev(cam);
    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;
    look.x = c->mPosX;
    look.y = c->mPosY;
    look.z = c->mPosZ;
    pos.x = c->mPosX;
    pos.y = c->mPosY;
    pos.z = c->mPosZ;
    look.y += 0x1a0000;
    in.z = 0x59c000;
    Matrix4x3_FromRotationY(&data_020a0e68, c->mAngleY);
    MulVec3Mat4x3(&in, &data_020a0e68, &out);
    pos.x = pos.x + out.x;
    pos.y = pos.y + 0x300000;
    pos.z = pos.z + out.z;
    ((Camera *)cam)->SetLookAt(look);
    ((Camera *)cam)->SetPos(pos);
}
}


//cpp
/* Eyerok, the two-handed pyramid boss (.text 0x021184c0..0x02119ce8).
 * ROM RTTI daIwante_c (_ZTS10daIwante_c ov066:0x0211ad30); this tree keeps
 * the coined name. Partial: shards 8,16,20,22,24,59 reference 0x02112c08..
 * (co-resident overlay, unresolved), so they can't link.
 *
 * Source is ROM-ascending under defer_codegen off. Do not reorder.
 *
 * Leftover: the func_ov066 helpers keep linker names. State dispatch stays
 * an incomplete-class pointer-to-member. EVec3 and M48 stay plain words so
 * ~Vector3 is not emitted. Vec4 is an unused stack object with a destructor.
 */

/* Turns off deferred codegen, which does two things at once here: it makes
 * the bracketed opt_common_subs / opt_strength_reduction pair around
 * _ZN6Eyerok8BehaviorEv bind to that member alone instead of leaking
 * file-wide, and it flips .text emission from reverse-source to source
 * order -- which is why this file is written ROM-ascending. */
#pragma defer_codegen off
/* Includes. decl_common.h is DELIBERATELY NOT included: it types
 * func_ov066_02119454 as returning void, and this TU *defines* that member --
 * conforming the definition to decl_common.h's spelling costs the match
 * (measured with tools/match.py: int -> MATCH, void -> no match). Every
 * symbol decl_common.h would have supplied is declared below instead, with
 * the spelling the shards actually matched under. */
#include "Eyerok.h"
#include "types.h"
#include "dBgW.h"
#include "common.h"
#include "decl_Message.h"
#include "SharedFilePtr.h"
#include "TextureSequence.h"
#include "Player.h"

/* EVec3 is three plain words. Vector3's destructor must not land in this TU. */
struct EVec3 { int x, y, z; };
struct C;
typedef int (C::*PMF)();
struct State { char pad[8]; PMF fn; };

extern "C" {
/* ---- ov066 .bss: 8-byte SharedFilePtr slots (0x0211ae14..0x0211aebc) ---- */
extern int data_ov066_0211ae14[];
extern int data_ov066_0211ae1c[];
extern int data_ov066_0211ae24[];
extern int data_ov066_0211ae2c[];
extern int data_ov066_0211ae34[];
extern int data_ov066_0211ae3c[];
extern int data_ov066_0211ae44[];
extern int data_ov066_0211ae4c[];
extern int data_ov066_0211ae54[];
extern int data_ov066_0211ae5c[];
extern int data_ov066_0211ae64[];
extern int data_ov066_0211ae6c[];
extern int data_ov066_0211ae74[];
extern int data_ov066_0211ae7c[];
extern int data_ov066_0211ae84[];
extern int data_ov066_0211ae8c[];
extern int data_ov066_0211ae94[];
extern int data_ov066_0211ae9c[];
extern int data_ov066_0211aea4[];
extern int data_ov066_0211aeac[];
extern int data_ov066_0211aeb4[];
extern int data_ov066_0211aebc[];

/* ---- ov066 .bss / .data byte flags and counters ---- */
extern unsigned char data_ov066_0211ae04;
extern unsigned char data_ov066_0211ae08;
extern unsigned char data_ov066_0211ae0c;
extern unsigned char data_ov066_0211ae10;
extern unsigned char data_ov066_0211abe0;
extern int data_ov066_0211abe4;
extern int data_ov066_0211ad18[];

/* ---- ov066 .bss state descriptors, 0x10 bytes each ---- */
extern char data_ov066_0211afcc;
extern char data_ov066_0211afdc;
extern char data_ov066_0211affc;
extern char data_ov066_0211b00c;
extern char data_ov066_0211b02c;
extern char data_ov066_0211b03c;
extern char data_ov066_0211b06c;
extern char data_ov066_0211b07c;
extern char data_ov066_0211b0ac;
extern char data_ov066_0211b0cc;
extern char data_ov066_0211b0dc;

/* ---- arm9 data ---- */
extern int data_0209e650;
extern void *data_0209f318;
extern int data_020a0e68[];

/* ---- arm9 helpers (unmangled ROM names) ---- */
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
extern void Matrix4x3_FromRotationY(void *m, short ang);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void MulVec3Mat4x3(void *a, void *m, void *b);
extern int RandomIntInternal(int *seed);
extern int Vec3_ApproachHorz(void *out, void *a, int maxStep);
extern void Vec3_Asr(void *d, void *s, int sh);
extern int Vec3_HorzDist(const void *a, const void *b);
extern void func_020092c4(void *cam, void *out, void *target);
extern void func_02011cfc(void);
extern void func_02011d2c(void);
extern void func_02012694(int a, void *p);

/* ---- arm9 / ov002 methods, mangled ROM spelling ---- */
extern void _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(void *self, void *m, short s);
extern void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void *self, const void *v);
extern void _ZN14BlendModelAnim7AdvanceEv(void *self);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *self, void *bca, int a, int b, int fix, unsigned short t);
extern void _ZN15TextureSequence6UpdateER15ModelComponents(void *self, void *mc);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, void *btp, int a, int fix, unsigned int b);
extern void _ZN4dBgW6EnableEP8dActor_c(void *self, void *actor);
extern void _ZN4dBgW7DisableEv(void *self);
extern int _ZN4dBgW9IsEnabledEv(void *self);
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int a, int b);
extern void _ZN5Sound22LoadAndSetMusic_Layer3Ej(unsigned int a);
extern void _ZN5Sound22StopLoadedMusic_Layer3Ev(void);
extern void _ZN5dCc_c5ClearEv(void *self);
extern void _ZN5dCc_c6UpdateEv(void *self);
extern void _ZN6Camera9SetFlag_3Ev(void *cam);
extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void *self, void *actor, unsigned int msg, const void *v, unsigned int d, unsigned int e);
extern int _ZN6Player12GetTalkStateEv(void *self);
extern void _ZN6Player17SetNoControlStateEhih(void *self, unsigned char a, int b, unsigned char c);
extern void _ZN7Message11PrepareTalkEv(void);
extern void _ZN7Message7EndTalkEv(void);
extern void _ZN7fBase_c18MarkForDestructionEv(void *self);
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 a, u32 b, int x, int y, int z, const void *v, void *cb);
extern void *_ZN8dActor_c13ClosestPlayerEv(void *self);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *self, void *sm, void *m, int rad, int h, unsigned int u);
extern void _ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h(void *self, signed char *a, unsigned int b, const void *v, unsigned int d);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *clsn);
extern void _ZN9Animation7AdvanceEv(void *self);
extern int _ZN9Animation8FinishedEv(void *self);

/* ---- siblings of this TU that stayed in their own src/ files ---- */
extern void func_ov066_021162e8(void *c);
extern void func_ov066_0211632c(void *c);
extern int func_ov066_021168b0(void *c);
extern int func_ov066_021168ec(void *c);
extern int func_ov066_02116a68(void *c);
extern void func_ov066_02116ac4(void *c, int v);

/* ---- this TU's own members, forward-declared: the file is written
 *      ROM-ascending, so a member that calls one defined further down
 *      needs a declaration first ---- */
extern void func_ov066_021194a4(char *c);
extern void func_ov066_021194fc(char *c);
extern int func_ov066_02119454(void *c, void *p);
}

typedef struct { int w[12]; } M48;

// @symbol func_ov066_021184c0
extern "C" {
int func_ov066_021184c0(char *p)
{
    Eyerok *self = (Eyerok *)p;
    self->mStateWork0 = 0;
    self->mStateWork1 = 0;
    self->mTimer1 = 0;
    self->mSubState = 0;
    return 1;
}
}

// @symbol func_ov066_021184e0
extern "C" {
int func_ov066_021184e0(char *c)
{
    Eyerok *self = (Eyerok *)c;
    if (func_ov066_021168ec(c) != 0 && func_ov066_021168ec(c) != 4) {
        self->mFlags = 0;
        func_ov066_021162e8((int *)c);
        return 1;
    }
    func_ov066_0211632c(c);
    switch (self->mSubState) {
    case 0:
        if (data_ov066_0211ae0c == self->mPartIdx) {
            int *p = &self->mSubState;
            self->mVertAccel = -0x14000;
            self->mVertSpeed = 0x64000;
            *p = *p + 1;
        }
        break;
    case 1:
        if (self->mVertAccel != 0) {
            if (self->mRestPosY >= self->mPosY) {
                self->mPosY = self->mRestPosY;
                self->mVertSpeed = 0;
                self->mVertAccel = 0;
                func_ov066_02116ac4(c, 0x7d0000);
                if ((data_ov066_0211ae0c & self->mPartIdx) != 0)
                    data_ov066_0211ae0c ^= self->mPartIdx;
                self->mSubState = 0;
            }
        }
        break;
    }
    return 1;
}
}

// @symbol func_ov066_021185e4
extern "C" {
int func_ov066_021185e4(char *p)
{
    Eyerok *self = (Eyerok *)p;
    self->mStateWork0 = 0;
    self->mStateWork1 = 0;
    self->mTimer1 = 0;
    self->mSubState = 0;
    return 1;
}
}

// @symbol func_ov066_02118604
extern "C" {
int func_ov066_02118604(void *c) {
    int r = func_ov066_021168ec(c);
    if (r != 0) {
        data_ov066_0211ae0c ^= ((Eyerok *)c)->mPartIdx;
        if (data_ov066_0211abe0 != 3) {
            data_ov066_0211ae0c |= 3;
        }
    }
    return 1;
}
}

// @symbol func_ov066_02118658
extern "C" {
int func_ov066_02118658(char *p)
{
    Eyerok *self = (Eyerok *)p;
    self->mStateWork0 = 0;
    self->mStateWork1 = 0;
    self->mTimer1 = 0;
    self->mSubState = 0;
    return 1;
}
}

// @symbol func_ov066_02118678
extern "C" {
int func_ov066_02118678(char* c)
{
    Eyerok *self = (Eyerok *)c;
    if (self->mStateWork0 == 0) {
        if (data_ov066_0211ae04 == 2) {
            self->mBlendModelAnim.speed = 0x1000;
            if (self->mPartIdx == 1) {
                self->mVertSpeed = 0x2d000;
                self->mVertAccel = -0x2000;
            } else {
                self->mVertSpeed = 0xa000;
                self->mVertAccel = -0x800;
            }
            self->mStateWork0 = 1;
            func_02012694(0x144, &self->mCamSpacePosX);
        }
        return 1;
    }

    Vec3_ApproachHorz(&self->mPosX, &self->mRestPosX, 0x14000);
    if (self->mVertAccel != 0) {
        int v = self->mRestPosY;
        if (v >= self->mPosY) {
            self->mPosY = v;
            self->mVertSpeed = 0;
            self->mVertAccel = 0;
            func_ov066_02116ac4(c, 0x7d0000);
        }
    }

    if (self->mVertAccel == 0
        && Vec3_HorzDist(&self->mPosX, &self->mRestPosX) <= 0x14000
        && self->mBlendModelAnim.Finished()) {
        _ZN4dBgW6EnableEP8dActor_c(&self->mMeshCollider2, c);
        data_ov066_0211ae0c |= self->mPartIdx;
        func_ov066_02119454(c, &data_ov066_0211b06c);
    }
    return 1;
}
}

// @symbol func_ov066_021187c8
extern "C" {
int func_ov066_021187c8(char* c){
    Eyerok *self = (Eyerok *)c;
  if(self->mPartIdx == 2){
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&self->mBlendModelAnim, (void*)data_ov066_0211ae74[1], 4, 0x40000000, 0x1000, 0);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&self->mTextureSequence, (void*)data_ov066_0211ae3c[1], 0x40000000, 0x1000, 0);
  } else {
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&self->mBlendModelAnim, (void*)data_ov066_0211ae7c[1], 4, 0x40000000, 0x1000, 0);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&self->mTextureSequence, (void*)data_ov066_0211aebc[1], 0x40000000, 0x1000, 0);
  }
  self->mBlendModelAnim.speed = 0;
  self->mStateWork0 = 0;
  self->mStateWork1 = 0;
  return 1;
}
}

// @symbol func_ov066_021188b0
extern "C" {
int func_ov066_021188b0(char* c){
    Eyerok *self = (Eyerok *)c;
  if(data_ov066_0211abe0==0){
    self->mTimer2=0x64;
    func_ov066_02119454(c, &data_ov066_0211b0ac);
    return 1;
  }
  if(data_ov066_0211ae08>=2){
    data_ov066_0211ae04=3;
    func_ov066_02119454(c, &data_ov066_0211b0cc);
  }
  return 1;
}
}

// @symbol func_ov066_02118934
extern "C" {
int func_ov066_02118934(char *p)
{
    Eyerok *self = (Eyerok *)p;
    self->mStateWork0 = 0;
    self->mStateWork1 = 0;
    self->mTimer1 = 0;
    self->mSubState = 0;
    return 1;
}
}

// @symbol func_ov066_02118954
extern "C" {
s32 func_ov066_02118954(char* c) {
    s32 r = func_ov066_021168b0(c);
    if (r == 0) {
        return 1;
    }
    ((Eyerok *)c)->mPickCount = 0;
    *(char*)&data_ov066_0211ae10 = 0;
    func_ov066_02119454(c, &data_ov066_0211b03c);
    return 1;
}
}

// @symbol func_ov066_021189a0
extern "C" {
int func_ov066_021189a0(char *p)
{
    Eyerok *self = (Eyerok *)p;
    self->mStateWork0 = 0;
    self->mStateWork1 = 0;
    self->mTimer1 = 0;
    self->mSubState = 0;
    return 1;
}
}

// @symbol func_ov066_021189c0
extern "C" {
int RandomIntInternal(int* seed);
int func_ov066_021189c0(char* c){
  if(func_ov066_021168b0(c) == 0) return 1;
  if((((unsigned int)RandomIntInternal(&data_0209e650) >> 0x1f) & 1) == 0)
    data_ov066_0211ae0c = 2;
  else
    data_ov066_0211ae0c = 1;
  ((Eyerok *)c)->mPickCount = 0;
  func_ov066_02119454(c, &data_ov066_0211b03c);
  return 1;
}
}

// @symbol func_ov066_02118a30
extern "C" {
int func_ov066_02118a30(char *p)
{
    Eyerok *self = (Eyerok *)p;
    self->mStateWork0 = 0;
    self->mStateWork1 = 0;
    self->mTimer1 = 0;
    self->mSubState = 0;
    return 1;
}
}

// @symbol func_ov066_02118a50
extern "C" {
s32 func_ov066_02118a50(char* c) {
    s32 r = func_ov066_021168b0(c);
    if (r == 0) return 1;
    if (data_ov066_0211abe0 == 3) {
        if (!(data_ov066_0211ae10 & 1)) data_ov066_0211ae0c = 2;
        else data_ov066_0211ae0c = 1;
    } else {
        data_ov066_0211ae0c = data_ov066_0211abe0;
    }
    {
        unsigned char* p = (unsigned char*)((int)&((Eyerok *)c)->mPickCount);
        *p += 1;
    }
    data_ov066_0211ae10 += 1;
    data_ov066_0211ae10 &= 1;
    func_ov066_02119454(c, &data_ov066_0211b03c);
    return 1;
}
}

// @symbol func_ov066_02118b08
extern "C" {
int func_ov066_02118b08(char *p)
{
    Eyerok *self = (Eyerok *)p;
    self->mStateWork0 = 0;
    self->mStateWork1 = 0;
    self->mTimer1 = 0;
    self->mSubState = 0;
    return 1;
}
}

// @symbol func_ov066_02118b28
extern "C" {
s32 func_ov066_02118b28(char* c) {
    s32 r = func_ov066_021168b0(c);
    if (r == 0) return 1;
    if (data_ov066_0211abe0 == 3) {
        if (!(data_ov066_0211ae10 & 1)) data_ov066_0211ae0c = 2;
        else data_ov066_0211ae0c = 1;
    } else {
        data_ov066_0211ae0c = data_ov066_0211abe0;
    }
    {
        unsigned char* p = (unsigned char*)((int)&((Eyerok *)c)->mPickCount);
        *p += 1;
    }
    data_ov066_0211ae10 += 1;
    data_ov066_0211ae10 &= 1;
    func_ov066_02119454(c, &data_ov066_0211b03c);
    return 1;
}
}

// @symbol func_ov066_02118be0
extern "C" {
int func_ov066_02118be0(char *p)
{
    Eyerok *self = (Eyerok *)p;
    self->mStateWork0 = 0;
    self->mStateWork1 = 0;
    self->mTimer1 = 0;
    self->mSubState = 0;
    return 1;
}
}

// @symbol func_ov066_02118c00
extern "C" {
s32 func_ov066_02118c00(char* c) {
    s32 r = func_ov066_021168b0(c);
    if (r == 0) return 1;
    if (data_ov066_0211abe0 == 3) {
        if (!(data_ov066_0211ae10 & 1)) data_ov066_0211ae0c = 2;
        else data_ov066_0211ae0c = 1;
    } else {
        data_ov066_0211ae0c = data_ov066_0211abe0;
    }
    {
        unsigned char* p = (unsigned char*)((int)&((Eyerok *)c)->mPickCount);
        *p += 1;
    }
    data_ov066_0211ae10 += 1;
    data_ov066_0211ae10 &= 1;
    func_ov066_02119454(c, &data_ov066_0211b03c);
    return 1;
}
}

// @symbol func_ov066_02118cb8
extern "C" {
int func_ov066_02118cb8(char *p)
{
    Eyerok *self = (Eyerok *)p;
    self->mStateWork0 = 0;
    self->mStateWork1 = 0;
    self->mTimer1 = 30;
    self->mSubState = 0;
    return 1;
}
}

// @symbol func_ov066_02118cdc
extern "C" {

int func_ov066_02118cdc(char* c) {
    Eyerok *self = (Eyerok *)c;
    if (func_ov066_021168b0(c) == 0)
        return 1;
    if (self->mTimer1 == 0) {
        if (func_ov066_02116a68(c) != (int)0xff3ae000) {
            if (data_ov066_0211ae0c == 0) {
                data_ov066_0211ae04 = 3;
                self->mTimer2 = 0x1e;
                func_ov066_02119454(c, &data_ov066_0211b0cc);
            }
            return 1;
        }
    }
    if (data_ov066_0211ae0c == 0) {
        if (data_ov066_0211abe0 == 3) {
            if (self->mStateWork0 == 0)
                data_ov066_0211ae0c = 1;
            else
                data_ov066_0211ae0c = 2;
        } else {
            data_ov066_0211ae0c = data_ov066_0211abe0;
        }
        volatile int* tmp = (volatile int*)((int)&self->mStateWork0);
        *tmp = *tmp + 1;
        *tmp = *tmp & 1;
    }
    return 1;
}
}

// @symbol func_ov066_02118de0
extern "C" {
int func_ov066_02118de0(char *p)
{
    Eyerok *self = (Eyerok *)p;
    self->mStateWork0 = 0;
    self->mStateWork1 = 0;
    self->mTimer1 = 30;
    self->mSubState = 0;
    return 1;
}
}

// @symbol func_ov066_02118e04
extern "C" {
int func_ov066_02118e04(void* self)
{
    u8* c = (u8*)self;
    void* p = _ZN8dActor_c13ClosestPlayerEv(self);
    int coinFlip;
    int v;

    if (data_ov066_0211abe0 == 0) {
        ((Eyerok *)c)->mTimer2 = 0x64;
        func_ov066_02119454(c, &data_ov066_0211b0ac);
        return 1;
    }

    if (p == 0 || ((Eyerok *)c)->mTimer2 != 0)
        return 1;

    coinFlip = ((unsigned int)RandomIntInternal(&data_0209e650) >> 31) & 1;

    data_ov066_0211ae08 = 0;
    data_ov066_0211ae0c = 0;
    v = func_ov066_02116a68(c);
    if (v == (int)0xff3ae000) {
        data_ov066_0211ae04 = 4;
        func_ov066_02119454(c, &data_ov066_0211b0dc);
        return 1;
    }

    if ((int)((Eyerok *)c)->mPickCount > data_ov066_0211abe4 + 3) {
        if (data_ov066_0211abe0 == 3) {
            data_ov066_0211abe4++;
            data_ov066_0211abe4 &= 1;
            data_ov066_0211ae04 = 8;
            func_ov066_02119454(c, &data_ov066_0211b00c);
        } else {
            data_ov066_0211abe4 = -3;
            data_ov066_0211ae04 = 9;
            func_ov066_02119454(c, &data_ov066_0211b02c);
        }
        return 1;
    }

    v = func_ov066_02116a68(c);
    if (v == -0xb50000) {
        data_ov066_0211ae04 = 5;
        func_ov066_02119454(c, &data_ov066_0211afcc);
        return 1;
    }

    if (coinFlip == 0) {
        data_ov066_0211ae04 = 7;
        func_ov066_02119454(c, &data_ov066_0211affc);
    } else {
        data_ov066_0211ae04 = 6;
        func_ov066_02119454(c, &data_ov066_0211afdc);
    }
    return 1;
}
}

// @symbol func_ov066_0211901c
extern "C" {
int func_ov066_0211901c(char *p)
{
    Eyerok *self = (Eyerok *)p;
    self->mStateWork0 = 0;
    self->mStateWork1 = 0;
    self->mTimer1 = 0;
    self->mSubState = 0;
    return 1;
}
}

// @symbol func_ov066_0211903c
extern "C" {
int func_ov066_0211903c(char* self) {
    Eyerok *obj = (Eyerok *)self;
    struct Vector3 v1, v2, in, out, star;
    void* cam;
    int msgid;

    if (obj->mTimer2) return 1;

    cam = data_0209f318;
    if (((Eyerok *)self)->mSubState == 0) {
        _ZN6Camera9SetFlag_3Ev(cam);
        ((Eyerok *)self)->mTalkPlayer = (Player *)_ZN8dActor_c13ClosestPlayerEv(self);
        if (((Eyerok *)self)->mTalkPlayer != 0)
            _ZN6Player17SetNoControlStateEhih(((Eyerok *)self)->mTalkPlayer, 5, -1, 0);
        ((Eyerok *)self)->mSubState = 1;
    } else {
        v1.x = ((Eyerok *)self)->mPosX;
        v1.y = ((Eyerok *)self)->mPosY;
        v1.z = ((Eyerok *)self)->mPosZ;
        v2.x = ((Eyerok *)self)->mPosX;
        v2.y = ((Eyerok *)self)->mPosY;
        v2.z = ((Eyerok *)self)->mPosZ;
        v1.y += 0x100000;
        v2.x += 0x10000;
        v2.y += 0x100000;
        v2.z += 0x564000;
        func_020092c4(cam, (char*)cam + 0x80, &v1);
        func_020092c4(cam, (char*)cam + 0x8c, &v2);
    }

    if (data_ov066_0211abe0 == 3) {
        if (data_ov066_0211ae0c != 3) return 1;
    }

    if (((Eyerok *)self)->mStateWork1 == 0) {
        if (((Eyerok *)self)->mTalkPlayer != 0) {
            in.x = 0; in.y = 0; in.z = 0;
            out.x = 0; out.y = 0; out.z = 0;
            in.y = 0x32000;
            in.z = -0x32000;

            Matrix4x3_FromRotationY(data_020a0e68, 0);
            MulVec3Mat4x3(&in, data_020a0e68, &out);

            out.x += ((Eyerok *)self)->mPosX;
            out.y += ((Eyerok *)self)->mPosY;
            out.z += ((Eyerok *)self)->mPosZ;

            msgid = 0xb8;
            if (data_ov066_0211abe0 == 0) {
                msgid = 0xb9;
                _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x14, 0x15666);
            }

            ((Eyerok *)self)->mTalkPlayer->mStateFlags |= 0x400;
            _ZN7Message11PrepareTalkEv();
            if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(((Eyerok *)self)->mTalkPlayer, self, msgid, &out, 0, 0) == 1) {
                ((Eyerok *)self)->mStateWork1 = 1;
                func_02012694(0x145, &((Eyerok *)self)->mCamSpacePosX);
            }
        }
    } else {
        if (((Eyerok *)self)->mTalkPlayer != 0) {
            if (_ZN6Player12GetTalkStateEv(((Eyerok *)self)->mTalkPlayer) < 0) {
                *(int*)(((int)cam + 0x154)) &= ~8;
                _ZN7Message7EndTalkEv();
                if (data_ov066_0211abe0 == 3) {
                    _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2d);
                    func_02011d2c();
                    func_ov066_02119454(self, &data_ov066_0211b0cc);
                } else {
                    _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x15666);
                    _ZN5Sound22StopLoadedMusic_Layer3Ev();
                    func_02011cfc();
                    star.x = 0;
                    star.y = (int)0xffa24000;
                    star.z = (int)0xff1b4000;
                    _ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h(self, (signed char*)(&((Eyerok *)self)->mStarTracked), obj->mStarId, &star, 4);
                    _ZN7fBase_c18MarkForDestructionEv(self);
                }
            }
        }
    }
    return 1;
}
}

// @symbol func_ov066_02119348
extern "C" {
int func_ov066_02119348(void *c)
{
    if (_ZN4dBgW9IsEnabledEv((char *)&((Eyerok *)c)->mMeshCollider2) != 0) {
        _ZN4dBgW7DisableEv((char *)&((Eyerok *)c)->mMeshCollider2);
    }
    ((Eyerok *)c)->mStateWork0 = 0;
    ((Eyerok *)c)->mStateWork1 = 0;
    ((Eyerok *)c)->mTimer1 = 0;
    ((Eyerok *)c)->mSubState = 0;
    return 1;
}
}

// @symbol func_ov066_02119398
struct Vec4 { int a, b, c, d; ~Vec4(){} };
extern "C" {

int func_ov066_02119398(char* c)
{
    Vec4 sp;
    /* Member loads of the player's position come out a different size.
       The base pointer is what matches. */
    char* p = (char *)_ZN8dActor_c13ClosestPlayerEv(c);
    if (p != 0) {
        char* playerPos = p + 0x5c;
        int v1 = *(int*)(playerPos + 4);
        int v2 = *(int*)(playerPos + 8);
        if (v1 < -0x300000) {
            int f = (int)((((Eyerok *)c)->mFlags & 8) != 0);
            if (f == 0) {
                if (v2 < -0xd70000) {
                    data_ov066_0211ae08 += 1;
                }
            }
        }
    }
    if (data_ov066_0211ae08 > 2) {
        data_ov066_0211ae08 = 0;
        data_ov066_0211ae04 = 2;
        func_ov066_02119454(c, &data_ov066_0211b0ac);
    }
    return 1;
}
}

// @symbol func_ov066_0211944c
extern "C" {
int func_ov066_0211944c(void)
{
    return 1;
}
}

// @symbol func_ov066_02119454
struct C { char pad[0x48c]; PMF *pp; };
extern "C" int func_ov066_02119454(void *cv, void *pv) { C *c = (C *)cv; PMF *p = (PMF *)pv; c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }

// @symbol func_ov066_021194a4
extern "C" void func_ov066_021194a4(char *c) {
  Matrix4x3_FromRotationY(&((Eyerok *)c)->mClsnMat2, ((Eyerok *)c)->mAngleY);
  ((Eyerok *)c)->mClsnMat2.t.x = ((Eyerok *)c)->mPosX;
  ((Eyerok *)c)->mClsnMat2.t.y = ((Eyerok *)c)->mPosY;
  ((Eyerok *)c)->mClsnMat2.t.z = ((Eyerok *)c)->mPosZ;
  _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(&((Eyerok *)c)->mMeshCollider2, &((Eyerok *)c)->mClsnMat2, ((Eyerok *)c)->mAngleY);
}

// @symbol func_ov066_021194fc
extern "C" {


void func_ov066_021194fc(char* c)
{
    Eyerok *self = (Eyerok *)c;
    int v[3];
    Vec3_Asr(v, &self->mPosX, 3);
    Matrix4x3_FromTranslation(data_020a0e68, v[0], v[1], v[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(data_020a0e68, self->mAngleX, self->mAngleY, self->mAngleZ);
    if (self->mPartIdx == 0)
        *(M48 *)&self->mModel2.mat4x3 = *(M48*)data_020a0e68;
    else
        *(M48 *)&self->mBlendModelAnim.mat4x3 = *(M48*)data_020a0e68;
    if (self->mPartIdx == 0)
        return;
    if (self->mRestPosY >= self->mPosY)
        return;
    {
        int d;
        if (self->mPartIdx == 2)
            d = 0x64000;
        else
            d = -0x64000;
        Matrix4x3_FromTranslation(data_020a0e68,
            (self->mPosX + d) >> 3,
            (self->mPosY - 0x8000) >> 3,
            (self->mPosZ + 0xa0000) >> 3);
    }
    *(M48 *)self->mShadowMtx = *(M48*)data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, &self->mShadowModel, self->mShadowMtx, 0x140000, 0x258000, 0xf);
}
}

// @symbol _ZN6Eyerok16CleanupResourcesEv
int Eyerok::CleanupResources()
{
  if(((dBgW *)&mMeshCollider2)->IsEnabled())
    ((dBgW *)&mMeshCollider2)->Disable();
  if(mPartIdx==0){
    ((SharedFilePtr *)(data_ov066_0211ae6c))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae4c))->Release();
    ((SharedFilePtr *)(data_ov066_0211aeb4))->Release();
    ((SharedFilePtr *)(data_ov066_0211aebc))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae9c))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae3c))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae2c))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae5c))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae84))->Release();
    ((SharedFilePtr *)(data_ov066_0211aea4))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae8c))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae54))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae94))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae64))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae44))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae74))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae7c))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae24))->Release();
    ((SharedFilePtr *)(data_ov066_0211aeac))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae14))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae1c))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae34))->Release();
  }
  return 1;
}

// @symbol _ZN6Eyerok16OnPendingDestroyEv
/* Eyerok::OnPendingDestroy -- vtable slot 12. The ROM body is empty: the
 * override exists only to occupy the slot. */
void Eyerok::OnPendingDestroy()
{
}

// @symbol _ZN6Eyerok6RenderEv
int Eyerok::Render()
{
  if (mPartIdx == 0) {
    if (data_ov066_0211ae04 == 1) {
      mModel2.Render(0);
    }
    return 1;
  }
  if (data_ov066_0211ae04 == 1) return 1;
  _ZN15TextureSequence6UpdateER15ModelComponents(&mTextureSequence, &mBlendModelAnim.data);
  mBlendModelAnim.Render(0);
  return 1;
}


/* Bracketed, and it binds only because of the file-top
 * `#pragma defer_codegen off`: with codegen deferred (mwccarm 2004/b56s
 * default) a bracketed opt_* pragma does not bind and these two go
 * file-global, which costs func_ov066_021184e0 (4 words) and
 * func_ov066_021194fc (a size change).  Deleting them outright instead
 * costs _ZN6Eyerok8BehaviorEv: 33/34, a 999-word content divergence plus one
 * wrong relocation destination, with the size UNCHANGED at 0x4b0.  (An earlier
 * revision of this comment said 0x4b0 -> 0x4ac; that size change belongs to a
 * ROM-descending arrangement, not this one.  Re-measured by negative control on
 * the shipped source.) */
#pragma opt_common_subs off
#pragma opt_strength_reduction off
// @symbol _ZN6Eyerok8BehaviorEv
/* Eyerok::Behavior -- vtable slot 6. Real C++ method over the shared header.
 * EVec3 is a local plain-int triple (stack temps); callees whose ROM symbols
 * carry by-value/ref class parameters keep their literal mangled extern "C"
 * spellings. */
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


#pragma opt_strength_reduction on
#pragma opt_common_subs on

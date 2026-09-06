//cpp
/* ov066/Eyerok -- ROM ordinals 25..58 of the 61-function linker run at
 * 0x02115ee0..0x0211a2e4.  PARTIAL by necessity: six shards in the run
 * (ordinals 8, 16, 20, 22, 24, 59) reference the cross-overlay addresses
 * 0x02112c08/c88/ca8/cc8/d48, which no modules symbols.txt names, so they
 * carry no `complete` marker and are excluded from the link.  A licensed
 * .text claim cannot contain a hole, so this file takes the largest
 * contiguous linkable side: ordinals 25..58, 0x021184c0..0x02119ce8.
 *
 * The RTTI name of this class in the ROM is daIwante_c (_ZTS10daIwante_c at
 * ov066:0x0211ad30).  `Eyerok` is a coined name that the tree already spells
 * into every one of this runs function symbols, so it is kept here; renaming
 * would be a symbols.txt change, which a text-only promotion does not make.
 *
 * FUNCTION ORDER IS ROM-ASCENDING.  mwccarm 2004/b56 emits .text in the
 * REVERSE of source order only while codegen is deferred; this file turns
 * deferral off (see the pragma below), which both makes the bracketed
 * opt_* pragmas around _ZN6Eyerok8BehaviorEv bind and flips emission to
 * source order -- so the file is written low address first.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [25] 0x021184c0  func_ov066_021184c0
 *   [26] 0x021184e0  func_ov066_021184e0
 *   [27] 0x021185e4  func_ov066_021185e4
 *   [28] 0x02118604  func_ov066_02118604
 *   [29] 0x02118658  func_ov066_02118658
 *   [30] 0x02118678  func_ov066_02118678
 *   [31] 0x021187c8  func_ov066_021187c8
 *   [32] 0x021188b0  func_ov066_021188b0
 *   [33] 0x02118934  func_ov066_02118934
 *   [34] 0x02118954  func_ov066_02118954
 *   [35] 0x021189a0  func_ov066_021189a0
 *   [36] 0x021189c0  func_ov066_021189c0
 *   [37] 0x02118a30  func_ov066_02118a30
 *   [38] 0x02118a50  func_ov066_02118a50
 *   [39] 0x02118b08  func_ov066_02118b08
 *   [40] 0x02118b28  func_ov066_02118b28
 *   [41] 0x02118be0  func_ov066_02118be0
 *   [42] 0x02118c00  func_ov066_02118c00
 *   [43] 0x02118cb8  func_ov066_02118cb8
 *   [44] 0x02118cdc  func_ov066_02118cdc
 *   [45] 0x02118de0  func_ov066_02118de0
 *   [46] 0x02118e04  func_ov066_02118e04
 *   [47] 0x0211901c  func_ov066_0211901c
 *   [48] 0x0211903c  func_ov066_0211903c
 *   [49] 0x02119348  func_ov066_02119348
 *   [50] 0x02119398  func_ov066_02119398
 *   [51] 0x0211944c  func_ov066_0211944c
 *   [52] 0x02119454  func_ov066_02119454
 *   [53] 0x021194a4  func_ov066_021194a4
 *   [54] 0x021194fc  func_ov066_021194fc
 *   [55] 0x02119654  _ZN6Eyerok16CleanupResourcesEv
 *   [56] 0x021197a0  _ZN6Eyerok16OnPendingDestroyEv
 *   [57] 0x021197a4  _ZN6Eyerok6RenderEv
 *   [58] 0x02119838  _ZN6Eyerok8BehaviorEv
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

/* Local shadow types the shards carried. Anything a real header supplies
 * (Vector3, Matrix4x3, BlendModelAnim, Fix12i, s16/s32/u8/u16/u32) has been
 * dropped in favour of the header. */
enum Bool { FALSE, TRUE };
typedef struct Vec3 { int x, y, z; } Vec3;
struct EVec3 { int x, y, z; };
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(void*); };
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
extern unsigned char data_ov066_0211ae00;
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
extern char data_ov066_0211afec;
extern char data_ov066_0211affc;
extern char data_ov066_0211b00c;
extern char data_ov066_0211b01c;
extern char data_ov066_0211b02c;
extern char data_ov066_0211b03c;
extern char data_ov066_0211b04c;
extern char data_ov066_0211b05c;
extern char data_ov066_0211b06c;
extern char data_ov066_0211b07c;
extern char data_ov066_0211b08c;
extern char data_ov066_0211b09c;
extern char data_ov066_0211b0ac;
extern char data_ov066_0211b0bc;
extern char data_ov066_0211b0cc;
extern char data_ov066_0211b0dc;
extern char data_ov066_0211b0ec;

/* ---- arm9 data ---- */
extern int data_0209e650;
extern void *data_0209f318;
extern int data_020a0e68[];

/* ---- arm9 helpers (unmangled ROM names) ---- */
extern int AngleDiff(int a, int b);
extern int ApproachAngle(s16 *angle, int target, int a, int b, int max);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
extern void Matrix4x3_FromRotationY(void *m, short ang);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void MulVec3Mat4x3(void *a, void *m, void *b);
extern int RandomIntInternal(int *seed);
extern int Vec3_ApproachHorz(void *out, void *a, int maxStep);
extern void Vec3_Asr(void *d, void *s, int sh);
extern int Vec3_Dist(const void *a, const void *b);
extern s16 Vec3_HorzAngle(const void *a, const void *b);
extern int Vec3_HorzDist(const void *a, const void *b);
extern void func_020092c4(void *cam, void *out, void *target);
extern void func_0200d8c8(void *cam, void *v, int strength);
extern void func_02011cfc(void);
extern void func_02011d2c(void);
extern void func_02012694(int a, void *p);
extern void func_020393c4(void *p, void *v);
extern void func_020393d4(void *p, void *v);
extern void func_020398fc(void *p);

/* ---- arm9 / ov002 methods, mangled ROM spelling ---- */
extern void _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(void *out, void *tgt, int step);
extern void _Z14ApproachLinearRiii(int *r, int target, int step);
extern void _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(void *self, void *m, short s);
extern void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void *self, const void *v);
extern void _ZN11ShadowModel12InitCylinderEv(void *self);
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
extern void _ZN6Player16IncMegaKillCountEv(void *p);
extern void _ZN6Player17SetNoControlStateEhih(void *self, unsigned char a, int b, unsigned char c);
extern void _ZN7Message11PrepareTalkEv(void);
extern void _ZN7Message7EndTalkEv(void);
extern void _ZN7fBase_c18MarkForDestructionEv(void *self);
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 a, u32 b, int x, int y, int z, const void *v, void *cb);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int a, int x, int y, int z);
extern void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
extern void *_ZN8dActor_c13ClosestPlayerEv(void *self);
extern void _ZN8dActor_c15HugeLandingDustEb(void *self, int b);
extern void _ZN8dActor_c16TriplePoofDustAtERK7Vector3(void *self, const void *v);
extern int _ZN8dActor_c18HorzAngleToCPlayerEv(void *self);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *self, void *sm, void *m, int rad, int h, unsigned int u);
extern void _ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h(void *self, signed char *a, unsigned int b, const void *v, unsigned int d);
extern void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 id, u32 b, const void *pos, const void *p, int e, int f);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *clsn);
extern u8 _ZN8dActor_c9TrackStarEjj(void *actor, u32 a, u32 b);
extern void _ZN9Animation7AdvanceEv(void *self);
extern int _ZN9Animation8FinishedEv(void *self);
extern int _ZNK9Animation13GetFrameCountEv(void *self);

/* ---- siblings of this TU that stayed in their own src/ files ---- */
extern int func_ov066_0211603c(void *c);
extern void func_ov066_021162e8(void *c);
extern void func_ov066_0211632c(void *c);
extern void func_ov066_02116390(void *c);
extern void func_ov066_021164ec(void *c);
extern void func_ov066_021165cc(void *c);
extern void func_ov066_021166c8(void *c);
extern int func_ov066_021168b0(void *c);
extern int func_ov066_021168ec(void *c);
extern int func_ov066_02116a68(void *c);
extern void func_ov066_02116ac4(void *c, int v);
extern int func_ov066_02116b78(void *c);

/* ---- this TU's own members, forward-declared (mwcc emits .text in reverse
 *      source order, so nearly every intra-TU call is a forward reference) ---- */
extern void func_ov066_021194a4(char *c);
extern void func_ov066_021194fc(char *c);
extern int func_ov066_02119454(void *c, void *p);
}

typedef struct { int w[12]; } M48;

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov066_021184c0, 0x021184c0, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_021184c0
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov066_021184c0(char *p)
{
    *(int *)(p + 0x494) = 0;
    *(int *)(p + 0x498) = 0;
    *(short *)(p + 0x4d0) = 0;
    *(int *)(p + 0x4a0) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov066_021184e0, 0x021184e0, size 0x104 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_021184e0
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov066_021184e0(char *c)
{
    if (func_ov066_021168ec(c) != 0 && func_ov066_021168ec(c) != 4) {
        *(int *)(c + 0xb0) = 0;
        func_ov066_021162e8((int *)c);
        return 1;
    }
    func_ov066_0211632c(c);
    switch (*(int *)(c + 0x4a0)) {
    case 0:
        if (data_ov066_0211ae0c == *(int *)(c + 0x49c)) {
            int *p = (int *)(c + 0x4a0);
            *(int *)(c + 0x9c) = -0x14000;
            *(int *)(c + 0xa8) = 0x64000;
            *p = *p + 1;
        }
        break;
    case 1:
        if (*(int *)(c + 0x9c) != 0) {
            if (*(int *)(c + 0x4a8) >= *(int *)(c + 0x60)) {
                *(int *)(c + 0x60) = *(int *)(c + 0x4a8);
                *(int *)(c + 0xa8) = 0;
                *(int *)(c + 0x9c) = 0;
                func_ov066_02116ac4(c, 0x7d0000);
                if ((data_ov066_0211ae0c & *(int *)(c + 0x49c)) != 0)
                    data_ov066_0211ae0c ^= *(int *)(c + 0x49c);
                *(int *)(c + 0x4a0) = 0;
            }
        }
        break;
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov066_021185e4, 0x021185e4, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_021185e4
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov066_021185e4(char *p)
{
    *(int *)(p + 0x494) = 0;
    *(int *)(p + 0x498) = 0;
    *(short *)(p + 0x4d0) = 0;
    *(int *)(p + 0x4a0) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- func_ov066_02118604, 0x02118604, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_02118604
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov066_02118604(void *c) {
    int r = func_ov066_021168ec(c);
    if (r != 0) {
        data_ov066_0211ae0c ^= *(int*)((char*)c + 0x49c);
        if (data_ov066_0211abe0 != 3) {
            data_ov066_0211ae0c |= 3;
        }
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- func_ov066_02118658, 0x02118658, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_02118658
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov066_02118658(char *p)
{
    *(int *)(p + 0x494) = 0;
    *(int *)(p + 0x498) = 0;
    *(short *)(p + 0x4d0) = 0;
    *(int *)(p + 0x4a0) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- func_ov066_02118678, 0x02118678, size 0x150 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_02118678
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov066_02118678(char* c)
{
    if (*(int*)(c + 0x494) == 0) {
        if (data_ov066_0211ae04 == 2) {
            *(int*)(c + 0x3bc) = 0x1000;
            if (*(int*)(c + 0x49c) == 1) {
                *(int*)(c + 0xa8) = 0x2d000;
                *(int*)(c + 0x9c) = -0x2000;
            } else {
                *(int*)(c + 0xa8) = 0xa000;
                *(int*)(c + 0x9c) = -0x800;
            }
            *(int*)(c + 0x494) = 1;
            func_02012694(0x144, c + 0x74);
        }
        return 1;
    }

    Vec3_ApproachHorz(c + 0x5c, c + 0x4a4, 0x14000);
    if (*(int*)(c + 0x9c) != 0) {
        int v = *(int*)(c + 0x4a8);
        if (v >= *(int*)(c + 0x60)) {
            *(int*)(c + 0x60) = v;
            *(int*)(c + 0xa8) = 0;
            *(int*)(c + 0x9c) = 0;
            func_ov066_02116ac4(c, 0x7d0000);
        }
    }

    if (*(int*)(c + 0x9c) == 0
        && Vec3_HorzDist(c + 0x5c, c + 0x4a4) <= 0x14000
        && _ZN9Animation8FinishedEv(c + 0x3b0)) {
        _ZN4dBgW6EnableEP8dActor_c(c + 0x674, c);
        data_ov066_0211ae0c |= *(int*)(c + 0x49c);
        func_ov066_02119454(c, &data_ov066_0211b06c);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- func_ov066_021187c8, 0x021187c8, size 0xe8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_021187c8
extern "C" {
int func_ov066_021187c8(char* c){
  if(*(int*)(c+0x49c) == 2){
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x360, (void*)data_ov066_0211ae74[1], 4, 0x40000000, 0x1000, 0);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c+0x448, (void*)data_ov066_0211ae3c[1], 0x40000000, 0x1000, 0);
  } else {
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x360, (void*)data_ov066_0211ae7c[1], 4, 0x40000000, 0x1000, 0);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c+0x448, (void*)data_ov066_0211aebc[1], 0x40000000, 0x1000, 0);
  }
  *(int*)(c+0x3bc) = 0;
  *(int*)(c+0x494) = 0;
  *(int*)(c+0x498) = 0;
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- func_ov066_021188b0, 0x021188b0, size 0x84 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_021188b0
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov066_021188b0(char* c){
  if(data_ov066_0211abe0==0){
    *(unsigned short*)(c+0x400+0xd2)=0x64;
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- func_ov066_02118934, 0x02118934, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_02118934
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov066_02118934(char *p)
{
    *(int *)(p + 0x494) = 0;
    *(int *)(p + 0x498) = 0;
    *(short *)(p + 0x4d0) = 0;
    *(int *)(p + 0x4a0) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 34 -- func_ov066_02118954, 0x02118954, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_02118954
extern "C" {  /* .c-derived member: C linkage for the whole block */
s32 func_ov066_02118954(char* c) {
    s32 r = func_ov066_021168b0(c);
    if (r == 0) {
        return 1;
    }
    *(char*)(c + 0x4d9) = 0;
    *(char*)&data_ov066_0211ae10 = 0;
    func_ov066_02119454(c, &data_ov066_0211b03c);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 35 -- func_ov066_021189a0, 0x021189a0, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_021189a0
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov066_021189a0(char *p)
{
    *(int *)(p + 0x494) = 0;
    *(int *)(p + 0x498) = 0;
    *(short *)(p + 0x4d0) = 0;
    *(int *)(p + 0x4a0) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 36 -- func_ov066_021189c0, 0x021189c0, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_021189c0
extern "C" {  /* .c-derived member: C linkage for the whole block */
int RandomIntInternal(int* seed);
int func_ov066_021189c0(char* c){
  if(func_ov066_021168b0(c) == 0) return 1;
  if((((unsigned int)RandomIntInternal(&data_0209e650) >> 0x1f) & 1) == 0)
    data_ov066_0211ae0c = 2;
  else
    data_ov066_0211ae0c = 1;
  c[0x4d9] = 0;
  func_ov066_02119454(c, &data_ov066_0211b03c);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 37 -- func_ov066_02118a30, 0x02118a30, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_02118a30
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov066_02118a30(char *p)
{
    *(int *)(p + 0x494) = 0;
    *(int *)(p + 0x498) = 0;
    *(short *)(p + 0x4d0) = 0;
    *(int *)(p + 0x4a0) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 38 -- func_ov066_02118a50, 0x02118a50, size 0xb8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_02118a50
extern "C" {  /* .c-derived member: C linkage for the whole block */
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
        unsigned char* p = (unsigned char*)(((int)c + 0x4d9));
        *p += 1;
    }
    data_ov066_0211ae10 += 1;
    data_ov066_0211ae10 &= 1;
    func_ov066_02119454(c, &data_ov066_0211b03c);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 39 -- func_ov066_02118b08, 0x02118b08, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_02118b08
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov066_02118b08(char *p)
{
    *(int *)(p + 0x494) = 0;
    *(int *)(p + 0x498) = 0;
    *(short *)(p + 0x4d0) = 0;
    *(int *)(p + 0x4a0) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 40 -- func_ov066_02118b28, 0x02118b28, size 0xb8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_02118b28
extern "C" {  /* .c-derived member: C linkage for the whole block */
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
        unsigned char* p = (unsigned char*)(((int)c + 0x4d9));
        *p += 1;
    }
    data_ov066_0211ae10 += 1;
    data_ov066_0211ae10 &= 1;
    func_ov066_02119454(c, &data_ov066_0211b03c);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 41 -- func_ov066_02118be0, 0x02118be0, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_02118be0
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov066_02118be0(char *p)
{
    *(int *)(p + 0x494) = 0;
    *(int *)(p + 0x498) = 0;
    *(short *)(p + 0x4d0) = 0;
    *(int *)(p + 0x4a0) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 42 -- func_ov066_02118c00, 0x02118c00, size 0xb8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_02118c00
extern "C" {  /* .c-derived member: C linkage for the whole block */
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
        unsigned char* p = (unsigned char*)(((int)c + 0x4d9));
        *p += 1;
    }
    data_ov066_0211ae10 += 1;
    data_ov066_0211ae10 &= 1;
    func_ov066_02119454(c, &data_ov066_0211b03c);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 43 -- func_ov066_02118cb8, 0x02118cb8, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_02118cb8
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov066_02118cb8(char *p)
{
    *(int *)(p + 0x494) = 0;
    *(int *)(p + 0x498) = 0;
    *(short *)(p + 0x4d0) = 30;
    *(int *)(p + 0x4a0) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 44 -- func_ov066_02118cdc, 0x02118cdc, size 0x104 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_02118cdc
struct dActor_c;
typedef struct { int x; } Obj44;
typedef struct { int y; } Pmf44;
extern "C" {

int func_ov066_02118cdc(char* c) {
    if (func_ov066_021168b0(c) == 0)
        return 1;
    if (*(unsigned short*)(c + 0x4d0) == 0) {
        if (func_ov066_02116a68(c) != (int)0xff3ae000) {
            if (data_ov066_0211ae0c == 0) {
                data_ov066_0211ae04 = 3;
                *(unsigned short*)(c + 0x4d2) = 0x1e;
                func_ov066_02119454(c, &data_ov066_0211b0cc);
            }
            return 1;
        }
    }
    if (data_ov066_0211ae0c == 0) {
        if (data_ov066_0211abe0 == 3) {
            if (*(int*)(c + 0x494) == 0)
                data_ov066_0211ae0c = 1;
            else
                data_ov066_0211ae0c = 2;
        } else {
            data_ov066_0211ae0c = data_ov066_0211abe0;
        }
        volatile int* tmp = (volatile int*)(((int)c + 0x494));
        *tmp = *tmp + 1;
        *tmp = *tmp & 1;
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 45 -- func_ov066_02118de0, 0x02118de0, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_02118de0
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov066_02118de0(char *p)
{
    *(int *)(p + 0x494) = 0;
    *(int *)(p + 0x498) = 0;
    *(short *)(p + 0x4d0) = 30;
    *(int *)(p + 0x4a0) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 46 -- func_ov066_02118e04, 0x02118e04, size 0x218 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_02118e04
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov066_02118e04(void* self)
{
    u8* c = (u8*)self;
    void* p = _ZN8dActor_c13ClosestPlayerEv(self);
    int r4;
    int v;

    if (data_ov066_0211abe0 == 0) {
        *(u16*)(c + 0x4d2) = 0x64;
        func_ov066_02119454(c, &data_ov066_0211b0ac);
        return 1;
    }

    if (p == 0 || *(u16*)(c + 0x4d2) != 0)
        return 1;

    r4 = ((unsigned int)RandomIntInternal(&data_0209e650) >> 31) & 1;

    data_ov066_0211ae08 = 0;
    data_ov066_0211ae0c = 0;
    v = func_ov066_02116a68(c);
    if (v == (int)0xff3ae000) {
        data_ov066_0211ae04 = 4;
        func_ov066_02119454(c, &data_ov066_0211b0dc);
        return 1;
    }

    if ((int)*(u8*)(c + 0x4d9) > data_ov066_0211abe4 + 3) {
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

    if (r4 == 0) {
        data_ov066_0211ae04 = 7;
        func_ov066_02119454(c, &data_ov066_0211affc);
    } else {
        data_ov066_0211ae04 = 6;
        func_ov066_02119454(c, &data_ov066_0211afdc);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 47 -- func_ov066_0211901c, 0x0211901c, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_0211901c
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov066_0211901c(char *p)
{
    *(int *)(p + 0x494) = 0;
    *(int *)(p + 0x498) = 0;
    *(short *)(p + 0x4d0) = 0;
    *(int *)(p + 0x4a0) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 48 -- func_ov066_0211903c, 0x0211903c, size 0x30c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_0211903c
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov066_0211903c(char* self) {
    struct Vector3 v1, v2, in, out, star;
    void* cam;
    int msgid;

    if (*(unsigned short*)(self + 0x4d2)) return 1;

    cam = data_0209f318;
    if (*(int*)(self + 0x4a0) == 0) {
        _ZN6Camera9SetFlag_3Ev(cam);
        *(void**)(self + 0x490) = _ZN8dActor_c13ClosestPlayerEv(self);
        if (*(void**)(self + 0x490) != 0)
            _ZN6Player17SetNoControlStateEhih(*(void**)(self + 0x490), 5, -1, 0);
        *(int*)(self + 0x4a0) = 1;
    } else {
        v1.x = *(int*)(self + 0x5c);
        v1.y = *(int*)(self + 0x60);
        v1.z = *(int*)(self + 0x64);
        v2.x = *(int*)(self + 0x5c);
        v2.y = *(int*)(self + 0x60);
        v2.z = *(int*)(self + 0x64);
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

    if (*(int*)(self + 0x498) == 0) {
        if (*(void**)(self + 0x490) != 0) {
            in.x = 0; in.y = 0; in.z = 0;
            out.x = 0; out.y = 0; out.z = 0;
            in.y = 0x32000;
            in.z = -0x32000;

            Matrix4x3_FromRotationY(data_020a0e68, 0);
            MulVec3Mat4x3(&in, data_020a0e68, &out);

            out.x += *(int*)(self + 0x5c);
            out.y += *(int*)(self + 0x60);
            out.z += *(int*)(self + 0x64);

            msgid = 0xb8;
            if (data_ov066_0211abe0 == 0) {
                msgid = 0xb9;
                _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x14, 0x15666);
            }

            *(unsigned short*)(((int)*(void**)(self + 0x490) + 0x6ce)) |= 0x400;
            _ZN7Message11PrepareTalkEv();
            if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(*(void**)(self + 0x490), self, msgid, &out, 0, 0) == 1) {
                *(int*)(self + 0x498) = 1;
                func_02012694(0x145, self + 0x74);
            }
        }
    } else {
        if (*(void**)(self + 0x490) != 0) {
            if (_ZN6Player12GetTalkStateEv(*(void**)(self + 0x490)) < 0) {
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
                    _ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h(self, (signed char*)(self + 0x673), *(unsigned char*)(self + 0x672), &star, 4);
                    _ZN7fBase_c18MarkForDestructionEv(self);
                }
            }
        }
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 49 -- func_ov066_02119348, 0x02119348, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_02119348
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov066_02119348(void *c)
{
    if (_ZN4dBgW9IsEnabledEv((char *)c + 0x674) != 0) {
        _ZN4dBgW7DisableEv((char *)c + 0x674);
    }
    *(int *)((char *)c + 0x494) = 0;
    *(int *)((char *)c + 0x498) = 0;
    *(short *)((char *)c + 0x4d0) = 0;
    *(int *)((char *)c + 0x4a0) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 50 -- func_ov066_02119398, 0x02119398, size 0xb4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_02119398
struct Vec4 { int a, b, c, d; ~Vec4(){} };
extern "C" {

int func_ov066_02119398(char* c)
{
    Vec4 sp;
    char* p = (char *)_ZN8dActor_c13ClosestPlayerEv(c);
    if (p != 0) {
        char* sp2 = p + 0x5c;
        int v1 = *(int*)(sp2 + 4);
        int v2 = *(int*)(sp2 + 8);
        if (v1 < -0x300000) {
            int f = (int)((*(int*)(c + 0xb0) & 8) != 0);
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 51 -- func_ov066_0211944c, 0x0211944c, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_0211944c
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov066_0211944c(void)
{
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 52 -- func_ov066_02119454, 0x02119454, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_02119454
struct C { char pad[0x48c]; PMF *pp; };
extern "C" int func_ov066_02119454(void *cv, void *pv) { C *c = (C *)cv; PMF *p = (PMF *)pv; c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 53 -- func_ov066_021194a4, 0x021194a4, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_021194a4
extern "C" void func_ov066_021194a4(char *c) {
  Matrix4x3_FromRotationY(c+0x83c, *(short*)(c+0x8e));
  *(int*)(c+0x860) = *(int*)(c+0x5c);
  *(int*)(c+0x864) = *(int*)(c+0x60);
  *(int*)(c+0x868) = *(int*)(c+0x64);
  _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(c+0x674, c+0x83c, *(short*)(c+0x8e));
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 54 -- func_ov066_021194fc, 0x021194fc, size 0x158 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov066_021194fc
extern "C" {


void func_ov066_021194fc(char* c)
{
    int v[3];
    Vec3_Asr(v, c + 0x5c, 3);
    Matrix4x3_FromTranslation(data_020a0e68, v[0], v[1], v[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(data_020a0e68, *(short*)(c + 0x8c), *(short*)(c + 0x8e), *(short*)(c + 0x90));
    if (*(int*)(c + 0x49c) == 0)
        *(M48*)(c + 0x3ec) = *(M48*)data_020a0e68;
    else
        *(M48*)(c + 0x37c) = *(M48*)data_020a0e68;
    if (*(int*)(c + 0x49c) == 0)
        return;
    if (*(int*)(c + 0x4a8) >= *(int*)(c + 0x60))
        return;
    {
        int d;
        if (*(int*)(c + 0x49c) == 2)
            d = 0x64000;
        else
            d = -0x64000;
        Matrix4x3_FromTranslation(data_020a0e68,
            (*(int*)(c + 0x5c) + d) >> 3,
            (*(int*)(c + 0x60) - 0x8000) >> 3,
            (*(int*)(c + 0x64) + 0xa0000) >> 3);
    }
    *(M48*)(c + 0x45c) = *(M48*)data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x420, c + 0x45c, 0x140000, 0x258000, 0xf);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 55 -- _ZN6Eyerok16CleanupResourcesEv, 0x02119654, size 0x14c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Eyerok16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 56 -- _ZN6Eyerok16OnPendingDestroyEv, 0x021197a0, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Eyerok16OnPendingDestroyEv
/* Eyerok::OnPendingDestroy -- vtable slot 12. The ROM body is empty: the
 * override exists only to occupy the slot. */
void Eyerok::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 57 -- _ZN6Eyerok6RenderEv, 0x021197a4, size 0x94 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Eyerok6RenderEv
/* recovered: named members + shared header, real C++ method */
int Eyerok::Render()
{
  if (mPartIdx == 0) {
    if (data_ov066_0211ae04 == 1) {
      ((Sub*)&mModel2)->g5(0);
    }
    return 1;
  }
  if (data_ov066_0211ae04 == 1) return 1;
  _ZN15TextureSequence6UpdateER15ModelComponents(&mTextureSequence, &mBlendModelAnim.data);
  ((Sub*)&mBlendModelAnim)->g5(0);
  return 1;
}


/* Bracketed, and it binds only because of the file-top
 * `#pragma defer_codegen off`: with codegen deferred (mwccarm 2004/b56s
 * default) a bracketed opt_* pragma does not bind and these two go
 * file-global, which costs func_ov066_021184e0 (4 words) and
 * func_ov066_021194fc (a size change).  Deleting them outright instead
 * costs _ZN6Eyerok8BehaviorEv, 0x4b0 -> 0x4ac. */
#pragma opt_common_subs off
#pragma opt_strength_reduction off
/* -------------------------------------------------------------------------- */
/* ROM ordinal 58 -- _ZN6Eyerok8BehaviorEv, 0x02119838, size 0x4b0 */
/* -------------------------------------------------------------------------- */
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

//cpp
/* Production translation unit for ov078/daBombking_c -- the King Bob-omb that
 * holds the star at the top of Bob-omb Battlefield.
 *
 * SM64DS RTTI names this class daBombking_c.  ov078 carries _ZTS12daBombking_c
 * at 0x02126e18 and _ZTI12daBombking_c at 0x02126e0c, and the vtable's -4
 * header word points back at that _ZTI.  The tree used to carry the class under
 * the coined name KingBobOmb; this file and its header use the cartridge's own
 * spelling. Member and helper identifiers remain reconstructed; in particular,
 * KingBobOmb_SetState at 0x02125c48 retains its coined spelling.
 *
 * The base is dEnemyBase_c.  _ZTV12daBombking_c is 31 slots with its address
 * point at ov078 0x02126e4c, and this class overrides 8 of them -- slots 0, 3,
 * 6, 9, 12, 16, 17 and 29, read back with tools/rtti_vtables.py --own.  All 8
 * override bodies are inside the run below.
 *
 * The TU covers 52 functions in the contiguous .text run
 * 0x02123740..0x02126660. The factory at 0x021265fc occupies the final 0x64
 * bytes before .init. The historical queue counted 51 shards before folding;
 * the regenerated promoted row now counts this one source file.
 *
 * TEXT-ONLY ROUTE.  ov078's delinks.txt carves out no .data whatsoever, so this
 * TU claims .text and nothing else.  It is nonetheless the class's KEY-FUNCTION
 * TU -- the out-of-line destructor is the key function -- so mwccarm emits
 * _ZTV/_ZTI/_ZTS for the whole inherited chain as vague linkage.  Each of those
 * is licensed as compiler-only output in the manifest and dropped before the
 * link; the cartridge's own copies are what the ROM keeps, and
 * tools/romdata_check.py compares this object's emitted RTTI against them.
 *
 * FUNCTION ORDER IS ROM-ASCENDING. With defer_codegen off, mwccarm 2004/b56
 * emits the functions in source order, with the lowest ROM address first.
 * Keep that order. A destructor's D0/D1/D2
 * group has a compiler-chosen order of its own; see the destructor comment in
 * the class header for what was measured on this TU.
 *
 * deslop
 * Leftover: BlendModelAnim::SetAnim / dCcAcPos_c::Init / dBgCh_Actr::Init /
 *   DropShadowRadHeight stay mangled (Fix12-by-value, 6az; dBgCh Init header
 *   Fix12i mangles as int -- this TU's InitResources call). dActor_c::Spawn
 *   s8/s16 by-value (func_ov078_02125350). Player+8 param1 / +0x6ce talk flag
 *   / +0xc8 mtx ptr belong on Player. Camera+0x114 / +0x154 belong on Camera.
 *   data_ov078_* SharedFilePtr handles (Init LoadFile / Cleanup Release) and
 *   state records (KingBobOmb_SetState). func_02035550 is the unnamed
 *   dBgCh_Actr mFlags |= 0x4000 setter (no method on the header). S14 no
 *   g_profile_BOMBKING. common.h first (M12
 *   shadow/hold matrix copies at 0x434 and 0x4a4).
 *
 * Folded from 52 one-function sources, each of which was its own file in src/
 * before this promotion and none of which is in the tree any more.  Listed by
 * the symbol assigned in the repository's symbols.txt, in ROM order:
 *   [0] 0x02123740  _ZN12daBombking_cD1Ev
 *   [1] 0x02123798  _ZN12daBombking_cD0Ev
 *   [2] 0x02123804  func_ov078_02123804
 *   [3] 0x02123864  func_ov078_02123864
 *   [4] 0x021238ac  func_ov078_021238ac
 *   [5] 0x02123a3c  func_ov078_02123a3c
 *   [6] 0x02123aa0  func_ov078_02123aa0
 *   [7] 0x02123bc4  func_ov078_02123bc4
 *   [8] 0x02123c20  func_ov078_02123c20
 *   [9] 0x02123cf0  func_ov078_02123cf0
 *   [10] 0x02123d3c  func_ov078_02123d3c
 *   [11] 0x02123eb8  func_ov078_02123eb8
 *   [12] 0x02123f1c  func_ov078_02123f1c
 *   [13] 0x02123fb4  func_ov078_02123fb4
 *   [14] 0x02124000  func_ov078_02124000
 *   [15] 0x02124060  func_ov078_02124060
 *   [16] 0x021240a0  func_ov078_021240a0
 *   [17] 0x021243c0  func_ov078_021243c0
 *   [18] 0x02124470  func_ov078_02124470
 *   [19] 0x021244d0  func_ov078_021244d0
 *   [20] 0x02124520  func_ov078_02124520
 *   [21] 0x02124778  func_ov078_02124778
 *   [22] 0x021247bc  func_ov078_021247bc
 *   [23] 0x02124b40  func_ov078_02124b40
 *   [24] 0x02124bc4  func_ov078_02124bc4
 *   [25] 0x02124c94  func_ov078_02124c94
 *   [26] 0x02124cf4  func_ov078_02124cf4
 *   [27] 0x02124e9c  func_ov078_02124e9c
 *   [28] 0x02124f28  func_ov078_02124f28
 *   [29] 0x021250d0  func_ov078_021250d0
 *   [30] 0x021250f8  func_ov078_021250f8
 *   [31] 0x02125350  func_ov078_02125350
 *   [32] 0x02125448  func_ov078_02125448
 *   [33] 0x02125734  func_ov078_02125734
 *   [34] 0x02125790  func_ov078_02125790
 *   [35] 0x021258e4  func_ov078_021258e4
 *   [36] 0x02125950  func_ov078_02125950
 *   [37] 0x021259e4  func_ov078_021259e4
 *   [38] 0x021259ec  func_ov078_021259ec
 *   [39] 0x02125bc8  func_ov078_02125bc8
 *   [40] 0x02125c24  func_ov078_02125c24
 *   [41] 0x02125c48  KingBobOmb_SetState
 *   [42] 0x02125c98  func_ov078_02125c98
 *   [43] 0x02125de0  func_ov078_02125de0
 *   [44] 0x02125f8c  func_ov078_02125f8c
 *   [45] 0x02125ff4  _ZN12daBombking_c16CleanupResourcesEv
 *   [46] 0x021260a8  _ZN12daBombking_c16OnPendingDestroyEv
 *   [47] 0x021260ac  _ZN12daBombking_c6RenderEv
 *   [48] 0x02126104  _ZN12daBombking_c8BehaviorEv
 *   [49] 0x02126368  _ZN12daBombking_c13InitResourcesEv
 *   [50] 0x021265f4  _ZN12daBombking_c16OnAimedAtWithEggEv
 *   [51] 0x021265fc  daBombking_c_classInit
 */

/* TUBUILD NOTE -- #pragma directive(s) were present in the legacy sources
 * of this TU. `#pragma long_calls` is POSITIONAL in mwccarm 2004/b56 and is
 * carried verbatim before its own member below, bracketed with `off` so it
 * cannot leak into later members (dropping it silently costs the pooled
 * cross-overlay tail-call -- a byte diff; see ShutterBob in ov014).
 * Any OTHER pragma is FILE-GLOBAL last-wins (opt_propagation,
 * optimize_for_size) and is still left out: carried into a merged TU it
 * would silently recompile every other member. Decide those by hand:
 *   func_ov078_02125448: opt_strength_reduction off / opt_common_subs off
 *     carried, #pragma push/pop around the member (not 02125350).
 *   InitResources: opt_strength_reduction off carried, push/pop.
 */

/* common.h FIRST: daBombking_c.h reaches math/Matrix.h through BlendModelAnim.h,
 * and that header spells Matrix4x3 as {Matrix3x3 r; Vector3 t;} where common.h
 * spells it flat as s32 m[12]. Helpers whole-struct-assign the shadow/hold
 * matrices through M12; only the flat spelling reproduces the block move. */
#include "common.h"
#include "daBombking_c.h"
#include "daBmb_c.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"
#include "decl_common.h"
#include "decl_Animation.h"
#include "decl_Message.h"

/* Remaining reconstruction views. Reconciled by hand against include/: every type
 * the real headers already define (Vector3, Matrix4x3, Fix12<int>, u8/u16/s16,
 * dActor_c, fBase_c, Player, BMD_File, BlendModelAnim) was DROPPED here -- the
 * generated preamble redefined all of them. The shapes below are surviving
 * views from the legacy shards, not evidence of original local classes. */

/* File-local flat matrix. math/Matrix.h spells Matrix4x3 as {Matrix3x3 r;
 * Vector3 t;} while common.h spells it flat; whole-matrix assignment must stay
 * flat or mwcc splits it into ldm/stm + a CSE'd tail. */
struct M12 { int w[12]; };

struct Vec3 { int x, y, z; };

/* Whole-struct 3-word copies MUST go through an array member. mwccarm block-moves
 * (ldm/stm) a POD struct copy under -lang c99 but scalarises the same copy under
 * -lang c++; an array member forces the block move back. Four members of this TU
 * turn on this one rule. */
struct M3 { int w[3]; };
struct BCA_File;
typedef daBombking_c::StateFunction PMF;
extern "C" int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *self, void *bca, int a, int b, int fix, unsigned short t);
extern "C" int KingBobOmb_SetState(void *c, void *p);
typedef struct { int a, b; } P2;
struct G { int w[2]; };

/* Raw-offset views retained from the legacy shards. Their remaining fields
 * still need reconstruction and local compiler measurements; a CSE result
 * from another class does not establish a constraint here. */
struct CView {
    char pad0[0x8e];
    short field_8e;
    char pad90[0x31c-0x90];
    char anim_31c;
    char pad31d[0x430-0x31d];
    int field_430;
    char pad434[0x494-0x434];
    int field_494;
};


extern "C" {
    extern int data_ov078_02126ee0[];
    extern int data_ov078_02126ee8[];
    extern int data_ov078_02126ef0[];
    extern int data_ov078_02126ef8[];
    extern int data_ov078_02126f00[];
    extern int data_ov078_02126f08[];
    extern int data_ov078_02126f10[];
    extern int data_ov078_02126f18[];
    extern int data_ov078_02126f20[];
    extern int data_ov078_02126f28[];
    extern int data_ov078_02126f30[];
    extern int data_ov078_02126f38[];
    extern int data_ov078_02126f40[];
    /* Canonical single declaration per ROM data symbol -- the generated
     * preamble carried up to three contradictory spellings of each. */
    extern unsigned char data_0209f220[];
    extern void *data_0209f318;
    extern struct M12 data_020a0e68;
    extern int data_ov078_0212703c[];
    extern int data_ov078_0212705c[];
    extern int data_ov078_0212707c[];
    extern int data_ov078_0212709c[];
    extern int data_ov078_021270cc[];
    extern int data_ov078_021270dc[];
    extern int data_ov078_021270fc[];
    extern int data_ov078_0212710c[];
extern int _ZN8dActor_c10FindWithIDEj(unsigned int id);
extern int data_ov078_02126ffc[];
extern int _ZN6Player12GetTalkStateEv(void *p);
extern void _ZN6Camera9SetFlag_3Ev(void *cam);
extern void MulMat4x3Mat4x3(void *dst, void *a, void *b);
extern void Vec3_Lsl(void *d, void *s, int sh);
extern void func_02012694(int a, void *p);
extern void _ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h(void *c, signed char *s, unsigned int n, struct Vec3 *pos, unsigned int u);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void _ZN8dActor_c16TriplePoofDustAtERK7Vector3(void *c, struct Vec3 *pos);
extern void _ZN5Sound22StopLoadedMusic_Layer3Ev(void);
extern void func_02011cfc(void);
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int a, int b);
extern void _ZN7fBase_c18MarkForDestructionEv(void *c);
extern short Vec3_HorzAngle(const void* a, const void* b);
extern char* _ZN8dActor_c13ClosestPlayerEv(void* c);
extern int Vec3_Dist(const void* a, const void* b);
extern void ApproachAngle(void *p, int target, int a, int b, int c);
extern "C" int _ZN6Player9GetHealthEv(void);
extern "C" int _ZN9Animation8FinishedEv(void* anim);
extern s16 _ZN8dActor_c18HorzAngleToCPlayerEv(void*);   /* dActor_c.h:177 -- s16, not int */
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* c);
extern int _ZN6Player9StartTalkER7fBase_cb(void* pl, char* c, int b);
extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void* pl, char* c, int m, void* v, int a, int b);
extern void _ZN8dActor_c15HugeLandingDustEb(char* c, int b);
extern int RandomIntInternal(int* seed);
extern void _ZN6Player12Unk_020c6a10Ej(void* p, unsigned int a);
extern int data_0209e650;
extern int _ZNK9Animation12WillHitFrameEi(void *anim, int frame);
extern void _ZN8dActor_c17HugeLandingDustAtER7Vector3b(void *actor, Vector3 *v, int b);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void MulVec3Mat4x3(Vector3 *v, void *m, Vector3 *out);
extern void _ZN6Camera9SetLookAtERK7Vector3(void *cam, Vector3 *v);
extern void _ZN6Camera6SetPosERK7Vector3(void *cam, Vector3 *v);
extern void _Z14ApproachLinearRsss(s16 *cur, s16 tgt, s16 step);
extern short Vec3_VertAngle(const void *a, const void *b);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, int angle);
extern void func_0200fa8c(void *c, int a);
extern void _ZN10dBgCh_Actr15ClearGroundFlagEv(void *thiz);
extern void func_ov102_0214b384(void* a, int b);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
extern int Vec3_HorzLen(void* v);
extern int _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as( unsigned int a, unsigned int b, void* pos, int rot, int e, int f);
extern int _ZN6Player7TryGrabER8dActor_c(void* p, void* a);
extern int func_ov002_020db5f4(char* c, char* arg);
extern int _ZN8dActor_c13DistToCPlayerEv(void* c);
extern void func_0200d8c8(void* cam, void* v, int strength);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* self, void* sm, void* mtx, int fix, int t, unsigned int j);
extern Matrix4x3 IDENTITY_MATRIX4X3;
extern void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void* self, dActor_c* a, Vector3* v, Fix12i r, Fix12i h, unsigned int e, unsigned int g);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, dActor_c* a, Fix12i r, Fix12i h, Vector3_16* p, Vector3_16* q);
}


/* `opt_strength_reduction`, `opt_common_subs` and `opt_propagation` are
 * file-global last-wins in mwccarm 2004/b56, so the three members that need
 * them would otherwise recompile the other 48. `defer_codegen off` makes the
 * positional push/pop brackets below actually bind -- and it also flips .text
 * emission from reversed to source order, which is why this file is written
 * ROM-ASCENDING. The two are one decision: linkcheck [4b/8] refuses the mix.
 */
#pragma defer_codegen off

/* Out of line, and FIRST in this ROM-ascending file: that is what puts D1 and
 * D0 at 0x02123740 / 0x02123798, the two lowest addresses in the run. */
daBombking_c::~daBombking_c() {}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * A consequence of `struct daBombking_c : dEnemyBase_c`: its own vptr store,
 * then the destructors of the five members that have one -- dBgCh_Actr at
 * +0x110, BlendModelAnim at +0x2cc, dCcAcPos_c at +0x33c and +0x37c, and
 * CommonModel at +0x3bc -- then the base chain. Each of those member calls is
 * a relocation the ROM build checks, which is what named the members.
 *
 * DEFINED OUT OF LINE at the top of this file, and DECLARED FIRST in the
 * header. Both halves are load-bearing; see the block above the definition.
 */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_cD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases --
 * which is why more than one vptr store appears -- then return the object to
 * its heap. Nobody writes that; declaring `~daBombking_c()` is enough, because
 * mwcc emits D1, D0 and D2 together (measured on this TU, in that order) and
 * objisolate keeps the ones this file is bound to. D2 is homeless -- the ROM
 * has no D2 -- and is licensed as `deadstrip` in the manifest.
 *
 * The deallocation is an inline operator delete, which is why nothing below
 * mentions a heap.
 */

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02123804
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov078_02123804(char *c){
    unsigned int v=*(unsigned int*)(c+0x3a0);
    if(v==0) return 0;
    if(_ZN8dActor_c10FindWithIDEj(v)==0) return 0;
    if((*(int*)(c+0x39c) & 0x4000)==0) return 0;
    KingBobOmb_SetState(c, data_ov078_02126ffc);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02123864
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov078_02123864(char* r7) {
  int i = 0;
  do {
    daBmb_c *bmb = (daBmb_c *)_ZN8dActor_c10FindWithIDEj(((unsigned int*)(r7 + 0x424))[i]);
    if (bmb) {
      bmb->unk_3e0 = 0;
      bmb->unk_3f6 = 1;
    }
    i++;
  } while (i < 2);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_021238ac
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov078_021238ac(char *c)
{
    void *cam = data_0209f318;
    struct Vec3 v;
    struct Vec3 t;
    struct Vec3 u;

    if (_ZN6Player12GetTalkStateEv(*(void **)(c + 0x430)) != -1) {
        _ZN6Camera9SetFlag_3Ev(cam);
        return 1;
    }

    {
        int *f = (int *)((char *)cam + 0x154);
        *f &= ~8;
    }
    v.x = 0;
    v.y = 0;
    v.z = 0;
    data_020a0e68 = *(struct M12 *)(c + 0x2e8);
    MulMat4x3Mat4x3((char *)*(void **)(c + 0x2e0) + 0x30, &data_020a0e68, &data_020a0e68);

    v.x = data_020a0e68.w[0x24 / 4];
    v.y = data_020a0e68.w[0x28 / 4];
    v.z = data_020a0e68.w[0x2c / 4];
    Vec3_Lsl(&t, &v, 3);
    v.x = t.x;
    v.y = t.y;
    v.z = t.z;
    func_02012694(0x130, c + 0x74);

    _ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h(
        c, (signed char *)(c + 0x507), *(u8 *)(c + 0x509), &v, 4);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x1a, v.x, v.y, v.z);
    u.x = v.x;
    u.y = v.y;
    u.z = v.z;
    _ZN8dActor_c16TriplePoofDustAtERK7Vector3(c, &u);

    if (*(u8 *)(c + 0x506) == 1) {
        *(u8 *)(c + 0x506) = 0;
        _ZN5Sound22StopLoadedMusic_Layer3Ev();
        func_02011cfc();
        _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x15666);
    }
    _ZN7fBase_c18MarkForDestructionEv(c);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02123a3c
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov078_02123a3c(char* c){
    *(int*)(c+0x4fc)=2;
    *(int*)(c+0x98)=0;
    *(short*)(c+0x100)=0x32;
    func_ov078_02123864((char*)c);
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((void*)(c+0x2cc), (void*)(data_ov078_02126f20[1]), 0, 0, 0x1000, 0);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02123aa0
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov078_02123aa0(char* c){
    short ang = Vec3_HorzAngle(c+0x5c, c+0x4e0);
    if(func_ov078_02123804(c) == 1) return 1;
    char* p = _ZN8dActor_c13ClosestPlayerEv(c);
    if(p != 0){
        struct Vector3 v = *(struct Vector3*)(p+0x5c);
        if(Vec3_Dist(c+0x4d4, &v) < 0x640000){
            if(*(int*)(c+0x4d8) - 0x64000 < v.y){
                KingBobOmb_SetState(c, data_ov078_0212703c);
                return 1;
            }
        }
    }
    if(*(unsigned char*)(c+0x505) == 0){
        func_02012694(0x12d, c+0x74);
        *(unsigned char*)(c+0x505) = 5;
    }
    ApproachAngle((short*)(c+0x94), ang, 5, 0x1000, 0x100);
    *(short*)(c+0x8e) = *(short*)(c+0x94);
    if(Vec3_Dist(c+0x5c, c+0x4e0) < 0x32000){
        KingBobOmb_SetState(c, data_ov078_0212710c);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02123bc4
/* This caller retains the reconstructed SetAnim ABI declaration above.
   The callee's Fix12 definition experiment in notes/mwccarm-codegen.md 6az
   does not establish that a typed call here would fail to match. */
extern "C" int func_ov078_02123bc4(char* c){
  *(int*)(c+0x9c)=-0x2000;
  *(int*)(c+0x4fc)=2;
  *(int*)(c+0x98)=0xa000;
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((void*)((BlendModelAnim*)(c+0x2cc)), (void*)(data_ov078_02126ee8[1]), 4, 0, 0x1000, 0);
  return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02123c20
extern "C" {
extern int _ZNK9Animation12WillHitFrameEi(void* self, int f);
extern int _ZN9Animation8FinishedEv(void* self);

int func_ov078_02123c20(char* c){
    if(func_ov078_02123804(c) == 1){
        func_ov078_02123864((char*)c);
        int v = *(int*)(c+0x494);
        if(v != 0){
            func_ov002_020db54c((char*)v, 0, 0x50000, *(short*)(c+0x8e));
            *(int*)(c+0x430) = *(int*)(c+0x494);
            *(int*)(c+0x494) = 0;
        }
        return 1;
    }
    if(_ZNK9Animation12WillHitFrameEi(c+0x31c, 0x14)){
        int v = *(int*)(c+0x494);
        if(v != 0){
            func_ov002_020db54c((char*)v, 0x28000, 0x50000, *(short*)(c+0x8e));
            *(int*)(c+0x430) = *(int*)(c+0x494);
            *(int*)(c+0x494) = 0;
            func_02012694(0x131, c+0x74);   /* ROM: the 3rd arg the shard declared is inert -- measured both ways, identical bytes */
        }
    }
    if(_ZN9Animation8FinishedEv(c+0x31c)){
        KingBobOmb_SetState(c, data_ov078_021270fc);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02123cf0
struct BCA_File;
extern "C" {
int func_ov078_02123cf0(char* c){
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((void*)(c+0x2cc), (void*)(data_ov078_02126ef0[1]), 0, 0x40000000, 0x1000, 0);
  *(int*)(c+0x98)=0; return 1;
}}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02123d3c
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov078_02123d3c(char* c)
{
    if (func_ov078_02123804(c) == 1) {
        func_ov078_02123864((char*)c);
        if (*(int*)(c + 0x494) != 0) {
            func_ov002_020db54c((char*)*(int*)(c + 0x494), 0, 0x50000, *(s16*)(c + 0x8e));
            *(int*)(c + 0x430) = *(int*)(c + 0x494);
            *(int*)(c + 0x494) = 0;
        }
        return 1;
    }

    Vector3 v;
    v.x = *(int*)(c + 0x4d4);
    v.y = *(int*)(c + 0x4d8);
    v.z = *(int*)(c + 0x4dc);

    if (data_0209f220[0] != 1) {
        v.x = *(int*)(c + 0x5c);
        v.y = *(int*)(c + 0x60);
        v.z = *(int*)(c + 0x64);
        *(s16*)(((int)c + 0x94)) += 0x1000;
        *(int*)(c + 0x98) = 0;
    } else {
        s16 a = Vec3_HorzAngle((Vector3*)(c + 0x5c), &v);
        ApproachAngle(c + 0x94, a, 5, 0x1000, 0x100);
    }

    *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);

    if (Vec3_Dist((Vector3*)(c + 0x5c), &v) > 0x1e000) {
        if (*(u8*)(c + 0x505) == 0) {
            func_02012694(0x12d, c + 0x74);
            *(u8*)(c + 0x505) = 0xf;
        }
    } else {
        if (*(u8*)(c + 0x505) == 0) {
            *(u8*)(c + 0x505) = 0x14;
            func_02012694(0x12f, c + 0x74);
        }
    }

    if (*(u16*)(c + 0x100) == 0)
        KingBobOmb_SetState(c, &data_ov078_021270cc);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02123eb8
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov078_02123eb8(int *t)
{
    daBombking_c *self = (daBombking_c *)t;
    self->mVertAccel = -0x2000;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&self->mBlendModelAnim, (void*)(data_ov078_02126f28[1]), 0, 0, 0x1000, 0);
    self->mStateTimer = 0x32;
    self->mHorzSpeed = 0xa000;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02123f1c
extern "C" int func_ov078_02123f1c(CView* c)
{
    if (func_ov078_02123804((char*)c) != 1) {
        if (c->field_494 == 0) goto L6c;
        if (_ZN6Player9GetHealthEv() != 0) goto L6c;
    }
    func_ov078_02123864((char*)c);
    if (c->field_494 != 0) {
        func_ov002_020db54c((char*)c->field_494, 0, 0x50000, c->field_8e);
        c->field_430 = c->field_494;
        c->field_494 = 0;
    }
    return 1;
L6c:
    if (_ZN9Animation8FinishedEv(&c->anim_31c)) {
        KingBobOmb_SetState(c, data_ov078_0212709c);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02123fb4
extern "C" {
int func_ov078_02123fb4(char *c){
  *(int*)(c+0x9c) = -0x2000;
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((void*)(c+0x2cc), (void*)(data_ov078_02126f18[1]), 0, 0x40000000, 0x1000, 0);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02124000
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov078_02124000(char* c){
  int ang = _ZN8dActor_c18HorzAngleToCPlayerEv(c);
  ApproachAngle((char*)c+0x94, ang, 1, 0x500, 0x500);
  *(short*)(c+0x8e)=*(short*)(c+0x94);
  if(_ZN9Animation8FinishedEv((char*)c+0x31c)){
    KingBobOmb_SetState(c, data_ov078_0212703c);
  }
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02124060
extern "C" {
int func_ov078_02124060(char *c){
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((void*)(c+0x2cc), (void*)(data_ov078_02126f00[1]), 0, 0x40000000, 0x1000, 0);
  return 1;
}
}

#pragma push
#pragma opt_propagation off
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_021240a0
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov078_021240a0(char* c)
{
    struct Vector3 v;
    short msg;
    int lim;

    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x110) != 0) {
        if (*(int*)(c + 0x500) <= 0) {
            if (*(int*)(c + 0x98) != 0) {
                *(int*)(c + 0x98) = 0;
                func_ov078_02125c24(c, 0x7d0000);
            }
            if ((unsigned short)(*(unsigned short*)((char*)*(int*)(c + 0x430) + 0x6ce) & 0x800) != 0)
                return 1;

            ApproachAngle(c + 0x94, _ZN8dActor_c18HorzAngleToCPlayerEv(c), 5, 0x1000, 0x200);
            *(short*)(c + 0x8e) = *(short*)(c + 0x94);
            if (AngleDiff(_ZN8dActor_c18HorzAngleToCPlayerEv(c), *(short*)(c + 0x8e)) < 0x1000) {
                char* pl = (char*)(long)*(int*)(c + 0x430);
                if (_ZN6Player9StartTalkER7fBase_cb(pl, c, 1) != 0) {
                    msg = 0;
                    if (data_0209f220[0] == 1) {
                        msg += (short)(*(int*)(pl + 8) + 0x9a);
                    } else {
                        msg = 0x95;
                    }
                    _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x14, 0x15666);
                    if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(pl, c, (int)msg, c + 0x5c, 0, 0) != 0) {
                        func_02012694(0x12a, c + 0x74);
                        KingBobOmb_SetState(c, &data_ov078_0212705c);
                    }
                }
            }
            return 1;
        }

        if (*(unsigned char*)(c + 0x499) == 0) {
            func_02012694(0x128, c + 0x74);
            func_ov078_02125c24(c, 0x7d0000);
            _ZN8dActor_c15HugeLandingDustEb(c, 1);
            *(unsigned char*)(c + 0x499) = 1;
        }
        if (*(unsigned short*)(c + 0x100) == 0) {
            *(int*)(c + 0x49c) = 0;
            KingBobOmb_SetState(c, &data_ov078_0212702c);
            return 1;
        }
        if (Vec3_Dist(c + 0x4d4, c + 0x5c) < 0x258000) {
            int r = RandomIntInternal(&data_0209e650);
            int d;
            r = ((unsigned)r >> 8) & 0xf;
            r <<= 0x1c;
            lim = 0x4000;
            d = r >> 0x10;
            r = -lim;
            if (d < r)
                d = r;
            else if (d > 0x4000)
                d = lim;
            {
                short *p = (short *)(c + 0x4fa);
                *p = (short)(*p + d);
            }
        } else {
            *(short*)(c + 0x4fa) = Vec3_HorzAngle(c + 0x5c, c + 0x4d4);
        }
        *(unsigned char*)(c + 0x499) = 0;
        *(int*)(c + 0xa8) = 0x1e000;
        *(int*)(c + 0x98) = 0xa000;
    } else {
        if (*(int*)(c + 0xa8) < 0) {
            unsigned int id = *(unsigned int*)(c + 0x360);
            if (id != 0) {
                char* a = (char*)_ZN8dActor_c10FindWithIDEj(id);
                if (a != 0) {
                    int b = (int)(*(unsigned short*)(a + 0xc) == 0xbf);
                    if (b != 0) {
                        *(struct M3*)&v = *(struct M3*)(a + 0x5c);
                        if (*(int*)(c + 0x60) > v.y)
                            _ZN6Player12Unk_020c6a10Ej(a, 1);
                    }
                }
            }
        }
    }

    *(short*)(c + 0x8e) = *(short*)(c + 0x94);
    ApproachAngle(c + 0x94, *(short*)(c + 0x4fa), 5, 0x1000, 0x200);
    return 1;
}
}

#pragma pop

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_021243c0
extern "C" {
int func_ov078_021243c0(char* c){
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((void*)(c+0x2cc), (void*)((void*)data_ov078_02126f20[1]), 0, 0, 0x1000, 0);
  *(short*)(c+0x100) = 0xc8;
  *(short*)(c+0x4fa) = Vec3_HorzAngle(c+0x5c, c+0x4d4);
  *(int*)(((long long)(int)(c + 0x500))) =
    *(int*)(((long long)(int)(c + 0x500))) - 1;
  *(char*)(c+0x499) = 0;
  *(int*)(c+0xa8) = 0x28000;
  *(int*)(c+0x98) = 0x5000;
  if (*(int*)(c+0x500) <= 0) *(int*)(c+0xb0) = 0x10000000;
  *(int*)(c+0x9c) = -0x2000;
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02124470
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov078_02124470(char* c)
{
    s16 ang = _ZN8dActor_c18HorzAngleToCPlayerEv(c);
    ApproachAngle(c + 0x94, ang, 1, 0x500, 0x500);
    *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
    if (*(u16*)(c + 0x100) == 0)
        KingBobOmb_SetState(c, &data_ov078_0212702c);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_021244d0
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov078_021244d0(char *c) {
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((void*)(c+0x2cc), (void*)((void*)data_ov078_02126f20[1]), 0, 0, 0x1000, 0);
    *(short*)(c+0x100) = 0x32;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02124520
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov078_02124520(char *c)
{
    Vector3 a;
    Vector3 look;
    Vector3 pos;
    Vector3 in;
    Vector3 out;
    Vector3 ppos;
    Vector3 tmp;
    Vector3 dust;
    void *cam;
    char *player;
    int *pp;
    unsigned int *flags;
    s16 ang;

    cam = data_0209f318;
    if (_ZNK9Animation12WillHitFrameEi(c + 0x31c, 0x46)) {
        func_ov078_02125c24(c, 0x7d0000);
        func_02012694(0x12c, c + 0x74);
        a.x = 0;
        a.y = 0;
        a.z = 0;
        data_020a0e68 = *(struct M12 *)(c + 0x2e8);
        MulMat4x3Mat4x3((char *)*(void **)(c + 0x2e0) + 0x120, &data_020a0e68, &data_020a0e68);
        a.x = data_020a0e68.w[9];
        a.y = data_020a0e68.w[10];
        a.z = data_020a0e68.w[11];
        Vec3_Lsl(&tmp, &a, 3);
        a.x = tmp.x;
        dust.x = tmp.x;
        a.y = tmp.y;
        dust.y = tmp.y;
        a.z = tmp.z;
        dust.z = tmp.z;
        _ZN8dActor_c17HugeLandingDustAtER7Vector3b(c, &dust, 1);
    }
    player = *(char **)(c + 0x430);
    if (_ZN6Player12GetTalkStateEv(player) != -1) {
        pp = (int *)(((int)player + 0x5c));
        in.x = 0;
        in.y = 0;
        in.z = 0;
        out.x = 0;
        out.y = 0;
        out.z = 0;
        ppos.x = pp[0];
        ppos.y = pp[1];
        ppos.z = pp[2];
        ang = Vec3_HorzAngle((Vector3 *)(c + 0x5c), &ppos);
        look.x = *(int *)(c + 0x5c);
        look.y = *(int *)(c + 0x60);
        look.z = *(int *)(c + 0x64);
        pos.x = *(int *)(c + 0x5c);
        pos.y = *(int *)(c + 0x60);
        pos.z = *(int *)(c + 0x64);
        look.y += 0x100000;
        in.z = 0x800000;
        Matrix4x3_FromRotationY(&data_020a0e68, ang);
        MulVec3Mat4x3(&in, &data_020a0e68, &out);
        pos.y += 0x100000;
        pos.x += out.x;
        pos.z += out.z;
        _ZN6Camera9SetLookAtERK7Vector3(cam, &look);
        _ZN6Camera6SetPosERK7Vector3(cam, &pos);
        _Z14ApproachLinearRsss((s16 *)(c + 0x8e), ang, 0x800);
        *(s16 *)(c + 0x94) = *(s16 *)(c + 0x8e);
        return 1;
    }
    *(unsigned int *)(((int)cam + 0x154)) &= ~8;
    if (_ZN9Animation8FinishedEv(c + 0x31c)) {
        KingBobOmb_SetState(c, &data_ov078_0212703c);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02124778
extern "C" {
int func_ov078_02124778(char *c){
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((void*)(c+0x2cc), (void*)(data_ov078_02126ef8[1]), 8, 0x40000000, 0x1000, 0);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_021247bc
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov078_021247bc(void *thiz)
{
    char *c = (char *)thiz;
    short horz;
    short vert;
    Vec3 vA;
    Vec3 vB;
    Vec3 vC;
    Vec3 D;
    Vec3 E;

    if (*(unsigned char *)(c + 0x499) != 2) {
        horz = Vec3_HorzAngle(c + 0x5c, c + 0x4d4);
        vert = Vec3_VertAngle(c + 0x5c, c + 0x4d4);
        Vec3_Dist(c + 0x5c, c + 0x4d4);
        vA.x = 0;
        vA.y = 0;
        vA.z = 0;
        ApproachAngle(c + 0x92, vert, 5, 0x1000, 0x300);
        ApproachAngle(c + 0x94, horz, 5, 0x1000, 0x300);
        *(short *)(c + 0x8e) = *(short *)(c + 0x94);
        if (_ZN9Animation8FinishedEv(c + 0x31c) == 0)
            return 1;
        vB.x = 0;
        vB.y = 0x3c000;
        vB.z = 0x28000;
        vC.x = 0;
        vC.y = 0;
        vC.z = 0;
        Matrix4x3_FromRotationY(&data_020a0e68, *(short *)(c + 0x94));
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(short *)(c + 0x92));
        MulVec3Mat4x3((Vector3*)&vB, &data_020a0e68, (Vector3*)&vC);
        *(int *)(c + 0xa4) = vC.x;
        if (*(unsigned char *)(c + 0x499) == 0) {
            *(int *)(c + 0xa8) = vC.y;
            if (*(int *)(c + 0xa8) > 0x3c000)
                *(int *)(c + 0xa8) = 0x3c000;
            else if (*(int *)(c + 0xa8) < 0x1e000)
                *(int *)(c + 0xa8) = 0x1e000;
        }
        *(int *)(c + 0xac) = vC.z;
        *(int *)(((int)c + 0xa8)) += *(int *)(c + 0x9c);
        vA.x = *(int *)(c + 0x5c);
        vA.y = *(int *)(c + 0x60);
        vA.z = *(int *)(c + 0x64);
        vA.y = *(int *)(c + 0x4d8);
        if (Vec3_Dist(&vA, c + 0x4d4) > 0x640000)
            return 1;
        *(unsigned char *)(c + 0x499) = 1;
    }

    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x110) != 0) {
        void *pl = _ZN8dActor_c13ClosestPlayerEv(c);
        if (pl != 0) {
            *(struct M3*)&D = *(struct M3*)((char *)pl + 0x5c);
            if (Vec3_Dist(c + 0x4d4, &D) > 0x640000 ||
                *(int *)(c + 0x4d8) - 0x64000 > D.y) {
                *(int *)(c + 0xa4) = 0;
                *(int *)(c + 0xa8) = 0;
                *(int *)(c + 0xac) = 0;
                KingBobOmb_SetState(c, &data_ov078_021270fc);
                return 1;
            }
        }

        if (*(unsigned char *)(c + 0x499) == 1) {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((void*)(c + 0x2cc), (void*)(*(void **)((char*)data_ov078_02126f08 + 4)), 0, 0x40000000, 0x1000, 0);
            *(int *)(c + 0x9c) = -0x2000;
            *(int *)(c + 0xb0) = 0x10000002;
            func_02012694(0x12c, c + 0x74);
            func_ov078_02125c24(c, 0xfa0000);
            func_0200fa8c(c, 1);
            *(unsigned char *)(c + 0x499) = 2;
            *(int *)(c + 0xa4) = 0;
            *(int *)(c + 0xa8) = 0;
            *(int *)(c + 0xac) = 0;
            return 1;
        }

        if (_ZN9Animation8FinishedEv(c + 0x31c) != 0) {
            char *pl2 = *(char **)(c + 0x430);
            unsigned short m;
            E.x = *(int *)(c + 0x5c);
            E.y = *(int *)(c + 0x60);
            E.z = *(int *)(c + 0x64);
            E.y += 0xc8000;
            m = *(unsigned short *)(pl2 + 0x6ce) & 0x800;
            if (m == 0) {
                if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(pl2, c, 0x94, &E, 0, 0) != 0) {
                    _ZN6Camera9SetFlag_3Ev(*(void **)&data_0209f318);
                    func_02012694(0x12a, c + 0x74);
                    KingBobOmb_SetState(c, &data_ov078_021270dc);
                }
            }
        }
    }

    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02124b40
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov078_02124b40(char *c)
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((void*)(c + 0x2cc), (void*)((void *)data_ov078_02126f10[1]), 0, 0x40000000, 0x1000, 0);
    func_02012694(0x129, c + 0x74);
    _ZN10dBgCh_Actr15ClearGroundFlagEv(c + 0x110);
    *(short *)(c + 0x92) = 0;
    *(int *)(c + 0x98) = 0;
    *(int *)(c + 0xa4) = 0;
    *(int *)(c + 0xa8) = 0;
    *(int *)(c + 0xac) = 0;
    *(unsigned char *)(c + 0x499) = 0;
    *(int *)(c + 0x9c) = -0x6000;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02124bc4
extern "C" {
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* thiz);
extern void func_0200fa8c(void* t, int a);
extern void func_02012694(int a, void* v);
extern int data_02092138;

int func_ov078_02124bc4(char* c)
{
    if (*(unsigned short*)(c + 0x100) != 0) return 1;
    if (*(int*)&data_02092138 > *(int*)(c + 0x60)) {
        KingBobOmb_SetState(c, data_ov078_021270bc);
        return 1;
    }
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x110) != 0) {
        if (*(unsigned char*)(c + 0x499) == 0) {
            func_ov078_02125c24(c, 0x7d0000);
            func_0200fa8c(c, 1);
            func_02012694(0x128, c + 0x74);
            *(short*)(c + 0x100) = 5;
            *(int*)(c + 0xa8) = 0x14000;
            *(int*)(c + 0x98) = 0xa000;
            *(unsigned char*)(c + 0x499) = 1;
        } else {
            KingBobOmb_SetState(c, data_ov078_021270bc);
        }
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02124c94
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov078_02124c94(char *p) {
    *(int*)(p+0x9c) = -0x2000;
    *(int*)(p+0xa8) = 0x1e000;
    *(int*)(p+0x98) = 0x14000;
    *(char*)(p+0x499) = 0;
    *(short*)(p+0x100) = 5;
    *(int*)(p+0xb0) = 0x10000000;
    func_ov078_02125c24(p, 0xfa0000);
    func_0200fa8c((int*)p, 1);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02124cf4
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" int func_ov078_02124cf4(unsigned char* thiz)
{
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(thiz + 0x110) == 0) goto done;
    *(int*)(thiz + 0x98) = 0;
    if ((int)(*(int*)(thiz + 0x4d8) - 0x28000) > *(int*)(thiz + 0x60)) {
        func_02012694(0x128, (int*)(thiz + 0x74));
        KingBobOmb_SetState(thiz, &data_ov078_021270ac);
        goto done;
    }
    if (thiz[0x499] == 0) {
        func_02012694(0x128, (int*)(thiz + 0x74));
        func_02012694(0x12b, (int*)(thiz + 0x74));
        func_ov078_02125c24((char*)thiz, 0x7d0000);
        func_0200fa8c((int*)thiz, 1);
        thiz[0x499] = 1;
        ((daBombking_c *)thiz)->mHealth -= 1;
    }
    if (*(int*)(thiz + 0x500) > 0) {
        KingBobOmb_SetState(thiz, &data_ov078_021270ec);
        goto done;
    }
    unsigned char* other = *(unsigned char**)(thiz + 0x430);
    if (other == 0) goto done;
    if ((unsigned short)(*(unsigned short*)(other + 0x6ce) & 0x800) != 0) goto done;
    if (_ZN6Player9StartTalkER7fBase_cb(other, (char*)thiz, 1) == 0) goto done;

    short msg = 0;
    _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x14, 0x15666);
    if (data_0209f220[0] == 1) {
        msg += (short)(*(int*)(other + 8) + 0x9a);
    } else {
        msg = 0x95;
    }

    if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(other, (char*)thiz, (unsigned)(int)msg, (Vector3*)(thiz + 0x5c), 0, 0) == 0) goto done;
    func_02012694(0x12a, (int*)(thiz + 0x74));
    KingBobOmb_SetState(thiz, &data_ov078_0212705c);
done:
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02124e9c
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov078_02124e9c(char *c)
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((void*)(c + 0x2cc), (void*)((void *)data_ov078_02126f20[1]), 0, 0, 0x1000, 0);
    if (*(int *)(c + 0x500) <= 0)
        *(int *)(c + 0xb0) = 0x10000000;
    _ZN10dBgCh_Actr15ClearGroundFlagEv(c + 0x110);
    *(unsigned char *)(c + 0x499) = 0;
    *(int *)(c + 0x9c) = -0x2000;
    *(int *)(c + 0xa8) = 0x28000;
    *(int *)(c + 0x98) = 0x14000;
    *(int *)(c + 0xa0) = -0x3c000;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02124f28
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern "C" {
extern int Vec3_Dist(const void* a, const void* b);
extern void _ZN6Player9DropActorEv(void* self);



int func_ov078_02124f28(unsigned char* c)
{
    if (c[0x499] == 0) {
        int b = (*(int*)(c + 0xb0) & 0x4000) != 0;
        if (b != 0) {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((void*)(c + 0x2cc), (void*)((void*)data_ov078_02126ee0[1]), 0, 0, 0x1000, 0);
            c[0x499] = 1;
        }
    }

    {
        unsigned char* obj = *(unsigned char**)(c + 0x494);
        if (obj == 0) {
            KingBobOmb_SetState(c, &data_ov078_0212708c);
            return 1;
        }
        {
            Vector3 copy = *(Vector3*)(obj + 0x5c);
            if (Vec3_Dist(c + 0x4d4, &copy) > 0x640000)
                goto drop;
            if (*(int*)(c + 0x4d8) - 0xa000 <= copy.y)
                goto flags;
        }
    }

drop:
    _ZN6Player9DropActorEv(*(void**)(c + 0x494));
    *(int*)(c + 0x494) = 0;
    KingBobOmb_SetState(c, &data_ov078_0212708c);
    return 1;

flags:
    {
        int flags = *(int*)(c + 0xb0);
        int b0 = (flags & 0x400) != 0;
        if (b0 == 0) {
            int b1 = (flags & 0x2000) != 0;
            if (b1 == 0) {
                int b2 = (flags & 0x100) != 0;
                if (b2 != 0)
                    goto done;
            }
        }
        if (b0 != 0) {
            *(short*)(c + 0x94) = *(short*)(*(unsigned char**)(c + 0x494) + 0x8e);
        }
        {
            short t = *(short*)(c + 0x94);
            *(short*)(c + 0x8e) = t;
        }
        {
            int* p354 = (int*)(c + 0x354);
            *p354 = *p354 & ~2;
        }
        KingBobOmb_SetState(c, &data_ov078_0212708c);
        {
            unsigned char* p = *(unsigned char**)(c + 0x494);
            if (p != 0) {
                if (*(int*)(p + 8) == 2) {
                    *(int*)(c + 0xa8) = 0x32000;
                    *(int*)(c + 0x98) = 0x1e000;
                }
            }
        }
        *(int*)(c + 0x494) = 0;
    }
done:
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_021250d0
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov078_021250d0(char *c)
{
    int *flags = (int *)(c + 0x354);
    *flags |= 2;
    *(int *)(c + 0x9c) = 0;
    *(int *)(c + 0x98) = 0;
    *(unsigned char *)(c + 0x499) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_021250f8
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov078_021250f8(char* c) {
    struct Vector3 in, out, v[2];
    void* target;
    void* player;

    if (func_ov078_02123804(c) == 1) {
        func_ov078_02123864((char*)c);
        return 1;
    }

    if (*(unsigned char*)(c + *(int*)(c + 0x49c) + 0x42c) == 0) {
        target = (char*)_ZN8dActor_c10FindWithIDEj(*(int*)(c + (*(int*)(c + 0x49c) << 2) + 0x424));
        if (target != 0) {
            *(int*)((char*)target + 0x5c) = *(int*)(c + 0x4ec);
            *(int*)((char*)target + 0x60) = *(int*)(c + 0x4f0);
            *(int*)((char*)target + 0x64) = *(int*)(c + 0x4f4);
            *(short*)((char*)target + 0x92) = *(short*)(c + 0x92);
            *(short*)((char*)target + 0x94) = *(short*)(c + 0x94);
            *(short*)((char*)target + 0x96) = *(short*)(c + 0x96);
            *(short*)((char*)target + 0x8c) = *(short*)(c + 0x8c);
            *(short*)((char*)target + 0x8e) = *(short*)(c + 0x8e);
            *(short*)((char*)target + 0x90) = *(short*)(c + 0x90);
            func_ov102_0214b384(target, 0x78);
            if (_ZNK9Animation12WillHitFrameEi(c + 0x31c, 0x13) != 0
                || func_ov078_02123804(c) == 1) {
                in.x = 0; in.y = 0; in.z = 0x28000;
                out.x = 0; out.y = 0; out.z = 0;
                v[0].x = 0; v[0].y = 0; v[0].z = 0;
                player = _ZN8dActor_c13ClosestPlayerEv(c);
                if (player != 0) {
                    int* q = (int*)(((int)player + 0x5c));
                    v[1].x = q[0];
                    v[1].y = q[1];
                    v[1].z = q[2];
                    v[0].x = v[1].x - *(int*)(c + 0x4ec);
                    v[0].y = v[1].y - *(int*)(c + 0x4f0);
                    v[0].z = v[1].z - *(int*)(c + 0x4f4);
                    Matrix4x3_FromRotationY(&data_020a0e68,
                        _ZN4cstd5atan2E5Fix12IiES1_(v[0].x, v[0].z));
                    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68,
                        (short)(-_ZN4cstd5atan2E5Fix12IiES1_(v[0].y, Vec3_HorzLen(&v[0]))));
                    out.y += 0x14000;
                    func_ov102_0214ad14(target);
                    *(int*)((char*)target + 0xa4) = out.x;
                    *(int*)((char*)target + 0xa8) = out.y;
                    *(int*)((char*)target + 0xac) = out.z;
                    *(int*)((char*)target + 0x98) = 0x14000;
                    *(int*)((char*)target + 0x9c) = -0x2000;
                    MulVec3Mat4x3(&in, &data_020a0e68, &out);
                }
                *(unsigned char*)(c + *(int*)(c + 0x49c) + 0x42c) = 1;
                *(int*)((char*)target + 0xc8) = 0;
            }
        }
    } else {
        if (func_ov078_02123804(c) == 1) {
            return 1;
        }
    }

    if (_ZN9Animation8FinishedEv(c + 0x31c) != 0) {
        player = _ZN8dActor_c13ClosestPlayerEv(c);
        if (player != 0) {
            if (*(int*)((char*)player + 8) != 3) {
                *(unsigned char*)(c + 0x504) = 0x64;
            }
        }
        KingBobOmb_SetState(c, &data_ov078_0212703c);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02125350
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov078_02125350(int sl)
{
    daBombking_c *self = (daBombking_c *)sl;
    int i;
    int r;

    self->mAnimSpeed = 1;
    self->mVertAccel = -0x2000;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&self->mBlendModelAnim, (void*)(data_ov078_02126ef0[1]), 0, 0x40000000, 0x1000, 0);

    self->mHorzSpeed = 0;

    for (i = 0; i < self->mPhase; i++) {
        if (self->mSpawnedId[i] == 0) {
            r = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                0xce, 2, (Vector3 *)&self->mThrowPosX, 0, self->mAreaId, -1);
            if (r != 0) {
                self->mSpawnedId[i] = *(int*)(r + 4);
                self->mSpawnSlot = i;
                return 1;
            }
        }
    }

    self->mActionStep = 0;
    return 1;
}
}

#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02125448
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov078_02125448(char* c)
{
    char* p;
    char* a;
    struct Vector3 v;
    int isType;
    int index;
    int empty;
    int z;
    unsigned int id;
    int v4a0;
    int slot;

    if (*(unsigned char*)(c + 0x505) == 0) {
        func_02012694(0x12d, c + 0x74);
        *(unsigned char*)(c + 0x505) = 0xf;
    }
    if (func_ov078_02123804(c) == 1) {
        func_ov078_02123864((char*)c);
        return 1;
    }

    p = _ZN8dActor_c13ClosestPlayerEv(c);
    if (p != 0) {
        *(struct M3*)&v = *(struct M3*)(p + 0x5c);
        if (Vec3_Dist(c + 0x4d4, &v) > 0x640000
            || (*(int*)(c + 0x4d8) - 0xa000) > v.y) {
            *(int*)(c + 0x98) = 0;
            KingBobOmb_SetState(c, data_ov078_021270fc);
            return 1;
        }

        ApproachAngle(c + 0x94, _ZN8dActor_c18HorzAngleToCPlayerEv(c), 0xa, 0x200, 0x100);
        {
            s16 ang = *(s16*)(c + 0x94);
            int* pf = (int*)(c + 0xb0);
            *(s16*)(c + 0x8e) = ang;
            *pf = *pf & ~0x80;
        }

        id = *(unsigned int*)(c + 0x3a0);
        if (id != 0) {
            a = (char*)_ZN8dActor_c10FindWithIDEj(id);
            if (a != 0) {
                isType = (int)(*(unsigned short*)(a + 0xc) == 0xbf);
                if (isType != 0) {
                    if (AngleDiff(_ZN8dActor_c18HorzAngleToCPlayerEv(c), *(short*)(c + 0x8e)) > 0x2800) {
                        if ((*(int*)(c + 0x39c) & 0x1000) != 0) {
                            {
                                int* pf = (int*)(c + 0xb0);
                                *pf = *pf | 0x80;
                            }
                            if (_ZN6Player7TryGrabER8dActor_c(a, c) != 0) {
                                *(int*)(c + 0x494) = (int)a;
                                *(int*)(c + 0x98) = 0;
                                KingBobOmb_SetState(c, data_ov078_0212707c);
                            }
                        }
                    } else if (func_ov002_020db5f4(a, c) != 0) {
                        *(int*)(c + 0x494) = (int)a;
                        *(int*)(c + 0x98) = 0;
                        KingBobOmb_SetState(c, data_ov078_0212706c);
                    }
                }
            }
            return 1;
        }

        if (*(unsigned char*)(c + 0x504) == 0) {
            if (data_0209f220[0] == 1) {
                if (_ZN8dActor_c13DistToCPlayerEv(c) < 0x3e8000) {
                    if (AngleDiff(_ZN8dActor_c18HorzAngleToCPlayerEv(c), *(short*)(c + 0x8e)) < 0x2800) {
                        index = 0;
                        empty = index;
                        z = index;
                        for (; index < 2; index++) {
                            slot = *(int*)(c + 0x424 + index * 4);
                            if (slot != 0) {
                                if (_ZN8dActor_c10FindWithIDEj((unsigned int)slot) == 0) {
                                    *(int*)(c + 0x424 + index * 4) = z;
                                    *(unsigned char*)(c + 0x42c + index) = (unsigned char)z;
                                }
                            }
                            if (*(int*)(c + 0x424 + index * 4) == 0) {
                                empty++;
                                if (empty == 2)
                                    *(int*)(c + 0x4a0) ^= 3;
                            }
                        }
                        v4a0 = *(int*)(c + 0x4a0);
                        if ((v4a0 == 2 && *(int*)(c + 0x428) == 0)
                            || (v4a0 == 1 && empty == 2)) {
                            KingBobOmb_SetState(c, data_ov078_0212704c);
                            return 1;
                        }
                    }
                }
            }
        }
    }
    return 1;
}
}

#pragma pop

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02125734
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov078_02125734(char *c) {
    *(int*)(c + 0x4fc) = 1;
    *(int*)(c + 0x98) = 0x5000;
    *(int*)(c + 0x9c) = -0x2000;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((void*)(c + 0x2cc), (void*)(data_ov078_02126ee8[1]), 8, 0, 0x1000, 0);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02125790
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" int func_ov078_02125790(char* self)
{
  Vector3 s;
  Vector3 d;
  Vector3 v;
  if (func_ov078_02123804(self) == 1) return 1;
  ApproachAngle((short*)(self + 0x94), _ZN8dActor_c18HorzAngleToCPlayerEv(self), 1, 0x500, 0x500);
  *(short*)(self + 0x8e) = *(short*)(self + 0x94);
  if (_ZNK9Animation12WillHitFrameEi(self + 0x31c, 0x46)) {
    func_ov078_02125c24(self, 0x7d0000);
    func_02012694(0x12c, self + 0x74);
    s.x = 0;
    s.y = 0;
    s.z = 0;
    data_020a0e68 = *(struct M12*)(self + 0x2e8);
    MulMat4x3Mat4x3((char*)*(void**)(self + 0x2e0) + 0x120, &data_020a0e68, &data_020a0e68);
    s.x = data_020a0e68.w[9];
    s.y = data_020a0e68.w[10];
    s.z = data_020a0e68.w[11];
    Vec3_Lsl(&d, &s, 3);
    s.x = d.x;
    v.x = d.x;
    s.y = d.y;
    v.y = d.y;
    s.z = d.z;
    v.z = d.z;
    _ZN8dActor_c17HugeLandingDustAtER7Vector3b(self, &v, 1);
  }
  if (_ZN9Animation8FinishedEv(self + 0x31c)) {
    KingBobOmb_SetState(self, &data_ov078_0212703c);
  }
  return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_021258e4
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov078_021258e4(int *t)
{
    *(int*)((char*)t+0x9c) = -0x2000;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((void*)((char*)t+0x2cc), (void*)(data_ov078_02126ef8[1]), 8, 0x40000000, 0x1000, 0);
    if(*(unsigned char*)((char*)t+0x506)==0){
        *(unsigned char*)((char*)t+0x506)=1;
        *(int*)((char*)t+0xb0)=0x10000002;
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02125950
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
void ApproachLinear(short &v, short t, short step);
extern "C" void _ZN7Message7EndTalkEv();

extern "C" int func_ov078_02125950(char *c)
{
    char *r5 = *(char **)(c + 0x430);
    int *src = (int *)(((int)(r5) + 0x5c));
    Vector3 v;
    int t = src[0];
    Vector3 *arg0 = (Vector3 *)(c + 0x5c);
    v.x = t;
    v.y = src[1];
    v.z = src[2];
    short ang = Vec3_HorzAngle(arg0, &v);
    ApproachLinear(*(short *)(c + 0x8e), ang, 0x800);
    *(short *)(c + 0x94) = *(short *)(c + 0x8e);
    if (_ZN6Player12GetTalkStateEv(r5) == -1) {
        _ZN7Message7EndTalkEv();
        func_02011d44();
        _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2d);
        KingBobOmb_SetState(c, &data_ov078_0212701c);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_021259e4
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov078_021259e4(void)
{
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_021259ec
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov078_021259ec(char* c)
{
    int dist;
    struct Vector3 ppos;
    struct Vector3 v;
    void* player;
    unsigned int b;

    if (*(unsigned char*)(c + 0x506) == 1) {
        unsigned char *p = (unsigned char*)(c + 0x50a);
        *p += 1;
        if (*(unsigned char*)(c + 0x50a) > 0xc8) {
            *(int*)(c + 0xb0) = 0x10000003;
            *(unsigned char*)(c + 0x506) = 0;
            *(unsigned char*)(c + 0x50a) = 0;
            *(int*)(c + 0x500) = 3;
            _ZN5Sound22StopLoadedMusic_Layer3Ev();
            func_02011cfc();
            *(unsigned char*)(c + 0x508) = 0;
        }
    }

    _Z14ApproachLinearRsss((short*)(c + 0x8e), *(short*)(c + 0x4f8), 0x800);
    *(short*)(c + 0x94) = *(short*)(c + 0x8e);

    if (func_ov078_02123804(c) == 1) {
        return 1;
    }

    player = _ZN8dActor_c13ClosestPlayerEv(c);
    if (player != 0) {
        *(struct M3*)&ppos = *(struct M3*)((char*)player + 0x5c);

        if (*(unsigned char*)(c + 0x508) == 0) {
            dist = Vec3_Dist((struct Vector3*)(c + 0x5c), &ppos);
            if (dist < 0x12c000) {
                *(void**)(c + 0x430) = player;
                v.x = *(int*)(c + 0x5c);
                v.y = *(int*)(c + 0x60);
                v.z = *(int*)(c + 0x64);
                v.y = v.y + 0xc8000;

                if (_ZN6Player9StartTalkER7fBase_cb(*(void**)(c + 0x430), c, 1)) {
                    _ZN7Message11PrepareTalkEv();
                    b = (data_0209f220[0] != 1) ? 0x93 : (unsigned int)(short)(*(int*)((char*)player + 8) + 0x96);
                    if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(player, c, b, &v, 0, 0)) {
                        func_02012694(0x12a, c + 0x74);
                        *(unsigned char*)(c + 0x508) = 1;
                        KingBobOmb_SetState(c, &data_ov078_0212700c);
                    }
                }
            }
        } else {
            dist = Vec3_Dist((struct Vector3*)(c + 0x5c), &ppos);
            if (dist < 0x258000) {
                if (*(int*)(c + 0x4d8) - 0xa000 < ppos.y) {
                    KingBobOmb_SetState(c, &data_ov078_0212701c);
                }
            }
        }
    }

    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02125bc8
extern "C" int func_ov078_02125bc8(char* c) {
    *(int*)(c+0x98) = 0;
    *(int*)(c+0x4fc) = 1;
    *(int*)(c+0x9c) = -0x2000;
    *(unsigned char*)(c+0x50a) = 0;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((void*)(c+0x2cc), (void*)((void*)data_ov078_02126f30[1]), 8, 0, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02125c24
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov078_02125c24(char* c, int strength) {
    func_0200d8c8(data_0209f318, c + 0x5c, strength);
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol KingBobOmb_SetState
extern "C" int KingBobOmb_SetState(void *cv, void *pv) {
    daBombking_c *c = (daBombking_c *)cv;
    PMF *p = (PMF *)pv;
    c->mState = p;
    PMF *q = c->mState;
    if (*q == 0) return 1;
    return (c->**q)();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02125c98
/* recovered: shared common types */
/* func_ov078_02125c98 at 0x02125c98 (ov078), size 0x148
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
extern "C" void func_ov078_02125c98(void* cv) {
  char* c = (char*)cv;
  int h = *(int*)(c+0x60);
  if (_ZNK10dBgCh_Actr10IsOnGroundEv(c+0x110) == 0) {
    dBgCh_Gnd rg;
    rg.SetObjAndPos(*(const Vector3*)(c+0x5c), 0);
    if (rg.DetectClsn() != 0)
      h = rg.clsnY;
  }
  int b = (*(int*)(c+0xb0) & 0x4000) != 0;
  if (b) {
    char* p = *(char**)(c+0x494);
    if (p != 0)
      h = *(int*)(p+0x60);
  }
  int ip = *(int*)(c+0x60) - h;
  if (ip <= 0x1000)
    ip = 0x1000;
  int r8 = 0x15e000 - (int)(((long long)ip * 0x180 + 0x800) >> 12);
  if (r8 < 0xa000)
    r8 = 0xa000;
  *(struct M12*)(c+0x434) = *(struct M12*)&IDENTITY_MATRIX4X3;   /* flat */
  *(int*)(c+0x458) = *(int*)(c+0x5c) >> 3;
  *(int*)(c+0x45c) = *(int*)(c+0x60) >> 3;
  *(int*)(c+0x460) = *(int*)(c+0x64) >> 3;
  _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
      c, c+0x3f8, c+0x434, r8, ip + 0x28000, 0xf);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02125de0
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov078_02125de0(char *c)
{
    struct Vector3 lv;
    int i;

    Matrix4x3_FromRotationY(c + 0x2e8, *(short*)(c + 0x8e));
    *(int*)(c + 0x30c) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x310) = *(int*)(c + 0x60) >> 3;
    *(int*)(c + 0x314) = *(int*)(c + 0x64) >> 3;
    MulMat4x3Mat4x3(*(char**)(c + 0x2e0) + 0x1e0, c + 0x2e8, c + 0x4a4);

    *(void**)(c + 0xc8) = c + 0x4a4;
    *(int*)(c + 0x4ec) = 0;
    *(int*)(c + 0x4f0) = 0;
    *(int*)(c + 0x4f4) = 0;
    *(int*)(c + 0x4ec) = *(int*)(c + 0x4c8);
    *(int*)(c + 0x4f0) = *(int*)(c + 0x4cc);
    *(int*)(c + 0x4f4) = *(int*)(c + 0x4d0);
    Vec3_Lsl(&lv, (struct Vector3*)(c + 0x4ec), 3);
    *(int*)(c + 0x4ec) = lv.x;
    *(int*)(c + 0x4f0) = lv.y;
    *(int*)(c + 0x4f4) = lv.z;

    for (i = 0; i < 2; i++) {
        unsigned int id;
        void *actor;

        if (((unsigned char*)(c + 0x42c))[i] != 0)
            continue;
        id = ((unsigned int*)(c + 0x424))[i];
        if (id == 0)
            continue;
        actor = (void*)_ZN8dActor_c10FindWithIDEj(id);
        if (actor == 0)
            continue;

        data_020a0e68 = *(struct M12*)(c + 0x4a4);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0xb000, -0x7000, -0x6000);
        Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, 0x4000, -0x1000, 0x2000);
        *(struct M12*)(c + 0x4a4) = data_020a0e68;
        *(void**)((char*)actor + 0xc8) = c + 0x4a4;
    }
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov078_02125f8c
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"


extern "C" {
extern struct Matrix4x3* _ZN8dActor_c11UpdateCarryER6PlayerRK7Vector3(void* self, void* player, struct Vector3* pos);
void func_ov078_02125f8c(void* c_){
    char* c = (char*)c_;
    void* player = *(void**)(c+0x494);
    int idx = 0;
    if(player == 0) return;
    if(*(int*)((char*)player+8) == 2) idx = 1;
    struct Matrix4x3* res = _ZN8dActor_c11UpdateCarryER6PlayerRK7Vector3(c, player, &data_ov078_0212711c[idx]);
    *(struct M12*)(c+0x2e8) = *(struct M12*)res;   /* flat: see include-union note */
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c16CleanupResourcesEv
int daBombking_c::CleanupResources()
{
    ((SharedFilePtr *)data_ov078_02126f38)->Release();
    ((SharedFilePtr *)data_ov078_02126f00)->Release();
    ((SharedFilePtr *)data_ov078_02126f20)->Release();
    ((SharedFilePtr *)data_ov078_02126f10)->Release();
    ((SharedFilePtr *)data_ov078_02126f08)->Release();
    ((SharedFilePtr *)data_ov078_02126f18)->Release();
    ((SharedFilePtr *)data_ov078_02126ee0)->Release();
    ((SharedFilePtr *)data_ov078_02126ef0)->Release();
    ((SharedFilePtr *)data_ov078_02126f40)->Release();
    ((SharedFilePtr *)data_ov078_02126f30)->Release();
    ((SharedFilePtr *)data_ov078_02126ee8)->Release();
    ((SharedFilePtr *)data_ov078_02126f28)->Release();
    ((SharedFilePtr *)data_ov078_02126ef8)->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c16OnPendingDestroyEv
void daBombking_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c6RenderEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daBombking_c::Render()
{
    void *r1 = mHeldActor;
    if (r1 != 0) {
        int r0 = mFlags;
        int flag = (r0 & 0x4000) ? 1 : 0;
        if (flag != 0) {
            if (*(int *)((char *)r1 + 0xc8) != 0) {
                func_ov078_02125f8c(this);
            }
        }
    }
    mBlendModelAnim.Model::Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */


extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
}

int daBombking_c::Behavior()
{
    char *self = (char *)this;

    if (DistToCPlayer() < 0x1770000) {
        *(daBombking_c **)((char *)data_0209f318 + 0x114) = this;
    }

    if (*(void **)((char *)mState + 8) != 0) {
        PMF *p = mState + 1;
        (this->**p)();
    }

    mBlendModelAnim.speed = mAnimSpeed << 0xc;
    mBlendModelAnim.UpdateVerts();
    mBlendModelAnim.Advance();

    if ((char *)mState == (char *)data_ov078_0212707c) {
        void *r1 = mHeldActor;
        int b;
        if (r1 != 0) {
            b = (mFlags & 0x4000) != 0;
            if (b != 0 && *(int *)((char *)r1 + 0xc8) != 0) {
                goto skip_de0;
            }
        }
        func_ov078_02125de0(self);
    skip_de0:
        func_ov078_02125c98(self);
        return 1;
    }

    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    DecIfAbove0_Byte(&unk_505);
    DecIfAbove0_Byte(&unk_504);

    if ((char *)mState != (char *)data_ov078_021270bc) {
        UpdatePos(&mdCcAcPos_c);
    } else {
        UpdatePosWithOnlySpeed(&mdCcAcPos_c);
    }

    if ((char *)mState != (char *)data_ov078_021270bc || mActionStep == 1) {
        UpdateWMClsn(mWithMeshClsn, 0);
    }

    if ((char *)mState == (char *)data_ov078_0212703c || (char *)mState == (char *)data_ov078_021270fc) {
        if (mWithMeshClsn.IsOnWall() != 0
            || mWithMeshClsn.IsOnGround() == 0
            || (mArenaPosY - 0x28000) > mPosY) {
            KingBobOmb_SetState(self, data_ov078_021270bc);
        }
    }

    {
        Vector3 v;
        v.x = data_ov078_02126e00.x;
        v.y = data_ov078_02126e00.y;
        v.z = data_ov078_02126e00.z;
        mdCcAcPos_c.SetPosRelativeToActor(v);
    }
    {
        Vector3 v;
        v.x = data_ov078_02126e00.x;
        v.y = data_ov078_02126e00.y;
        v.z = data_ov078_02126e00.z;
        mdCcAcPos_c2.SetPosRelativeToActor(v);
    }
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    mdCcAcPos_c2.Clear();
    mdCcAcPos_c2.Update();

    func_ov078_02125de0(self);
    func_ov078_02125c98(self);
    return 1;
}

#pragma push
#pragma opt_strength_reduction off
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* SharedFilePtr's complete declaration is included above. The file handles
   below still use their existing ROM-backed data declarations. */
int daBombking_c::InitResources()
{
    BMD_File *f;
    Vector3 v0;
    Vector3 v1;
    int i;
    f = (BMD_File *)Model::LoadFile(*(SharedFilePtr *)data_ov078_02126f38);
    mBlendModelAnim.SetFile(f, 1, 1);
    mShadowModel.InitCylinder();
    Animation::LoadFile(*(SharedFilePtr *)data_ov078_02126f00);
    Animation::LoadFile(*(SharedFilePtr *)data_ov078_02126f20);
    Animation::LoadFile(*(SharedFilePtr *)data_ov078_02126f10);
    Animation::LoadFile(*(SharedFilePtr *)data_ov078_02126f08);
    Animation::LoadFile(*(SharedFilePtr *)data_ov078_02126f18);
    Animation::LoadFile(*(SharedFilePtr *)data_ov078_02126ee0);
    Animation::LoadFile(*(SharedFilePtr *)data_ov078_02126ef0);
    Animation::LoadFile(*(SharedFilePtr *)data_ov078_02126f40);
    Animation::LoadFile(*(SharedFilePtr *)data_ov078_02126f30);
    Animation::LoadFile(*(SharedFilePtr *)data_ov078_02126ee8);
    Animation::LoadFile(*(SharedFilePtr *)data_ov078_02126f28);
    Animation::LoadFile(*(SharedFilePtr *)data_ov078_02126ef8);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    v0.x = data_ov078_02126e00.x;
    v0.y = data_ov078_02126e00.y;
    v0.z = data_ov078_02126e00.z;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &v0, 0x78000, 0xc8000, 0x200004, 0x206000);
    v1.x = data_ov078_02126e00.x;
    v1.y = data_ov078_02126e00.y;
    v1.z = data_ov078_02126e00.z;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c2, this, &v1, 0xc8000, 0xc8000, 0x200000, 0x207000);
    unk_498 = 0x1f;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mArenaPosX = 0xb1d000;
    mArenaPosY = 0x1060000;
    mArenaPosZ = 0xfee15000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x190000, 0x190000, 0, 0);
    func_02035550(&mWithMeshClsn);
    mAnimSpeed = 1;
    mHealth = 3;
    mStarID = param1 & 0xf;
    mStarTracked = TrackStar(mStarID, 2);
    {
    int z = 0;
    for (i = 0; i < 2; i++) {
        mSpawnedId[i] = z;
        mSpawnedThrown[i] = (unsigned char)z;
    }
    }
    mPhase = ((unsigned int)RandomIntInternal(&data_0209e650) >> 0x1e) & 1;
    mPhase = mPhase + 1;
    mInitAngleY = mAngleY;
    KingBobOmb_SetState(this, &data_ov078_0212710c);
    return 1;
}

#pragma pop

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBombking_c16OnAimedAtWithEggEv
// recovered name: KingBobOmb_OnAimedAtWithEgg
/* recovered: renamed to Class_Method */
/* daBombking_c::OnAimedAtWithEgg - recovered from vtable slot identity */
s32 daBombking_c::OnAimedAtWithEgg() {
    return 1024000;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* The factory immediately follows ordinal 50 and ends at the .init boundary. */
// @symbol daBombking_c_classInit
/* The registry factory behind the BOMBKING profile. `return new daBombking_c()`
 * MATCHES (size 0x64); the synthesized ctor stores `_ZTV12daBombking_c + 2`. */
extern "C" daBombking_c *daBombking_c_classInit(void)
{
    return new daBombking_c();
}

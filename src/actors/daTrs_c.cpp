//cpp
/* Production translation unit for ov063/daTrs_c.
 * 97 function(s), .text 0x02115ee0..0x0211c600: the Boo (TERESA), the Big Boo
 * (BOSS_TERESA), the Big Boo's balcony basket (T_BASKET) and the Boo radar
 * icon (ICON_TERESA).
 *
 * One object: its .data holds _ZTS7daTrs_c (0x0211e720) through
 * _ZTV11daTBasket_c (0x0211e930), all three classes' RTTI and vtables with the
 * four registry profiles between them, and __sinit_ov063_0211e29c constructs
 * the SharedFilePtrs this file loads and releases. That sinit keeps its own
 * source because a text-only promotion cannot own .init.
 *
 * The three out-of-line destructors are the key functions, so this TU emits
 * the vtables. `#pragma defer_codegen off` lays .text down in source order
 * (this file is ROM-ascending) and makes each destructor come out D1, D0, then
 * a D2 the cartridge has no home for.
 *
 * The file-local helpers keep C linkage and raw member offsets: they were
 * matched as standalone C bodies, and their ROM symbols are the bare names.
 * Their declarations are gathered here once; the few call sites where a
 * shard's own declaration disagreed with the definition now cast explicitly.
 */

#pragma defer_codegen off

#include "common.h"
#include "types.h"
#include "dActor_c.h"
#include "daTrs_c.h"
#include "daTBasket_c.h"
#include "daTrsIcon_c.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "Sound.h"
#include "dBgCh_Gnd.h"

bool ApproachLinear(short &value, short target, short step);

extern "C" {
/* shared engine entry points without a header declaration */
void LoadKeyModels(int n);
void UnloadKeyModels(int n);
void LoadBlueCoinModel(void *self);
void UnloadBlueCoinModel(void *o);
unsigned NumStars(void);
int IsStarCollectedInCurLevel(int n);
void func_02035800(void *self);
void Matrix4x3_FromTranslation(void *m, s32 x, s32 y, s32 z);
void MulMat4x3Mat4x3(void *a, void *b, void *dst);
void SubVec3(void *a, void *b, void *dst);
void func_0200f760(void *thiz, void *cyl);
int func_0201267c(u32 a, const void *b);
u8 IsAreaShowing(s8 idx);
s16 Vec3_HorzAngle(const void *a, const void *b, ...);
s32 Vec3_HorzDist(const void *a, const void *b);
u16 DecIfAbove0_Short(void *p);

/* ABI seams whose header member forms do not reproduce the ROM (see notes) */
int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int a, int speed, unsigned int d);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *self, void *actor, void *pos, int r, int h, unsigned int e, unsigned int g);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, dActor_c *actor, Fix12i radius, Fix12i height, unsigned int flags, unsigned int vulnFlags);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, dActor_c *actor, Fix12i radius, Fix12i height, Vector3_16 *a, Vector3_16 *b);
int _ZN11dCapEnemy_c11GetCapStateEv(char *c);
unsigned int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int uniqueID, unsigned int effectID,
    int x, int y, int z, const void *dir, void *callback);

extern signed char data_0209f2f8;
extern unsigned char data_0209f264;
/* Per-variant tables indexed by unk_5cf: death mode, hurt damage. */
extern int data_ov063_0211e22c[];
extern int data_ov063_0211e1ec[];
extern Matrix4x3 data_020a0e68;
}

extern SharedFilePtr data_ov063_0211edc4;
extern SharedFilePtr data_ov063_0211edcc;
extern SharedFilePtr data_ov063_0211edd4;
extern SharedFilePtr data_ov063_0211eddc;
extern SharedFilePtr data_ov063_0211ede4;
extern SharedFilePtr data_ov063_0211edec;
extern SharedFilePtr data_ov063_0211edf4;
extern SharedFilePtr data_ov002_0210d9c8;
extern SharedFilePtr data_ov002_0210d9f8;
extern SharedFilePtr data_ov002_0210d9b8;

/* File-local helpers and engine entry points, with the C-linkage spellings
 * these bodies were matched against. */
typedef struct Vec3 { int x, y, z; } Vec3;
typedef struct Vec3_16 { s16 x, y, z; } Vec3_16;
extern "C" {
void Matrix4x3_ApplyInPlaceToRotationY(void *m, s16 angY);
int RandomIntInternal(int *seed);
void Vec3_Asr(void *d, const void *s, int sh);
void _Z14ApproachLinearRiii(int *p, int target, int step);
char *_ZN11dCapEnemy_c15RespawnIfHasCapEv(void *self);
void _ZN15ModelComponents21UpdateVertsUsingBonesEv(void *self);
void _ZN5Model12SetPolygonIDEi(void *self, int id);
int _ZN5Sound15PlaySecretSoundEP8dActor_cPt(void *actor, void *p);
int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, s32 d, int e);
void _ZN6Camera9SetFlag_3Ev(void *cam);
int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void *pl, void *a, unsigned m, void *v, unsigned d, unsigned e);
int _ZN6Player12GetTalkStateEv(void *pl);
int _ZN6Player9StartTalkER7fBase_cb(void *pl, void *a, int b);
char *_ZN8dActor_c10FindWithIDEj(unsigned int id);
void *_ZN8dActor_c13ClosestPlayerEv(void *self);
char *_ZN8dActor_c15FindWithActorIDEjPS_(unsigned int id, void *p);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *self, void *sm, void *m, int rad, int h, u32 a);
void _ZN8dActor_c24KillAndTrackInDeathTableEv(void *self);
char *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, const void *pos, const void *rot, int e, int f);
int func_020092c4(void *cam, void *out, void *target);
int func_02012694(int a, void *p, ...);
u16 func_0201277c(int a);
extern s16 data_02082214[];
extern char *data_0209f318;
void func_ov063_021160d4(char *c);
int func_ov063_02116190(char* self);
void func_ov063_02116244(char *c);
void func_ov063_021162c8(char *self);
unsigned char func_ov063_021163d0(char *c);
void func_ov063_0211640c(char *c);
void func_ov063_021166ac(char *c);
void func_ov063_021169c4(char *c);
void func_ov063_02116a1c(void *cc);
void func_ov063_02116bf0(char *c);
void func_ov063_02116bf4(char* c);
void func_ov063_02116d38(char* c);
void func_ov063_02116d98(void *c);
void func_ov063_02116dbc(void* c);
void func_ov063_02116df0(void *c);
void func_ov063_02116e14(char* c);
int func_ov063_02116f48(char *c);
void func_ov063_02116fac(char* c);
void func_ov063_021172a8(void* thiz);
void func_ov063_02117364(void* c);
void func_ov063_02117650(char *self);
void func_ov063_0211776c(char* c);
void func_ov063_021177b0(char* c);
void func_ov063_02117b0c(char *c);
void func_ov063_02117cdc(u8 *arg0);
void func_ov063_02118458(void* self);
void func_ov063_0211873c(char* self);
void func_ov063_02118914(char *c);
void func_ov063_021189f4(char *c);
int func_ov063_02118b2c(char *c);
void func_ov063_02118b98(char *c);
void func_ov063_02118cd8(char *self);
void func_ov063_02118ddc(char* c);
void func_ov063_02118e5c(void *c);
void func_ov063_02118ea0(char *c);
void func_ov063_02118eac(char *c);
void func_ov063_02118f24(void *c, void *vec);
void func_ov063_02118f50(void* c);
void func_ov063_02118f74(char *c);
void func_ov063_02119074(char *self);
void func_ov063_02119274(char* c);
void func_ov063_021192d4(char *c);
void func_ov063_0211934c(char *c);
void func_ov063_0211975c(char* self);
void func_ov063_02119870(void* c);
void func_ov063_02119894(char *c);
void func_ov063_02119960(char *c);
void func_ov063_02119a2c(void* c);
void func_ov063_02119a50(char *c);
void func_ov063_02119ab0(char *self);
void func_ov063_02119b1c(char *c);
void func_ov063_02119b84(char *c);
void func_ov063_02119bb0(char *c);
void func_ov063_02119c18(void* c, unsigned int id);
void func_ov063_02119c58(char *c);
void func_ov063_02119cc0(char *c, int unused, s16 a2, int a3);
void func_ov063_02119e38(char *thiz, int a1, short a2, int a3);
void func_ov063_0211a030(struct C* c, int a, int b);
int func_ov063_0211a0a8(int a0, int a1, int a2, int a3, int a4);
int func_ov063_0211a0dc(char* c);
int func_ov063_0211a3d0(char* c);
int func_ov063_0211a564(char *c, int arg1);
int func_ov063_0211a634(char *thiz, int arg);
void func_ov063_0211a6f0(char *c);
void func_ov063_0211a718(char* o);
void func_ov063_0211a76c(char* c, int cond, int val);
void func_ov063_0211a810(char *r0, int cond);
int func_ov063_0211a8a4(char *thiz);
void func_ov063_0211a960(char *c);
void func_ov063_0211a964(char *c, int arg1);
void func_ov063_0211aa34(char* self);
void func_ov063_0211ab68(char* obj);
int func_ov063_0211ad00(char *c);
int func_ov063_0211adb4(char *c);
void func_ov063_0211adfc(char *p);
extern void Vec3_MulScalarInPlace(void *v, int s);
extern void _ZN9ModelBase12ApplyOpacityEjj(void *self, unsigned int opacity, unsigned int unused);
extern void func_020167a4(void *p);
void Matrix4x3_FromRotationY(Matrix4x3 *m, s16 angle);
void _ZN11dCapEnemy_c12UpdateCapPosERK7Vector3RK10Vector3_16(void *self, const void *pos, const void *rot);
extern int data_0209e650;
extern int Vec3_Dist(const void* a, const void* b);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned a, int fx);
extern void _ZN5Sound22StopLoadedMusic_Layer3Ev(void);
extern void func_02011cfc(void);
extern s16 data_ov063_0211e1c8[];
extern int LenVec3(int *v);
extern void _ZN17daObjSlIceBlock_c16CleanupResourcesEv(void);
extern int func_ov002_020c51d0(void *c, int *st);
extern void _ZN5Sound22LoadAndSetMusic_Layer3Ej(u32 x);
extern u16 func_02011d14(void);
extern int data_ov008_02111b6c;
extern int data_0209caa0[];
extern s16 data_ov063_0211e1dc[];
extern s16 data_ov063_0211e1e4[];
extern int _ZN9Animation8FinishedEv(void* self);
extern int _ZNK9Animation12WillHitFrameEi(void* self, int f);
extern char* _ZN8dActor_c13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(void* self, const void * v, const void* p, s32 a, s32 b, unsigned int n);
extern s16 data_ov063_0211e1c0[];
extern void _ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h(void *self, void *a, unsigned int b, void *c, unsigned int d);
extern int data_ov063_0211e1d0[];
extern int data_0209b490[];
extern u8 data_0209d660;
extern int _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(void* self, void* clsn, void* player);
extern void _ZN6Player6BounceE5Fix12IiE(void* p, s32 f);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, const void * v, u32 a, s32 f, u32 b, u32 c, u32 d);
extern void _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(void* c, void* v, void* r4, s32 flag);
int _ZNK10dBgCh_Actr8IsOnWallEv(void* self);
void _ZN8dActor_c10PoofDustAtERK7Vector3(void* self, Vec3 *v);
void _ZN8dActor_c8PoofDustEv(void* self);
extern short data_ov063_0211e7e0[];
extern int data_ov063_0211edc0;
extern struct Vec3 data_ov063_0211ee74;
extern struct Vec3 data_ov063_0211ee80;
extern struct Vec3 data_ov063_0211ee8c;
extern void* data_ov063_0211ee20;
extern void* data_ov063_0211edfc;
extern void* data_ov063_0211ee08;
extern void *_ZN7Vector3D1Ev(void *object);
extern void func_020731dc(void *object, void *destructor, void **node);
extern void Vec3_Add(void * out, void * a, void * b);
}

inline int inline_fn(int arg0) { return data_020a0e68.m[arg0]; }
struct Flags { unsigned short bit0 : 1; };
struct Frame { Vec3 v; int pad[10]; };
typedef struct { u8 pad0[0xbe]; u16 fbe; u16 fc0; u16 fc2; u16 fc4; u8 pad1[0xe]; u16 lo8 : 8; u16 flag : 1; u16 hi7 : 7; } Sub364;
typedef struct { u16 lo8 : 8; u16 flag : 1; u16 hi7 : 7; } FlagW;
typedef struct { unsigned char pad[0xd4]; unsigned short lo8 : 8; unsigned short flag : 1; unsigned short hi7 : 7; } Sub500;
struct Sub { char pad[0x98]; int f98; };
struct C { char pad[0x98]; int f98; char pad2[0x484-0x98-4]; struct Sub* p484; };

// @symbol _ZN7daTrs_cD1Ev
// @symbol _ZN7daTrs_cD0Ev
daTrs_c::~daTrs_c()
{
}

// @symbol _ZN11daTBasket_cD1Ev
// @symbol _ZN11daTBasket_cD0Ev
daTBasket_c::~daTBasket_c()
{
}

// @symbol _ZN11daTrsIcon_cD1Ev
// @symbol _ZN11daTrsIcon_cD0Ev
daTrsIcon_c::~daTrsIcon_c()
{
}

// @symbol _ZN7daTrs_c16OnAimedAtWithEggEv
int daTrs_c::OnAimedAtWithEgg() {
    return mdCcAcPos_c.height / 2;
}

// @symbol func_ov063_021160d4
extern "C" void func_ov063_021160d4(char *c)
{
  int *p;
  *((int *) (c + 0x534)) = 0;
  *((int *) (c + 0x538)) = 0;
  *((int *) (c + 0x53c)) = 0;
  Matrix4x3_FromTranslation(&data_020a0e68, *((int *) (c + 0x5c)), *((int *) (c + 0x60)), *((int *) (c + 0x64)));
  MulMat4x3Mat4x3(*((Matrix4x3 **) (c + 0x394)), &data_020a0e68, &data_020a0e68);
  *((int *) (c + 0x534)) = data_020a0e68.m[9];
  *((int *) (c + 0x538)) = inline_fn(10);
  *((int *) (c + 0x53c)) = inline_fn(11);
  SubVec3((Vec3 *) (c + 0x534), (Vec3 *) (c + 0x5c), (Vec3 *) (c + 0x534));
  SubVec3((Vec3 *) (c + 0x534), (Vec3 *) (c + 0x540), (Vec3 *) (c + 0x534));
  Vec3_MulScalarInPlace((Vec3 *) (c + 0x534), 0x6800);
  p = (int *) (((int) c + 0x53c));
  *p += *((int *) (c + 0x598));
}

// @symbol func_ov063_02116190
extern "C" int func_ov063_02116190(char* self)
{
    int vec[3];
    void* p;

    if (((struct Flags*)(self + 0x5d4))->bit0) {
        p = _ZN8dActor_c13ClosestPlayerEv(self);
        if (p != 0 && *(int*)((char*)p + 0x64) >= 0x3e8000)
            return 1;
    } else if (*(int*)(self + 0x60) <= (int)0xff768000) {
        p = _ZN8dActor_c13ClosestPlayerEv(self);
        vec[0] = -0xbe000;
        vec[1] = (int)0xff66e000;
        vec[2] = 0xbe000;
        if (Vec3_HorzDist(vec, (char*)p + 0x5c) >= 0x73a000)
            return 1;
    }
    return 0;
}

// @symbol func_ov063_02116244
extern "C" void func_ov063_02116244(char *c) {
    char *r;
    if (*(int*)(c + 0x180) < 5) return;
    if (*(int*)(c + 0x49c) != 0) return;
    *(char**)(c + 0x48c) = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
        0xd3, *(unsigned int*)(c + 8), c + 0x5c, 0,
        *(signed char*)(c + 0x500 + 0xd0), -1);
    r = *(char**)(c + 0x48c);
    if (r != 0) *(int*)(c + 0x49c) = *(int*)(r + 4);
    *(char**)(c + 0x48c) = 0;
}

// @symbol func_ov063_021162c8
extern "C" void func_ov063_021162c8(char *self)
{
    if ((unsigned int)(*(unsigned short*)(self + 0x5d4) << 0x19) >> 0x1f) {
        *(unsigned char*)(self + 0x5cc) = 4;
    } else {
        *(unsigned char*)(self + 0x5cc) = 3;
        Vector3 v;
        v.x = *(int*)(self + 0x564);
        v.y = *(int*)(self + 0x568);
        v.z = *(int*)(self + 0x56c);
        ((dCapEnemy_c*)self)->ReleaseCap(*(Vector3*)&v);
        if ((unsigned int)(*(unsigned short*)(self + 0x5d4) << 0x1e) >> 0x1f) {
            unsigned int flags = 2;
            if (*(unsigned short*)(self + 0x4a0) == 0x121) flags |= 0x10;
            dActor_c *a = dActor_c::Spawn(*(unsigned short*)(self + 0x4a0), flags,
                                    *(Vector3*)(self + 0x504), 0,
                                    *(signed char*)(self + 0x5d0), -1);
            if (a != 0) {
                *(unsigned char*)((char*)a + 0x3aa) = 0xa;
                if (*(unsigned short*)(self + 0x4a0) == 0x121)
                    *(unsigned short*)((char*)a + 0x3a8) = 0;
            }
            unsigned short *ip = (unsigned short *)(self + 0x5d4);
            *ip = (unsigned short)(*ip & ~2);
        }
    }
    func_0201267c(0x14a, self + 0x74);
}

// @symbol func_ov063_021163d0
extern "C" unsigned char func_ov063_021163d0(char *c){
    unsigned int id = *(unsigned int*)(c+0x498);
    if(id==0) return 0;
    char *a = (char*)(unsigned int)_ZN8dActor_c10FindWithIDEj(id);
    if(a==0) return 0;
    return *(unsigned char*)(a+0x153);
}

/* 0211640c needs opt_propagation off: the ROM keeps the s16 -1 in a register
 * and multiplies (smulbb) where propagation would fold it to rsb. Measured in
 * this file, the setting binds one definition late: a bracket that closes
 * right after 0211640c leaves it unmatched, so the bracket also spans the
 * propagation-insensitive 021166ac. Deleting it unmatches 0211640c alone. */
#pragma push
#pragma opt_propagation off
// @symbol func_ov063_0211640c
extern "C" void func_ov063_0211640c(char *c)
{
    struct Vector3 pos, t1, t2;
    s16 ang;

    if (((u32)(*(u16 *)(c + 0x5d4) << 0x1c) >> 0x1f) == 0)
        return;

    pos.x = *(s32 *)(c + 0x5c);
    pos.y = *(s32 *)(c + 0x60);
    pos.z = *(s32 *)(c + 0x64);
    ang = *(s16 *)(c + 0x8e);

    if ((u32)(*(u16 *)(c + 0x5d4) << 0x17) >> 0x1f) {
        s16 neg = -1;
        s16 a = ang;
        pos.x = pos.x * (int)neg;
        ang = (s16)(a * neg);
    }

    if (*(u8 *)(c + 0x5cc) == 3) {
        Vec3_Asr(&t1, &pos, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, t1.x, t1.y, t1.z);
        *(struct Matrix4x3 *)(c + 0x39c) = data_020a0e68;
        _ZN9ModelBase12ApplyOpacityEjj(c + 0x380, (u8)((int)*(u8 *)(c + 0x5c8) >> 3), 1);
        func_020167a4(c + 0x380);
        {
            char *m = *(char **)(c + 0x390);
            *(s16 *)(m + 0x1a) = *(s16 *)(c + 0x8c);
            *(s16 *)(m + 0x1c) = (s16)(ang - 0x4000);
            *(s16 *)(m + 0x1e) = *(s16 *)(c + 0x90);
        }
        _ZN15ModelComponents21UpdateVertsUsingBonesEv(c + 0x388);
    } else {
        Vec3_Asr(&t2, &pos, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, t2.x, t2.y, t2.z);
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, ang);
        *(struct Matrix4x3 *)(c + 0x39c) = data_020a0e68;
        _ZN9ModelBase12ApplyOpacityEjj(c + 0x380, (u8)((int)*(u8 *)(c + 0x5c8) >> 3), 1);
    }

    if (*(u8 *)(c + 0x5c8) >= 0x10) {
        Matrix4x3_FromTranslation(&data_020a0e68,
            pos.x >> 3, pos.y >> 3, pos.z >> 3);
        *(struct Matrix4x3 *)(c + 0x4a4) = data_020a0e68;
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, c + 0x434, c + 0x4a4, 0x12c000, 0xc8000, 0xf);

        if ((u32)(*(u16 *)(c + 0x5d4) << 0x17) >> 0x1f) {
            int px = pos.x;
            int neg = -1;
            pos.x = px * neg;
            Matrix4x3_FromTranslation(&data_020a0e68,
                pos.x >> 3, pos.y >> 3, pos.z >> 3);
            *(struct Matrix4x3 *)(c + 0x4d4) = data_020a0e68;
            _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
                c, c + 0x45c, c + 0x4d4, 0x12c000, 0xc8000, 0xf);
        }
    }

    func_ov063_021160d4(c);
}

// @symbol func_ov063_021166ac
extern "C" void func_ov063_021166ac(char *c)
{
    daTrs_c *t = (daTrs_c *)c;
    Vector3_16 rot;
    Vector3 t1;
    Vector3 t2;
    Vector3 pos;

    if (t->unk_5cf == 0xf) {
        func_ov063_0211640c(c);
        return;
    }
    if (!t->mFlags_5d4.b3)
        return;
    if (t->mFlags_5d4.b1) {
        if (*(u16 *)(c + 0x4a0) != 0xd4) {
            s16 *ap = (s16 *)(c + 0x5ba);
            *ap += 0xc00;
        }
        Matrix4x3_FromRotationY((Matrix4x3 *)(c + 0x400), *(s16 *)(c + 0x5ba));
        *(s32 *)(c + 0x424) = *(s32 *)(c + 0x504) >> 3;
        *(s32 *)(c + 0x428) = *(s32 *)(c + 0x508) >> 3;
        *(s32 *)(c + 0x42c) = *(s32 *)(c + 0x50c) >> 3;
    }

    if (t->unk_5cc == 3 || t->unk_5cc == 3 ||
        t->unk_5cc == 3 || t->unk_5cc == 3) {
        Vec3_Asr(&t1, (Vector3 *)(c + 0x5c), 3);
        Matrix4x3_FromTranslation(&data_020a0e68, t1.x, t1.y, t1.z);
        *(Matrix4x3 *)(c + 0x39c) = data_020a0e68;
        _ZN9ModelBase12ApplyOpacityEjj(c + 0x380, (u8)(*(u8 *)(c + 0x5c8) >> 3), 1);
        func_020167a4(c + 0x380);
        {
            char *p = *(char **)(c + 0x390);
            *(s16 *)(p + 0x1a) = *(s16 *)(c + 0x8c);
            *(s16 *)(p + 0x1c) = *(s16 *)(c + 0x8e) - 0x4000;
            *(s16 *)(p + 0x1e) = *(s16 *)(c + 0x90);
        }
        _ZN15ModelComponents21UpdateVertsUsingBonesEv(c + 0x388);
    } else {
        Vec3_Asr(&t2, (Vector3 *)(c + 0x5c), 3);
        Matrix4x3_FromTranslation(&data_020a0e68, t2.x, t2.y, t2.z);
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(s16 *)(c + 0x8e));
        *(Matrix4x3 *)(c + 0x39c) = data_020a0e68;
        _ZN9ModelBase12ApplyOpacityEjj(c + 0x380, (u8)(*(u8 *)(c + 0x5c8) >> 3), 1);
    }

    Matrix4x3_FromTranslation(&data_020a0e68, *(s32 *)(c + 0x5c) >> 3, *(s32 *)(c + 0x60) >> 3, *(s32 *)(c + 0x64) >> 3);
    *(Matrix4x3 *)(c + 0x4a4) = data_020a0e68;
    {
        int big = (*(u16 *)(c + 0xc) == 0xd2);
        if (big)
            _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0x434, c + 0x4a4, 0x12c000, 0xc8000, 0xf);
        else
            _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0x434, c + 0x4a4, 0x64000, 0xc8000, 0xf);
    }
    *(s32 *)(c + 0x568) = *(s32 *)(c + 0x538) + (int)(((s64)*(s32 *)(c + 0x80) * 0x60000 + 0x800) >> 12);
    pos.x = *(s32 *)(c + 0x564);
    pos.y = *(s32 *)(c + 0x568);
    pos.z = *(s32 *)(c + 0x56c);
    {
        /* The angle triple copies as two halfword loads, then two stores, then
           the third pair: two named u16 temps for x and y reproduce that; a
           Vector3_16 struct copy or three direct member copies alternate
           load/store (and the s16-typed copy sign-extends, ldrsh). */
        u16 ax = *(u16 *)(c + 0x8c);
        u16 ay = *(u16 *)(c + 0x8e);
        rot.y = ay;
        rot.x = ax;
        rot.z = *(u16 *)(c + 0x90);
    }
    /* equal-arm ternary: the rot address (r2) is set up before pos (r1), as in the ROM */
    _ZN11dCapEnemy_c12UpdateCapPosERK7Vector3RK10Vector3_16(c, &pos, c ? &rot : &rot);
    func_ov063_021160d4(c);
}

#pragma pop
// @symbol func_ov063_021169c4
extern "C" void func_ov063_021169c4(char *c) {
    Matrix4x3_FromTranslation(c+0x31c, *(int*)(c+0x5c)>>3, *(int*)(c+0x60)>>3, *(int*)(c+0x64)>>3);
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c+0x350, c+0x31c, 0x64000, 0x64000, 0xf);
}

// @symbol func_ov063_02116a1c
extern "C" void func_ov063_02116a1c(void *cc)
{
    char *c = (char *)cc;
    Vec3 v;
    int a;
    int scale;

    v.x = *(int *)(c + 0x51c);
    v.y = *(int *)(c + 0x520);
    v.z = *(int *)(c + 0x524);
    *(int *)(c + 0x584) = 0x2000;

    a = *(unsigned char *)(c + 0x5cc);
    if (a == 0) {
        *(unsigned short *)((int)c + 0x5d4) &= ~8;
        if (NumStars() < 0xf) {
            ((fBase_c *)c)->MarkForDestruction();
            return;
        }
        if (((unsigned)(*(unsigned short *)(c + 0x5d4)) << 0x1b) >> 0x1f) {
            unsigned short *fp = (unsigned short *)(c + 0x5d4);
            unsigned char *st = (unsigned char *)(c + 0x5cc);
            *fp |= 8;
            *(unsigned char *)(c + 0x5c8) = 0xb4;
            scale = *(int *)(c + 0x584);
            *(int *)(c + 0x80) = scale;
            *(int *)(c + 0x84) = scale;
            *(int *)(c + 0x88) = scale;
            *(int *)(c + 0x188) = *(int *)(c + 0x590) * *(int *)(c + 0x584);
            *(int *)(c + 0x18c) = *(int *)(c + 0x594) * *(int *)(c + 0x584);
            *st += 1;
        }
    } else if (a == 1) {
        if (*(int *)(c + 0x580) < 0x3e8000) {
            unsigned char *st = (unsigned char *)(c + 0x5cc);
            *st += 1;
            func_0201267c(0xf8, c + 0x74);
        }
        *(int *)(c + 0x98) = 0;
    } else {
        int t;
        _Z14ApproachLinearRiii((int *)(c + 0x98), 0x30000, 0x1800);
        t = 0x3e8000;
        v.x = -t;
        v.z = (int)0xfdcd8000;
        if (*(int *)(c + 0x64) < (int)0xfec78000) {
            /* invert: laundered RMW as THEN, plain zero as ELSE
               -> movls/strbls + bls + unpredicated RMW */
            if (*(unsigned char *)(c + 0x5c8) > 0x14) {
                unsigned char *p = (unsigned char *)(c + 0x5c8);
                *p = (unsigned char)(*p - 0x14);
            } else {
                *(unsigned char *)(c + 0x5c8) = 0;
            }
        }
    }

    *(int *)(c + 0xa8) = 0;
    ApproachLinear(*(short *)(c + 0x94), Vec3_HorzAngle(c + 0x5c, &v, 0), 0x5a8);
    func_ov063_0211a964(c, 1);
}

// @symbol func_ov063_02116bf0
extern "C" void func_ov063_02116bf0(char *c)
{
}

// @symbol func_ov063_02116bf4
extern "C" void func_ov063_02116bf4(char* c)
    {
        unsigned short *ip = (unsigned short *)(c + 0x5d4);
        int *r3 = (int *)(c + 0x19c);
        unsigned char st;
    
        *ip = (unsigned short)(*ip & ~8);
        *r3 = *r3 | 1;
        func_ov063_02119c18(c, 0x9f);
    
        st = *(unsigned char*)(c + 0x5cc);
        switch (st) {
        case 0:
            if (*(int*)(c + 0x580) >= 0x3e8000)
                return;
            if (*(int*)(c + 0x5a0) < 5) {
                unsigned char cb = *(unsigned char*)(c + 0x5cb);
                if (cb != 5 && (int)cb - *(int*)(c + 0x5a0) < 2) {
                    char* p = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                        0xd1, 0xfff1, c + 0x5c, c + 0x92, *(signed char*)(c + 0x5d0), -1);
                    if (p != 0) {
                        *(int*)(p + 0x494) = *(int*)(c + 4);
                        *(int*)(p + 0x498) = *(int*)(c + 0x498);
                    }
                    {
                        unsigned char *q = (unsigned char *)(c + 0x5cb);
                        *q = *q + 1;
                    }
                }
                {
                    unsigned char *q = (unsigned char *)(c + 0x5cc);
                    *q = *q + 1;
                }
            }
            if (*(int*)(c + 0x5a0) >= 5)
                *(unsigned char*)(c + 0x5cc) = 2;
            break;
        case 1:
            if (*(unsigned short*)(c + 0x100) > 0x3c)
                *(unsigned char*)(c + 0x5cc) = 0;
            break;
        case 2:
            break;
        }
    }

// @symbol func_ov063_02116d38
extern "C" void func_ov063_02116d38(char* c){
    switch(*(unsigned char*)(c+0x5cc)){
        case 0: func_ov063_02116f48(c); break;
        case 1: func_ov063_02116e14(c); break;
        case 2: func_ov063_02116df0(c); break;
        case 3: func_ov063_02116dbc(c); break;
        case 4: func_ov063_02116d98(c); break;
    }
    func_ov063_0211aa34(c);
}

// @symbol func_ov063_02116d98
extern "C" void func_ov063_02116d98(void *c) {
    void *r4 = c;
    if (func_ov063_0211a564((char *)r4, 0x28)) {
        *((char *)r4 + 0x5CC) = 1;
    }
}

// @symbol func_ov063_02116dbc
extern "C" void func_ov063_02116dbc(void* c) {
    if (func_ov063_0211a3d0((char *)c) == 0) return;
    ((fBase_c *)c)->MarkForDestruction();
    func_0201267c(0xd5, (char*)c + 0x74);
}

// @symbol func_ov063_02116df0
extern "C" void func_ov063_02116df0(void *c) {
    void *r4 = c;
    if (func_ov063_0211a634((char *)r4, 0x14)) {
        *((char *)r4 + 0x5CC) = 1;
    }
}

// @symbol func_ov063_02116e14
extern "C" void func_ov063_02116e14(char* c){
    *(unsigned short*)((int)c + 0x5d4) &= ~0x40;
    if (*(unsigned short*)(c + 0x500 + 0xc0) == 0) {
        func_ov063_02119e38(c, 0x64, 0x200, 0x800);
    }
    {
        int r5 = func_ov063_0211a0dc(c);
        if (func_ov063_0211adb4(c) != 0) {
            *(unsigned char*)(c + 0x5cc) = 0;
        }
        if (r5 == -1) {
            *(unsigned char*)(c + 0x5cc) = 2;
            return;
        }
        if (r5 != 1) return;
    }
    if ((unsigned int)((unsigned short)*(unsigned short*)(c + 0x500 + 0xd4) << 0x19) >> 0x1f) {
        *(unsigned char*)(c + 0x5cc) = 4;
        func_0201267c(0x14a, c + 0x74);
        return;
    }
    *(unsigned char*)(c + 0x5cc) = 3;
    {
        char* r = (char*)_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
            *(unsigned short*)(c + 0x4a0), 0, (struct Vector3*)(c + 0x504), 0,
            *(signed char*)(c + 0x5d0), -1);
        if (r != 0) {
            *(int*)(r + 0xa4) = 0;
            *(int*)(r + 0xa8) = 0x32000;
            *(int*)(r + 0xac) = 0;
        }
    }
    *(unsigned short*)(c + 0x5d4) &= ~2;
}

// @symbol func_ov063_02116f48
extern "C" int func_ov063_02116f48(char *c) {
    *(int*)(c+0x490) = 0;
    *(unsigned char*)(c+0x5c9) = 0xff;
    *(int*)(c+0x584) = 0x2000;
    *(int*)(c+0x80) = 0x2000;
    *(int*)(c+0x84) = 0x2000;
    *(int*)(c+0x88) = 0x2000;
    *(int*)(c+0x188) = *(int*)(c+0x590) * *(int*)(c+0x80);
    *(int*)(c+0x18c) = *(int*)(c+0x594) * *(int*)(c+0x80);
    int r = func_ov063_0211ad00(c);
    if (r) { *(unsigned char*)(c+0x5cc) = 1; r = 1; }
    return r;
}

#define L16(c, off) ((u16*)(((int)(c) + (off))))
#define L8(c, off) ((u8*)(((int)(c) + (off))))
// @symbol func_ov063_02116fac
extern "C" void func_ov063_02116fac(char* c)
{
    struct Frame fr;
    s16 r4 = 0xc00;
    u16* flags;
    u8* state;

    fr.v.x = *(int*)(c + 0x51c);
    fr.v.y = *(int*)(c + 0x520);
    fr.v.z = *(int*)(c + 0x524);
    *(int*)(c + 0x584) = 0x1000;

    switch (*(u8*)(c + 0x5cc)) {
    case 0:
        flags = L16(c, 0x5d4);
        *flags &= ~8;
        if (((u32)(*(u16*)(c + 0x5d4) << 0x1b)) >> 0x1f != 0) {
            state = L8(c, 0x5cc);
            (*state)++;
            *(u8*)(c + 0x5c8) = 0xb4;
            *flags |= 8;
        }
        break;
    case 1:
        if (*(int*)(c + 0x580) < 0x258000) {
            state = L8(c, 0x5cc);
            (*state)++;
            func_0201267c(0xf8, c + 0x74);
        }
        *(int*)(c + 0x98) = 0;
        break;
    case 2:
        _Z14ApproachLinearRiii((int*)(c + 0x98), 0x3a000, 0x1800);
        fr.v.x = -0x3e8000;
        fr.v.y = 0;
        fr.v.z = 0xfea84000;
        if (*(int*)(c + 0x64) < (int)0xfee08000) {
            state = L8(c, 0x5cc);
            (*state)++;
        }
        break;
    case 3:
        _Z14ApproachLinearRiii((int*)(c + 0x98), 0, 0x9000);
        fr.v.x = -0x3e8000;
        fr.v.y = 0;
        fr.v.z = -0xfa0000;
        *(s16*)(c + 0x5bc) = Vec3_HorzAngle(c + 0x5c, &fr.v);
        r4 = 0x1000;
        if (*(s16*)(c + 0x8e) == *(s16*)(c + 0x5bc) && *(int*)(c + 0x98) == 0) {
            state = L8(c, 0x5cc);
            (*state)++;
        }
        break;
    case 4:
        if (*(u16*)(c + 0x100) == 6) {
            *(int*)(c + 0xa8) = 0xf000;
            *(int*)(c + 0x9c) = -0x4000;
            *(int*)(c + 0xa0) = -0xf000;
        }
        if (*(int*)(c + 0x60) < *(int*)(c + 0x520)) {
            *(int*)(c + 0x60) = *(int*)(c + 0x520);
            state = L8(c, 0x5cc);
            (*state)++;
            *(int*)(c + 0x9c) = 0;
            *(int*)(c + 0xa0) = 0;
        }
        break;
    case 5:
        _Z14ApproachLinearRiii((int*)(c + 0x98), 0x3a000, 0x1800);
        fr.v.x = -0x3e8000;
        fr.v.y = 0;
        fr.v.z = 0xfea84000;
        r4 = 0x1000;
        if (*(int*)(c + 0x64) < (int)0xfec78000) {
            state = L8(c, 0x5cc);
            (*state)++;
        }
        break;
    case 6:
        *(int*)(c + 0x98) = 0;
        *(u8*)(c + 0x5c8) = 0;
        flags = L16(c, 0x5d4);
        *flags &= ~8;
        break;
    }

    if (*(u8*)(c + 0x5cc) != 4) {
        *(int*)(c + 0xa8) = 0;
        *(s16*)(c + 0x5bc) = Vec3_HorzAngle(c + 0x5c, &fr.v);
    }
    ApproachLinear(*(short *)(c + 0x94), *(short*)(c + 0x5bc), r4);
    func_ov063_0211a964(c, 1);
}
#undef L16
#undef L8

// @symbol func_ov063_021172a8
extern "C" void func_ov063_021172a8(void* thiz)
{
    char* c = (char*)thiz;
    switch (*(unsigned char*)(c + 0x5cc)) {
    case 0: func_ov063_02118914(c); break;
    case 1: func_ov063_0211873c(c); break;
    case 2: func_ov063_02118f50(c); break;
    case 3: func_ov063_021177b0(c); break;
    case 4: func_ov063_02118458(c); break;
    case 5: func_ov063_0211776c(c); break;
    case 6: func_ov063_02117364(c); break;
    case 7: func_ov063_02117cdc((u8 *)c); break;
    case 8: func_ov063_02117b0c(c); break;
    }
    {
        int state = *(unsigned char*)(c + 0x5cc);
        if (state == 7) return;
        if (state == 6) {
            if (*(unsigned short*)(c + 0x5c2) > 0x5a) return;
        }
        func_ov063_0211aa34(c);
    }
}

#define New _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE
// @symbol func_ov063_02117364
extern "C" void func_ov063_02117364(void* c)
{
    volatile Vec3 pos;
    pos.x = *(s32*)((char*)c + 0x5c);
    pos.y = *(s32*)((char*)c + 0x60);
    pos.z = *(s32*)((char*)c + 0x64);

    if (((Sub364*)((char*)c + 0x500))->fc2 != 0)
        (*(u16*)(((int)c + 0x5c2)))--;

    if (*(u8*)((char*)c + 0x5c9) == 0) {
        if (*(u8*)((char*)c + 0x5c8) == 0) {
            if (((Sub364*)((char*)c + 0x500))->fc2 != 0)
                return;
            {
                FlagW* fw = (FlagW*)(((int)c + 0x5d4));
                fw->flag ^= 1;
            }
            func_ov063_02117650((char *)c);
            *(u8*)((char*)c + 0x5c9) = 0xff;
            *(u32*)((char*)c + 0x5dc) = 0;
            *(u32*)((char*)c + 0x5d8) = *(u32*)((char*)c + 0x5dc);
            ((Sub364*)((char*)c + 0x500))->fc2 = 0x78;
            func_02012694(0x154, (char*)c + 0x74, (char*)c + 0x500, 0x78);
            return;
        }
        pos.y += *(s32*)((char*)c + 0x584) * 0xaf;
        if (((Sub364*)((char*)c + 0x500))->flag)
            pos.x = pos.x * -1;
        *(u32*)((char*)c + 0x5d8) = New(*(volatile u32*)((char*)c + 0x5d8), 0x95, pos.x, pos.y, pos.z, 0, 0);
        *(u32*)((char*)c + 0x5dc) = New(*(volatile u32*)((char*)c + 0x5dc), 0x96, pos.x, pos.y, pos.z, 0, 0);
        return;
    }

    if (((Sub364*)((char*)c + 0x500))->fc2 > 0x1c) {
        pos.y += *(s32*)((char*)c + 0x584) * 0xaf;
        if (((Sub364*)((char*)c + 0x500))->flag)
            pos.x = pos.x * -1;
        *(u32*)((char*)c + 0x5d8) = New(*(volatile u32*)((char*)c + 0x5d8), 0x97, pos.x, pos.y, pos.z, 0, 0);
        *(u32*)((char*)c + 0x5dc) = New(*(volatile u32*)((char*)c + 0x5dc), 0x98, pos.x, pos.y, pos.z, 0, 0);
    }
    if (*(u8*)((char*)c + 0x5c8) != 0xff)
        return;
    if (((Sub364*)((char*)c + 0x500))->fc2 > 0x1c)
        ((Sub364*)((char*)c + 0x500))->fc2 = 0x1c;
    if (((Sub364*)((char*)c + 0x500))->fc2 != 0)
        return;
    *(u8*)((char*)c + 0x5cc) = 1;
    *(u32*)(((int)c + 0x19c)) &= ~1u;
    ((Sub364*)((char*)c + 0x500))->fbe = ((u32)RandomIntInternal(&data_0209e650) >> 16 & 0x3f) + 0x3c;
    ((Sub364*)((char*)c + 0x500))->fc4 = ((u32)RandomIntInternal(&data_0209e650) >> 16) % 150 + 0x12c;
    *(u32*)((char*)c + 0x5dc) = 0;
    *(u32*)((char*)c + 0x5d8) = *(u32*)((char*)c + 0x5dc);
}
#undef New

// @symbol func_ov063_02117650
extern "C" void func_ov063_02117650(char *self)
{
    struct Vector3 ppos;
    struct Vector3 npos;
    char *p;
    int neg1 = (int)(-1LL);

    p = (char *)_ZN8dActor_c13ClosestPlayerEv(self);
    if (p == 0) {
        return;
    }

    {
        int *pp = (int *)(p + 0x5c);
        ppos.x = pp[0];
        ppos.y = pp[1];
        ppos.z = pp[2];
    }
    npos.y = *(int *)(self + 0x60);

    do {
        npos.x = ((int)(((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) % 0x578) * neg1 - 0x190) << 0xc;
        npos.z = ((int)(((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) % 0xa28) - 0x514) << 0xc;
    } while (Vec3_HorzDist(&ppos, &npos) < 0x320000);

    *(int *)(self + 0x5c) = npos.x;
    *(int *)(self + 0x60) = npos.y;
    *(int *)(self + 0x64) = npos.z;
    *(short *)(self + 0x8e) = Vec3_HorzAngle((struct Vector3 *)(self + 0x5c), &ppos);
    *(short *)(self + 0x94) = *(short *)(self + 0x8e);
}

// @symbol func_ov063_0211776c
extern "C" void func_ov063_0211776c(char* c){
  func_ov063_0211adfc(c);
  unsigned short v=*(unsigned short*)(c+0x100);
  if(v>0xf) return;
  if(v!=0xf) return;
  func_ov063_02118ddc(c);
  ((fBase_c *)c)->MarkForDestruction();
}

// @symbol func_ov063_021177b0
extern "C" void func_ov063_021177b0(char* c)
{
    if (*(unsigned short*)(c + 0x100) == 0) {
        unsigned char* p = (unsigned char*)(c + 0x5ca);
        *p = (unsigned char)(*p - 1);
    }

    if (*(unsigned char*)(c + 0x5ca) == 0) {
        void* cam = data_0209f318;
        unsigned short flags = *(unsigned short*)(c + 0x5d4);

        if (((unsigned)(flags << 21)) >> 31) {
            void* pl = _ZN8dActor_c13ClosestPlayerEv(c);
            if (pl != 0) {
                Vec3 v;
                Vec3 mid;
                Vec3 look;
                int dist;
                int scaled;
                short ang;
                short sn;
                int t;

                {
                    int* src = (int*)((char*)pl + 0x5c);
                    v.x = src[0];
                    v.y = src[1];
                    v.z = src[2];
                }
                dist = Vec3_Dist((char*)c + 0x5c, &v);
                if (((Sub500*)(c + 0x500))->flag) {
                    mid.x = (v.x - *(int*)(c + 0x5c)) / 2;
                } else {
                    mid.x = (*(int*)(c + 0x5c) + v.x) / 2;
                }
                mid.y = (*(int*)(c + 0x60) + v.y) / 2;
                mid.z = (*(int*)(c + 0x64) + v.z) / 2;
                scaled = (int)((((long long)dist << 12) + 0x800) >> 12);
                look.x = mid.x;
                look.y = mid.y;
                look.z = mid.z;
                look.x = mid.x - scaled;

                if (look.x <= (int)0xFF894000) {
                    ang = Vec3_HorzAngle(&v, (char*)c + 0x5c);
                    sn = data_02082214[(((unsigned short)ang >> 4) * 2) + 1];
                    t = (int)0xFF894000 - look.x;
                    mid.z = mid.z + (int)(((long long)t * sn + 0x800) >> 12);
                    look.x = (int)0xFF894000;
                }

                func_020092c4(cam, (char*)cam + 0x8c, &look);
                func_020092c4(cam, (char*)cam + 0x80, &mid);
            }
        } else {
            *(unsigned short*)(c + 0x5d4) |= 0x400;
            _ZN6Camera9SetFlag_3Ev(cam);
        }

        if (func_ov063_0211a3d0(c) == 0)
            return;

        {
            unsigned short* pf = (unsigned short*)(c + 0x5d4);
            *pf = (unsigned short)(*pf & ~8);
        }
        *(unsigned char*)(c + 0x5cc) = 8;
        func_ov063_0211adfc(c);
        *(unsigned short*)(c + 0x92) = 0;
        *(unsigned short*)(c + 0x94) = 0;
        *(unsigned short*)(c + 0x96) = 0;
        *(unsigned char*)(c + 0x5ce) = 0;

        {
            volatile Vec3 pos;
            char *b500 = c + 0x500;
            int px = *(int*)(c + 0x5c);
            pos.x = px;
            int py = *(int*)(c + 0x60);
            pos.y = py;
            pos.z = *(int*)(c + 0x64);
            pos.y = py + 0xc8000;
            {
                int yarg = pos.y;
                if (((unsigned)(*(unsigned short*)(b500 + 0xd4) << 23)) >> 31) {
                    int m = ~0;
                    pos.x = px * (volatile int)m;
                }
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x93, pos.x, yarg, pos.z);
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x94, pos.x, pos.y, pos.z);
            }
        }
        return;
    }

    if (*(unsigned short*)(c + 0x100) == 0) {
        int* p584 = (int*)(c + 0x584);
        int dec = 0x255;
        *p584 = *p584 - dec;
    }
    if (func_ov063_0211a564(c, 0x28) == 0)
        return;
    *(unsigned char*)(c + 0x5cc) = 6;
    {
        int* p19c = (int*)(c + 0x19c);
        *p19c |= 1;
    }
    *(unsigned char*)(c + 0x5c9) = 0;
    func_ov063_0211adfc(c);
    *(unsigned short*)(c + 0x5c2) = 0x78;
    func_02012694(0x153, c + 0x74);
}

// @symbol func_ov063_02117b0c
extern "C" void func_ov063_02117b0c(char *c)
{
    int v[3];

    switch (*(u8 *)(c + 0x5ce)) {
    case 0:
        *(void **)(c + 0x488) = _ZN8dActor_c13ClosestPlayerEv(c);
        if (!_ZN6Player9StartTalkER7fBase_cb(*(void **)(c + 0x488), c, 1))
            return;
        {
            u8 *st = (u8 *)(c + 0x5ce);
            *st = *st + 1;
        }
        _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x14, 0x15666);
        break;
    case 1:
        {
            u8 *st = (u8 *)(c + 0x5ce);
            *st = *st + 1;
        }
        break;
    case 2:
        if (_ZN6Player12GetTalkStateEv(*(void **)(c + 0x488)) != 0)
            return;
        {
            int y = *(int *)(c + 0x60);
            int x = *(int *)(c + 0x5c);
            int z = *(int *)(c + 0x64);
            int ny = y + 0xc8000;
            int nx = -x;
            v[0] = nx;
            v[1] = ny;
            v[2] = z;
        }
        {
            void *pl = *(void **)(c + 0x488);
            unsigned m = (unsigned)(int)data_ov063_0211e1c8[*(int *)((char *)pl + 8)];
            if (!_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
                    *(void **)(c + 0x484), c, m, v, 0, 2))
                return;
            func_0201277c(0x151);
            {
                u8 *st = (u8 *)(c + 0x5ce);
                *st = *st + 1;
            }
        }
        break;
    case 3:
        if (_ZN6Player12GetTalkStateEv(*(void **)(c + 0x488)) != -1)
            return;
        *(u8 *)(c + 0x5cc) = 5;
        _ZN5Sound22StopLoadedMusic_Layer3Ev();
        _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x7222);
        func_02011cfc();
        {
            u16 *fl = (u16 *)(c + 0x5d4);
            *fl = (u16)(*fl & ~0x400);
        }
        {
            char *base = data_0209f318;
            int *gp = (int *)(base + 0x154);
            *gp = *gp & ~8;
        }
        break;
    }
}

// @symbol func_ov063_02117cdc
extern "C" void func_ov063_02117cdc(u8 *arg0) {
    s16 v[3];
    int w[3];
    void *cam;
    int neg;

    cam = data_0209f318;
    switch (arg0[0x5ce]) {
    case 0:
        *(void **)(arg0 + 0x488) = _ZN8dActor_c13ClosestPlayerEv(arg0);
        if (*(int *)(*(int *)(arg0 + 0x488) + 0x5c) <= -0x2bc000) {
            return;
        }
        *(u8 *)(arg0 + 0x5ce) += 1;
        *(s16 *)(arg0 + 0x100) = 0;
        return;
    case 1: {
        u8 *st;
        int n;
        if (*(u16 *)(arg0 + 0x100) < 0x96) {
            return;
        }
        st = (u8 *)(arg0 + 0x5ce);
        n = *st + 1;
        *st = n;
        return;
    }
    case 2: {
        Vec3 *src;
        int *d54c;
        int *d554;
        if (*(int **)(arg0 + 0x488) == 0) {
            return;
        }
        src = (Vec3 *)((char *)*(int **)(arg0 + 0x488) + 0x5c);
        w[0] = src->x;
        w[1] = src->y;
        w[2] = src->z;
        if (LenVec3(w) >= 0x12c000) {
            return;
        }
        if (_ZN6Player9StartTalkER7fBase_cb(*(void **)(arg0 + 0x488), arg0, 1) == 0) {
            return;
        }
        _ZN6Camera9SetFlag_3Ev(cam);
        *(int *)(arg0 + 0x54c) = w[0];
        *(int *)(arg0 + 0x550) = w[1];
        *(int *)(arg0 + 0x554) = w[2];
        d54c = (int *)(arg0 + 0x54c);
        d554 = (int *)(arg0 + 0x554);
        *d54c = *d54c - (0x3c000 - (w[0] / 8));
        *(int *)(arg0 + 0x550) = 0x64000;
        *d554 = *d554 - (0x64000 - (w[0] / 6));
        *(int *)(arg0 + 0x558) = w[0];
        *(int *)(arg0 + 0x55c) = w[1];
        *(int *)(arg0 + 0x560) = w[2];
        *(int *)(arg0 + 0x558) = 0;
        *(int *)(arg0 + 0x55c) = 0x64000;
        {
            u8 *st = (u8 *)(arg0 + 0x5ce);
            int n = *st + 1;
            *st = n;
        }
        return;
    }
    case 3: {
        s16 ang;
        int b;
        int *src;
        s16 *q;
        src = (int *)((char *)*(void **)(arg0 + 0x488) + 0x5c);
        w[0] = src[0];
        w[1] = src[1];
        w[2] = src[2];
        w[0] = 0;
        ang = Vec3_HorzAngle((int *)((char *)*(void **)(arg0 + 0x488) + 0x5c), w);
        q = (s16 *)((char *)*(void **)(arg0 + 0x488) + 0x8c);
        v[0] = q[0];
        v[1] = q[1];
        v[2] = q[2];
        b = func_020092c4(cam, (char *)cam + 0x8c, arg0 + 0x54c);
        b = b & func_020092c4(cam, (char *)cam + 0x80, arg0 + 0x558);
        if (ApproachLinear(v[1], ang, 0x200) != 0 && b != 0) {
            *(u8 *)(arg0 + 0x5ce) += 1;
            *(s16 *)(arg0 + 0x100) = 0;
        }
        {
            char *d = (char *)*(void **)(arg0 + 0x488);
            *(s16 *)(d + 0x8c) = v[0];
            *(s16 *)(d + 0x8e) = v[1];
            *(s16 *)(d + 0x90) = v[2];
        }
        {
            char *d = (char *)*(void **)(arg0 + 0x488);
            *(s16 *)(d + 0x92) = v[0];
            *(s16 *)(d + 0x94) = v[1];
            *(s16 *)(d + 0x96) = v[2];
        }
        return;
    }
    case 4:
        if (*(u16 *)(arg0 + 0x100) < 0x1e) {
            return;
        }
        _ZN17daObjSlIceBlock_c16CleanupResourcesEv();
        {
            u8 *st = (u8 *)(arg0 + 0x5ce);
            int n = *st + 1;
            *st = n;
        }
        return;
    case 5: {
        int *src;
        int *d560;
        u8 *st;
        int v0;
        if ((&data_ov008_02111b6c)[0] == 0x1f000 || (data_0209caa0[1] & 0x10)) {
            v0 = 1;
        } else {
            v0 = 0;
        }
        if (v0 == 0) {
            if ((data_0209caa0[1] & 0x10) == 0) {
                return;
            }
        }
        src = (int *)((char *)*(void **)(arg0 + 0x488) + 0x5c);
        d560 = (int *)(arg0 + 0x560);
        w[0] = src[0];
        w[1] = src[1];
        w[2] = src[2];
        *d560 = *d560 + (0x50000 - (w[0] / 3));
        *(int *)(arg0 + 0x5c) = w[0] - 0xc8000;
        *(int *)(arg0 + 0x64) = (*(int *)(arg0 + 0x560) + 0x12c000) - ((w[0] * 2) / 3);
        *(int *)(arg0 + 0x54c) = w[0] - 0x82000;
        *(int *)(arg0 + 0x554) = w[2] - 0x32000;
        *(s16 *)(arg0 + 0x100) = 0;
        {
            u8 *st2 = (u8 *)(arg0 + 0x5ce);
            int n = *st2 + 1;
            *st2 = n;
        }
        return;
    }
    case 6:
        if (*(u16 *)(arg0 + 0x100) < 0x1e) {
            return;
        }
        {
            u8 *st = (u8 *)(arg0 + 0x5ce);
            int n = *st + 1;
            *st = n;
        }
        return;
    case 7: {
        int *src;
        int t0;
        t0 = func_020092c4(cam, (char *)cam + 0x8c, arg0 + 0x54c);
        if ((t0 & func_020092c4(cam, (char *)cam + 0x80, arg0 + 0x558)) == 0) {
            return;
        }
        arg0[0x5c8] = 0;
        *(u8 *)(arg0 + 0x5ce) += 1;
        neg = -1;
        src = (int *)((char *)*(void **)(arg0 + 0x488) + 0x5c);
        w[0] = src[0];
        w[1] = src[1];
        w[2] = src[2];
        w[0] = w[0] * neg;
        *(s16 *)(arg0 + 0x8e) = Vec3_HorzAngle((int *)(arg0 + 0x5c), w);
        *(s16 *)(arg0 + 0x94) = *(s16 *)(arg0 + 0x8e);
        func_0201277c(0x150);
        return;
    }
    case 8:
        /* invert so ELSE (=0xff) is predicated and THEN (RMW) is branched (codegen 6c) */
        if (arg0[0x5c8] + 5 < 0xff) {
            u8 *p = (u8 *)(arg0 + 0x5c8);
            *p = (u8)(*p + 5);
        } else {
            arg0[0x5c8] = 0xff;
        }
        w[0] = *(int *)(arg0 + 0x5c);
        w[1] = *(int *)(arg0 + 0x60);
        w[2] = *(int *)(arg0 + 0x64);
        neg = -1;
        w[0] = w[0] * neg;
        w[1] = w[1] + 0xc8000;
        func_ov002_020c51d0(*(void **)(arg0 + 0x488), w);
        if (arg0[0x5c8] == 0xff) {
            *(u8 *)(arg0 + 0x5ce) += 1;
            arg0[0x5c9] = 0xff;
        }
        *(s16 *)(arg0 + 0x8e) = *(s16 *)(arg0 + 0x94);
        return;
    case 9: {
        int tk = _ZN6Player12GetTalkStateEv(*(void **)(arg0 + 0x488));
        s16 msg;
        if (tk != 0) {
            return;
        }

        {
            int x = 0 - *(int *)(arg0 + 0x5c);
            int z = *(int *)(arg0 + 0x64);
            int y = *(int *)(arg0 + 0x60) + 0xc8000;
            int fl = data_0209caa0[1] & 0x10;
            w[0] = x;
            w[1] = y;
            w[2] = z;
            if (fl)
                msg = data_ov063_0211e1e4[*(int *)(*(int *)(arg0 + 0x488) + 8)];
            else
                msg = data_ov063_0211e1dc[*(int *)(*(int *)(arg0 + 0x488) + 8)];
        }
        if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(*(void **)(arg0 + 0x484), arg0, (u32)msg, w, 0, 2) == 0) {
            return;
        }
        _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2c);
        {
            u8 *st = (u8 *)(arg0 + 0x5ce);
            int n = *st + 1;
            *st = n;
        }
        return;
    }
    case 10: {
        int tk = _ZN6Player12GetTalkStateEv(*(void **)(arg0 + 0x488));
        u32 rr;
        if (tk != -1) {
            return;
        }
        arg0[0x5cc] = 1;
        *(int *)(arg0 + 0x19c) &= ~1;
        *(s16 *)(arg0 + 0x5be) = (((u32)RandomIntInternal(&data_0209e650) >> 0x10) & 0x3f) + 0xb4;
        *(int *)((char *)cam + 0x154) &= ~8;
        rr = RandomIntInternal(&data_0209e650);
        *(s16 *)(arg0 + 0x5c4) = ((rr >> 0x10) % 0x96) + 0x12c;
        _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2d);
        func_02011d14();
        return;
    }
    }
    return;
}

// @symbol func_ov063_02118458
extern "C" void func_ov063_02118458(void* self)
{
    char* c = (char*)self;
    int r4 = func_ov063_0211a0dc(c);

    ApproachLinear(*(short *)(c + 0x94), *(s16*)(c + 0x5b0),
        data_ov063_0211e1c0[*(u8*)(c + 0x5ca) - 1]);

    if (_ZN9Animation8FinishedEv(c + 0x3d0)) {
        if (func_ov063_0211a8a4(c)) {
            if (*(int*)(c + 0x3e0) == *(int*)((char *)&data_ov063_0211edcc + 4)) {
                func_02012694(0x158, c + 0x74);
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x380,
                    *(void**)((char *)&data_ov063_0211edd4 + 4), 0x40000000, 0x1000, 0);
            } else if (*(u8*)(c + 0x5d2) != 0) {
                *(u8*)(c + 0x5d2) -= 1;
                *(int*)(c + 0x3d8) = 0;
                func_02012694(0x158, c + 0x74);
            } else {
                int rnd;
                *(u8*)(c + 0x5cc) = 1;
                *(int*)(c + 0x19c) &= ~1;
                rnd = ((unsigned int)RandomIntInternal(&data_0209e650) >> 16) & 0x3f;
                *(u16*)(c + 0x5be) = rnd + 0xb4;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x380,
                    *(void**)((char *)&data_ov063_0211ede4 + 4), 0, 0x1000, 0);
            }
        } else {
            int rnd;
            *(u8*)(c + 0x5cc) = 1;
            *(int*)(c + 0x19c) &= ~1;
            rnd = ((unsigned int)RandomIntInternal(&data_0209e650) >> 16) & 0x3f;
            *(u16*)(c + 0x5be) = rnd + 0xb4;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x380,
                *(void**)((char *)&data_ov063_0211ede4 + 4), 0, 0x1000, 0);
        }
    } else {
        if (*(int*)(c + 0x3e0) == *(int*)((char *)&data_ov063_0211edd4 + 4)
            && _ZNK9Animation12WillHitFrameEi(c + 0x3d0, 6)) {
            Vec3 v;
            s16 m = 0x78;
            v.x = *(int*)(c + 0x5c);
            v.y = *(int*)(c + 0x60);
            v.z = *(int*)(c + 0x64);
            v.x = data_02082214[(*(u16*)(c + 0x8e) >> 4) * 2] * m + v.x;
            v.z = data_02082214[(*(u16*)(c + 0x8e) >> 4) * 2 + 1] * m + v.z;
            char* fb;
            *(s16*)(c + 0x8c) = 0x1000;
            func_02012694(0x156, c + 0x74);
            fb = _ZN8dActor_c13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(
                c, &v, (const void*)(c + 0x8c), 0x14000, 0x6999, 4);
            *(s16*)(c + 0x8c) = 0;
            if (((unsigned int)*(u16*)(c + 0x5d4) << 23) >> 31)
                *(u8*)(fb + 0x36e) = 1;
            else
                *(u8*)(fb + 0x36e) = 0;
            *(int*)(fb + 0x364) = 0x3e8000;
            func_02012694(0x122, c + 0x74);
        }
    }

    if (r4 == -1) {
        *(u8*)(c + 0x5cc) = 2;
        return;
    }
    if (r4 != 1)
        return;
    *(u8*)(c + 0x5cc) = 3;
    func_02012694(0x152, c + 0x74);
}

#define ModelAnim_SetAnim _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj
// @symbol func_ov063_0211873c
extern "C" void func_ov063_0211873c(char* self)
{
    short a;
    int b;
    int r;
    u8 st = *(u8*)(self + 0x5ca);
    if (st == 3) {
        a = 0x180;
        b = 0x999;
    } else if (st == 2) {
        a = 0x240;
        b = 0x1000;
    } else {
        a = 0x380;
        b = 0x1800;
    }
    if (*(u16*)(self + 0x500 + 0xc0) != 0) {
        a = 0;
        b = 0;
    }
    func_ov063_02119e38(self, -100, a, b);
    r = func_ov063_0211a0dc(self);
    if (r == -1) {
        *(u8*)(self + 0x5cc) = 2;
        return;
    }
    if (r == 1) {
        *(u8*)(self + 0x5cc) = 3;
        if (*(u8*)(self + 0x5ca) == 1)
            func_02012694(0x155, self + 0x74);
        else
            func_02012694(0x152, self + 0x74);
        return;
    }
    if (*(u16*)(self + 0x500 + 0xc4) == 0) {
        int* p = (int*)(self + 0x19c);
        *(u8*)(self + 0x5cc) = 6;
        *p |= 1;
        *(u8*)(self + 0x5c9) = 0;
        *(int*)(self + 0x98) = 0;
        func_ov063_0211adfc(self);
        *(u16*)(self + 0x500 + 0xc2) = 0x78;
        func_02012694(0x153, self + 0x74);
        return;
    }
    if (*(u16*)(self + 0x500 + 0xbe) != 0)
        return;
    *(u8*)(self + 0x5cc) = 4;
    *(int*)(self + 0xa8) = 0;
    *(int*)(self + 0x9c) = 0;
    func_02012694(0x159, self + 0x74);
    ModelAnim_SetAnim(self + 0x380, *(void**)((char *)&data_ov063_0211edcc + 4), 0x40000000, 0x1000, 0);
    {
        unsigned int rnd = (unsigned int)RandomIntInternal(&data_0209e650);
        *(u8*)(self + 0x5d2) = (u8)((rnd >> 0x10) % 3);
    }
    *(int*)(self + 0x98) = 0;
}
#undef ModelAnim_SetAnim

// @symbol func_ov063_02118914
extern "C" void func_ov063_02118914(char *c)
{
    int zero = 0;
    u8 copied;
    s32 scale;

    *(u8 *)(c + 0x5cc) = 7;
    *(u8 *)(c + 0x5c8) = zero;
    copied = *(u8 *)(c + 0x5c8);
    *(u8 *)(c + 0x5c9) = copied;
    *(s32 *)(c + 0x584) = 0x1000;
    *(u8 *)(c + 0x5ca) = 3;

    scale = *(s32 *)(c + 0x584);
    *(s32 *)(c + 0x80) = scale;
    *(s32 *)(c + 0x84) = scale;
    *(s32 *)(c + 0x88) = scale;
    *(s32 *)(c + 0x188) = *(s32 *)(c + 0x590) * *(s32 *)(c + 0x584);
    *(s32 *)(c + 0x18c) = *(s32 *)(c + 0x594) * *(s32 *)(c + 0x584);
    *(u16 *)(c + 0x5d4) |= 8;
    *(u32 *)(c + 0x19c) |= 1;
    *(u16 *)(c + 0x5d4) |= 0x100;
    *(s32 *)(c + 0x5c) = -*(s32 *)(c + 0x51c);
    *(s16 *)(c + 0x8e) += 0x8000;
    *(s16 *)(c + 0x94) += 0x8000;
    *(u8 *)(c + 0x5ce) = zero;
    *(u32 *)(c + 0xb0) &= ~2;
}

// @symbol func_ov063_021189f4
extern "C" void func_ov063_021189f4(char *c)
{
    if (*(u16 *)(c + 0x500 + 0xc6) != 0) {
        if (*(u8 *)(c + 0x5cf) == 0xd) {
            if (*(u16 *)(c + 0x500 + 0xc6) < 0x4b) {
                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x14, 0x7f, 0x6b000, 1);
                *(u16 *)(c + 0x5c6) += 1;
            } else {
                int r = _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, *(s32 *)(c + 0x57c) >> 0xc, 0, 0x7f000, 1);
                if (r != 0)
                    *(u16 *)(c + 0x500 + 0xc6) = 0;
            }
        } else {
            int r = _ZN5Sound15PlaySecretSoundEP8dActor_cPt(c, (u16 *)(c + 0x5c6));
            if (r != 0)
                *(u16 *)(c + 0x500 + 0xc6) = 0;
        }
    }

    switch (*(u8 *)(c + 0x5cc)) {
    case 0:
        func_ov063_02119074(c);
        break;
    case 1:
        func_ov063_02118f74(c);
        break;
    case 2:
        func_ov063_02118f50(c);
        break;
    case 3:
        func_ov063_02118cd8(c);
        break;
    case 4:
        func_ov063_02118b98(c);
        break;
    case 5:
        func_ov063_02118b2c(c);
        break;
    }

    func_ov063_0211aa34(c);
}

// @symbol func_ov063_02118b2c
extern "C" int func_ov063_02118b2c(char *c) {
    int r;
    *(int *)(c + 0x98) = 0x5000;
    r = func_ov063_0211a0dc(c);
    if (r == -1) {
        *(char *)(c + 0x5cc) = 2;
        return 2;
    }
    if (r == 1) {
        *(char *)(c + 0x5cc) = 3;
        return func_0201267c(0xc7, c + 0x74);
    }
    {
        unsigned x = *(unsigned short *)(c + 0x100);
        if (x > 0x64) {
            x = 1;
            *(char *)(c + 0x5cc) = 1;
        }
        return x;
    }
}

// @symbol func_ov063_02118b98
extern "C" void func_ov063_02118b98(char *c)
{
    u16 v;

    *(u16 *)(c + 0x5d4) &= ~0x40;
    func_ov063_0211adfc(c);

    v = *(u16 *)(c + 0x100);
    if (v > 0xf) goto bigblock;
    if (v != 0xf) return;

    {
        int v2 = *(int *)(c + 0x5a4);
        if (v2 == 0) {
            func_ov063_02118eac(c);
        } else if (v2 == 1) {
            func_ov063_02118e5c(c);
        } else {
            func_ov063_02118ea0(c);
        }
    }

    *(int *)(c + 0x5c) = *(int *)(c + 0x51c);
    *(int *)(c + 0x60) = *(int *)(c + 0x520);
    *(int *)(c + 0x64) = *(int *)(c + 0x524);
    return;

bigblock:
    if (*(int *)(c + 0x5a4) != 0) goto special;
    if (v <= 0x3c) return;
    if (*(int *)(c + 0x580) >= 0x258000) return;
    {
        int r1 = 0;
        for (;;) {
            r1 = (int)_ZN8dActor_c15FindWithActorIDEjPS_(0x41, (void *)r1);
            if (r1 == 0) goto after_strb;
            if (((unsigned int)*(int *)(r1 + 8) >> 8 & 3) == 0) break;
        }
        *((char *)r1 + 0x155) = 1;
    }
after_strb:
    ((fBase_c *)c)->MarkForDestruction();
    func_0201267c(0xd5, c + 0x74);
    return;

special:
    ((fBase_c *)c)->MarkForDestruction();
    func_0201267c(0xd5, c + 0x74);
}

// @symbol func_ov063_02118cd8
extern "C" void func_ov063_02118cd8(char *self) {
    if (*(unsigned short *)(self + 0x100) == 0) {
        unsigned int v = *(unsigned short *)(self + 0x5d4);
        if ((v << 25 >> 31) == 0) {
            unsigned char *p = (unsigned char *)(((int)self + 0x5ca));
            (*p)--;
        }
    }
    if (*(unsigned char *)(self + 0x5ca) == 0) {
        unsigned short *p;
        if (func_ov063_0211a3d0(self) == 0) return;
        *(int *)(((int)self + 0x19c)) |= 1;
        p = (unsigned short *)(((int)self + 0x5d4));
        *p &= ~8;
        *(unsigned char *)(self + 0x5cc) = 4;
        *(unsigned short *)(self + 0x92) = 0;
        *(unsigned short *)(self + 0x94) = 0;
        *(unsigned short *)(self + 0x96) = 0;
        *p |= 0x80;
        return;
    }
    if (*(unsigned short *)(self + 0x100) == 0) {
        *(int *)(((int)self + 0x584)) -= 0x255;
    }
    if (func_ov063_0211a564(self, 0x28) != 0) {
        *(unsigned char *)(self + 0x5cc) = 1;
    }
}

// @symbol func_ov063_02118ddc
extern "C" void func_ov063_02118ddc(char* c){
  struct Vector3 v;
  v.x = *(int*)(c+0x5c);
  int y = *(int*)(c+0x60);
  v.y = y;
  v.z = *(int*)(c+0x64);
  v.y = y + 0x64000;
  int s = *(int*)(c+0x5c);
  if(s < 0) s = -s;
  v.x = s;
  _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x11a, 3, &v, 0, *(signed char*)(c+0x5d0), -1);
  func_02012694(0xbb, c+0x74);
}

// @symbol func_ov063_02118e5c
extern "C" void func_ov063_02118e5c(void *c) {
    func_ov063_02118eac((char *)c);
    unsigned int id = *(unsigned int*)((char*)c + 0x498);
    if (id == 0) return;
    void *actor = _ZN8dActor_c10FindWithIDEj(id);
    *(void**)((char*)c + 0x48c) = actor;
    void *p = *(void**)((char*)c + 0x48c);
    if (p != 0) {
        *(char*)((char*)p + 0x150) = 1;
    }
    *(void**)((char*)c + 0x48c) = 0;
}

// @symbol func_ov063_02118ea0
extern "C" void func_ov063_02118ea0(char *c) {
    func_ov063_02118eac(c);
}

// @symbol func_ov063_02118eac
extern "C" void func_ov063_02118eac(char *c)
{
    Vec3 v;
    void *p;
    unsigned int id;

    v.x = *(int *)(c + 0x5c);
    v.y = *(int *)(c + 0x60);
    v.z = *(int *)(c + 0x64);
    *(int *)(((int)c + 0x60)) =
        *(int *)(((int)c + 0x60)) + 0x64000;
    if (*(unsigned int *)(c + 0x49c) == 0) {
        return;
    }
    id = *(unsigned int *)(c + 0x49c);
    p = _ZN8dActor_c10FindWithIDEj(id);
    *(void **)(c + 0x48c) = p;
    if (*(void **)(c + 0x48c) != 0) {
        func_ov063_02118f24(*(void **)(c + 0x48c), &v);
    }
    *(void **)(c + 0x48c) = 0;
}

// @symbol func_ov063_02118f24
extern "C" void func_ov063_02118f24(void *c, void *vec)
{
    unsigned int val = 4;
    _ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h(c, (char*)c + 0xd4, *(unsigned char*)((char*)c + 0xd5), vec, val);
}

// @symbol func_ov063_02118f50
extern "C" void func_ov063_02118f50(void* c){
  if(func_ov063_0211a634((char *)c, 0x14)) *(unsigned char*)((char*)c+0x5cc)=1;
}

// @symbol func_ov063_02118f74
extern "C" void func_ov063_02118f74(char *c) {
    short a2;
    int a3;
    int r5v;
    unsigned char mode;

    *(unsigned short *)(c + 0x5d4) &= ~0x40;
    mode = *(unsigned char *)(c + 0x5ca);
    if (mode == 3) {
        a2 = 0x180;
        a3 = 0x999;
    } else if (mode == 2) {
        a2 = 0x240;
        a3 = 0x1000;
    } else {
        a2 = 0x380;
        a3 = 0x1800;
    }
    if (*(unsigned short *)(c + 0x5c0) != 0) {
        a2 = 0;
        a3 = 0;
    }
    func_ov063_02119e38(c, -0x64, a2, a3);
    r5v = func_ov063_0211a0dc(c);
    if (*(unsigned char *)(c + 0x5cf) == 0xd) {
        if (func_ov063_021163d0(c) == 0) {
            *(unsigned char *)(c + 0x5cc) = 0;
        }
    } else {
        if (func_ov063_0211adb4(c) != 0) {
            *(unsigned char *)(c + 0x5cc) = 0;
        }
    }
    if (r5v == -1) {
        *(unsigned char *)(c + 0x5cc) = 2;
        return;
    }
    if (r5v != 1) {
        return;
    }
    *(unsigned char *)(c + 0x5cc) = 3;
    func_0201267c(0xc7, (const struct Vector3 *)(c + 0x74));
}

// @symbol func_ov063_02119074
extern "C" void func_ov063_02119074(char *self)
{
    volatile int dummy[2];
    (void)&dummy;
    /* ROM: beq body on ==0xc; bne after on !=0xf  =>  body when 0xc OR 0xf */
    if (*(unsigned char *)(self + 0x5cf) == 0xc ||
        *(unsigned char *)(self + 0x5cf) == 0xf) {
        func_ov063_02116bf0(self);
        *(int *)(self + 0x180) = 0xa;
    }

    if (*(unsigned char *)(self + 0x5cf) == 0xd)
        func_ov063_02119c18(self, 0x9f);

    *(int *)(self + 0x490) = 0;
    *(unsigned char *)(self + 0x5c8) = 0x28;

    if (func_ov063_0211ad00(self) != 0 &&
        *(int *)(self + 0x180) >= 5) {
        char *r;
        s32 scale;

        *(u16 *)(self + 0x5d4) |= 8;
        *(unsigned char *)(self + 0x5ca) = 3;
        *(int *)(self + 0x584) =
            data_ov063_0211e1d0[*(unsigned char *)(self + 0x5cf) - 0xc];
        scale = *(int *)(self + 0x584);
        *(int *)(self + 0x80) = scale;
        *(int *)(self + 0x84) = scale;
        *(int *)(self + 0x88) = scale;

        _ZN5Model12SetPolygonIDEi(self + 0x380, 0x16);

        if (*(unsigned char *)(self + 0x5cf) == 0xd) {
            *(int *)(self + 0x57c) = data_0209b490[0];

            if (((u32)(*(u16 *)(self + 0x5d4) << 22) >> 31) == 0) {
                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x14, 0x7f,
                                                 0x6b000, 1);
                *(u16 *)(self + 0x500 + 0xc6) = 1;
                *(u16 *)((int)self + 0x5d4) |= 0x200;
            }

            *(int *)(self + 0x98) = 0x800;
            *(unsigned char *)(self + 0x5cc) = 5;
            r = _ZN8dActor_c15FindWithActorIDEjPS_(0x9f, 0);

            if (r != 0)
                *(s16 *)(self + 0x94) =
                    Vec3_HorzAngle((Vec3 *)(self + 0x5c),
                                   (Vec3 *)(r + 0x5c));
        } else {
            if (*(unsigned char *)(self + 0x5cf) == 0xe)
                func_ov063_02116244(self);
            *(unsigned char *)(self + 0x5cc) = 1;
        }

        *(unsigned char *)(self + 0x5c9) = 0xff;
        *(int *)(self + 0x188) =
            *(int *)(self + 0x590) * *(int *)(self + 0x584);
        *(int *)(self + 0x18c) =
            *(int *)(self + 0x594) * *(int *)(self + 0x584);
        *(int *)(self + 0x19c) &= ~1;
    } else {
        *(u16 *)(self + 0x5d4) &= ~8;
        *(int *)((int)self + 0x19c) |= 1;
        func_ov063_0211adfc(self);
    }
}

// @symbol func_ov063_02119274
extern "C" void func_ov063_02119274(char* c){
    switch(*(unsigned char*)(c+0x5cc)){
        case 0: func_ov063_02119b84(c); break;
        case 1: func_ov063_02119894(c); break;
        case 2: func_ov063_02119870(c); break;
        case 3: func_ov063_0211975c(c); break;
        case 4: func_ov063_02119a2c(c); break;
    }
    func_ov063_0211aa34(c);
}

// @symbol func_ov063_021192d4
extern "C" void func_ov063_021192d4(char *c){
  switch(*(unsigned char*)(c+0x5cc)){
  case 0: func_ov063_02119bb0(c); break;
  case 1: func_ov063_02119960(c); break;
  case 2: func_ov063_02119870(c); break;
  case 3: func_ov063_0211975c(c); break;
  case 4: func_ov063_02119a2c(c); break;
  case 5: func_ov063_0211934c(c); break;
  case 6: func_ov063_02119a50(c); break;
  }
  func_ov063_0211aa34(c);
}

// @symbol func_ov063_0211934c
extern "C" void func_ov063_0211934c(char *c)
{
    void *r4;
    u8 st;

    r4 = *(void **)(c + 0x488);
    if (r4 == 0 || *(u8 *)(c + 0x5cf) != 2) {
        _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
        func_0201267c(0xd5, c + 0x74);
        if (((*(u8 *)(c + 0x113)) & 0xf) >= 6)
            return;
        
        {
            void *cap;
            *(s32 *)(c + 0x5c) = *(s32 *)(c + 0x51c);
            *(s32 *)(c + 0x60) = *(s32 *)(c + 0x520);
            *(s32 *)(c + 0x64) = *(s32 *)(c + 0x524);
            *(s8 *)(c + 0xcc) = *(s8 *)(c + 0x5d0);
            *(s16 *)(c + 0x92) = *(s16 *)(c + 0x570);
            *(s16 *)(c + 0x94) = *(s16 *)(c + 0x572);
            *(s16 *)(c + 0x96) = *(s16 *)(c + 0x574);
            s16 *src = (s16 *)((int)c + 0x92);
            *(s16 *)(c + 0x8c) = src[0];
            *(s16 *)(c + 0x8e) = src[1];
            *(s16 *)(c + 0x90) = src[2];
            
            cap = _ZN11dCapEnemy_c15RespawnIfHasCapEv(c);
            if (cap == 0)
                return;
            {
                u16 *p = (u16 *)((int)cap + 0x5d4);
                *p &= ~2;
            }
        }
        return;
    }

    st = *(u8 *)(c + 0x5ce);
    switch (st) {
    case 0:
        if (_ZN6Player9StartTalkER7fBase_cb(r4, c, 1) == 0)
            return;
        {
            u8 *q = (u8 *)(c + 0x5ce);
            *q = *q + 1;
        }
        return;

    case 1:
    {
        void *found;
        *(void **)(c + 0x48c) = _ZN8dActor_c10FindWithIDEj(*(u32 *)(c + 0x490));
        found = *(void **)(c + 0x48c);
        if (found != 0) {
            s32 *cnt = (s32 *)((char *)found + 0x180);
            *cnt = *cnt + 1;
        }
        found = *(void **)(c + 0x48c);
        if (found != 0 && *(s32 *)((char *)found + 0x180) == 5) {
            _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(r4, c, 0xb5, c + 0x5c, 0, 2);
        } else {
            _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(r4, c, 0xb4, c + 0x5c, 0, 2);
        }
        *(void **)(c + 0x48c) = 0;
        {
            u8 *q = (u8 *)(c + 0x5ce);
            *q = *q + 1;
        }
        func_0201267c(0xf8, c + 0x74);
        return;
    }

    case 2:
        if (data_0209d660 != 0)
            return;
        func_0201267c(0xd5, c + 0x74);
        if (*(u32 *)(c + 0x490) != 0) {
            void *found;
            *(void **)(c + 0x48c) = _ZN8dActor_c10FindWithIDEj(*(u32 *)(c + 0x490));
            found = *(void **)(c + 0x48c);
            if (found != 0 && *(s32 *)((char *)found + 0x180) == 5) {
                func_ov063_02116244((char *)found);
                {
                    u16 *p = (u16 *)(c + 0x5c6);
                    *p = *p + 1;
                }
            }
        }
        {
            u8 *q = (u8 *)(c + 0x5ce);
            *q = *q + 1;
        }
        return;

    case 3:
        if (*(u16 *)(c + 0x500 + 0xc6) != 0) {
            if (_ZN5Sound15PlaySecretSoundEP8dActor_cPt(
                    c, (u16 *)(c + 0x5c6)) == 0)
                return;
            ((fBase_c *)c)->MarkForDestruction();
            if (((*(u8 *)(c + 0x113)) & 0xf) >= 6)
                return;
            
        {
            void *cap2;
            *(s32 *)(c + 0x5c) = *(s32 *)(c + 0x51c);
            *(s32 *)(c + 0x60) = *(s32 *)(c + 0x520);
            *(s32 *)(c + 0x64) = *(s32 *)(c + 0x524);
            *(s8 *)(c + 0xcc) = *(s8 *)(c + 0x5d0);
            *(s16 *)(c + 0x92) = *(s16 *)(c + 0x570);
            *(s16 *)(c + 0x94) = *(s16 *)(c + 0x572);
            *(s16 *)(c + 0x96) = *(s16 *)(c + 0x574);
            s16 *src = (s16 *)((int)c + 0x92);
            *(s16 *)(c + 0x8c) = src[0];
            *(s16 *)(c + 0x8e) = src[1];
            *(s16 *)(c + 0x90) = src[2];
            
            cap2 = _ZN11dCapEnemy_c15RespawnIfHasCapEv(c);
            if (cap2 == 0)
                return;
            {
                u16 *p = (u16 *)((int)cap2 + 0x5d4);
                *p &= ~2;
            }
        }
            return;
        }
        ((fBase_c *)c)->MarkForDestruction();
        if (((*(u8 *)(c + 0x113)) & 0xf) >= 6)
            return;
        
        {
            void *cap3;
            *(s32 *)(c + 0x5c) = *(s32 *)(c + 0x51c);
            *(s32 *)(c + 0x60) = *(s32 *)(c + 0x520);
            *(s32 *)(c + 0x64) = *(s32 *)(c + 0x524);
            *(s8 *)(c + 0xcc) = *(s8 *)(c + 0x5d0);
            *(s16 *)(c + 0x92) = *(s16 *)(c + 0x570);
            *(s16 *)(c + 0x94) = *(s16 *)(c + 0x572);
            *(s16 *)(c + 0x96) = *(s16 *)(c + 0x574);
            s16 *src = (s16 *)((int)c + 0x92);
            *(s16 *)(c + 0x8c) = src[0];
            *(s16 *)(c + 0x8e) = src[1];
            *(s16 *)(c + 0x90) = src[2];
            
            cap3 = _ZN11dCapEnemy_c15RespawnIfHasCapEv(c);
            if (cap3 != 0) {
                u16 *p = (u16 *)((int)cap3 + 0x5d4);
                *p &= ~2;
            }
        }
        return;
    }
}

// @symbol func_ov063_0211975c
extern "C" void func_ov063_0211975c(char* self) {
    if (!func_ov063_0211a3d0(self)) return;
    if (*(int*)(self + 0x5a4) != 0) {
        _ZN8dActor_c24KillAndTrackInDeathTableEv(self);
        func_0201267c(0xd5, self + 0x74);
        if ((*(u8*)(self + 0x113) & 0xf) >= 6) return;
        *(int*)(self + 0x5c) = *(int*)(self + 0x51c);
        *(int*)(self + 0x60) = *(int*)(self + 0x520);
        *(int*)(self + 0x64) = *(int*)(self + 0x524);
        *(s8*)(self + 0xcc) = *(s8*)(self + 0x5d0);
        *(s16*)(self + 0x92) = *(s16*)(self + 0x570);
        *(s16*)(self + 0x94) = *(s16*)(self + 0x572);
        *(s16*)(self + 0x96) = *(s16*)(self + 0x574);
        s16* src = (s16*)(((int)self + 0x92));
        *(s16*)(self + 0x8c) = src[0];
        *(s16*)(self + 0x8e) = src[1];
        *(s16*)(self + 0x90) = src[2];
        char* r = (char*)_ZN11dCapEnemy_c15RespawnIfHasCapEv(self);
        if (r == 0) return;
        {
            u16* p = (u16*)(((int)r + 0x5d4));
            *p &= ~2;
        }
    } else {
        *(u8*)(self + 0x5cc) = 5;
        *(u8*)(self + 0x5ce) = 0;
        {
            int* q = (int*)(((int)self + 0x19c));
            *q |= 1;
        }
        {
            u16* p = (u16*)(((int)self + 0x5d4));
            *p &= ~8;
        }
    }
}

// @symbol func_ov063_02119870
extern "C" void func_ov063_02119870(void* c){
  if(func_ov063_0211a634((char *)c, 0x14)) *(unsigned char*)((char*)c+0x5cc)=1;
}

// @symbol func_ov063_02119894
extern "C" void func_ov063_02119894(char *c)
{
    *(unsigned short *)(((int)c + 0x5d4)) &= ~0x40;
    if (*(unsigned short *)(c + 0x100) == 0) {
        unsigned r0 = (unsigned)RandomIntInternal(&data_0209e650);
        *(int *)(c + 0x588) = (int)(((r0 >> 16) & 0xfff) * 5);
        r0 = (unsigned)RandomIntInternal(&data_0209e650);
        {
            int x = (int)((r0 >> 16) & 0xfff);
            *(int *)(c + 0x58c) = (int)(((s64)x << 7) + 0x800 >> 12);
        }
    }
    if (*(unsigned short *)(c + 0x5c0) == 0) {
        func_ov063_02119cc0(c, -100, (short)(*(int *)(c + 0x58c) + 0x180), 0xc00);
    }
    func_ov063_02119b1c(c);
}

#define M(p) (p)
// @symbol func_ov063_02119960
extern "C" void func_ov063_02119960(char *c)
{
    unsigned short *p = (unsigned short *)(int)M(c + 0x5d4);
    *p &= ~0x40;
    if (*(unsigned short *)(c + 0x100) == 0) {
        int r0 = RandomIntInternal(&data_0209e650);
        *(int *)(c + 0x588) = (((unsigned)r0 >> 16) & 0xfff) * 5;
        int ri = RandomIntInternal(&data_0209e650);
        int x = 0xfff & ((unsigned)ri >> 16);
        *(int *)(c + 0x58c) = (int)((((long long)x << 7) + 0x800) >> 12);
    }
    if (*(unsigned short *)(c + 0x5c0) == 0) {
        func_ov063_02119e38(c, -100, (short)(*(int *)(c + 0x58c) + 0x180), 0xfe0);
    }
    func_ov063_02119b1c(c);
}
#undef M

// @symbol func_ov063_02119a2c
extern "C" void func_ov063_02119a2c(void* c){
  if(func_ov063_0211a564((char *)c, 0x28)) *(unsigned char*)((char*)c+0x5cc)=1;
}

// @symbol func_ov063_02119a50
extern "C" void func_ov063_02119a50(char *c) {
    *(unsigned short *)(((int)c + 0x5d4)) &= ~0x40;
    if (*(unsigned short *)(c + 0x100) >= 0x1e) {
        *(unsigned char *)(c + 0x5cc) = 1;
    } else {
        *(int *)(c + 0xa8) = 0;
        *(int *)(c + 0x98) = 0x7ccc;
        func_ov063_0211a964(c, 0);
    }
    func_ov063_02119b1c(c);
}

// @symbol func_ov063_02119ab0
extern "C" void func_ov063_02119ab0(char *self)
{
    if ((unsigned int)(*(unsigned short *)(self + 0x5d4) << 30) >> 31 == 0)
        return;

    *(int *)(self + 0x504) = *(int *)(self + 0x5c);
    *(int *)(self + 0x508) = *(int *)(self + 0x60);
    *(int *)(self + 0x50c) = *(int *)(self + 0x64);

    if (*(unsigned short *)(self + 0x4a0) == 0xd4) {
        *(int *)(self + 0x508) += 0x3c000;
        return;
    }
    *(int *)(self + 0x508) += 0xa000;
}

// @symbol func_ov063_02119b1c
extern "C" void func_ov063_02119b1c(char *c) {
    int r4 = func_ov063_0211a0dc(c);
    if (func_ov063_0211adb4(c) != 0) *(unsigned char *)(c + 0x5cc) = 0;
    if (r4 == -1) {
        *(unsigned char *)(c + 0x5cc) = 2;
        return;
    }
    if (r4 != 1) return;
    func_ov063_021162c8(c);
}

// @symbol func_ov063_02119b84
extern "C" void func_ov063_02119b84(char *c)
{
    func_ov063_0211adfc(c);
    *(unsigned char*)(c + 0x5cc) = 1;
    *(int*)(c + 0x584) = 0x1000;
    *(unsigned char*)(c + 0x5c9) = 0xff;
}

// @symbol func_ov063_02119bb0
extern "C" void func_ov063_02119bb0(char *c) {
    if (*(int *)(c + 0x5a4) == 2) *(int *)(c + 0x5a8) = 0xa;
    func_ov063_0211adfc(c);
    func_ov063_02119c58(c);
    if (func_ov063_0211ad00(c) != 0) {
        if (*(int *)(c + 0x5a4) != 2) *(unsigned char *)(c + 0x5cc) = 6;
        else *(unsigned char *)(c + 0x5cc) = 1;
    }
    *(int *)(c + 0x584) = 0x1000;
    *(unsigned char *)(c + 0x5c9) = 0xff;
}

// @symbol func_ov063_02119c18
extern "C" void func_ov063_02119c18(void* c, unsigned int id) {
    void* r = (void*)*(int*)((char*)c + 0x498);
    if (r != 0) return;
    void* a = _ZN8dActor_c15FindWithActorIDEjPS_(id, 0);
    *(void**)((char*)c + 0x48c) = a;
    a = *(void**)((char*)c + 0x48c);
    if (a != 0) {
        *(void**)((char*)c + 0x498) = *(void**)((char*)a + 4);
    }
}

// @symbol func_ov063_02119c58
void func_ov063_02119c58(char *c) {
    unsigned int id;
    dActor_c *a;
    if (*(int *)(c + 0x490) != 0) return;
    id = 0xd2;
    a = 0;
    for (;;) {
        a = (dActor_c *)_ZN8dActor_c15FindWithActorIDEjPS_(id, a);
        if (a == 0) return;
        unsigned char t = *((unsigned char *)a + 0x5cf);
        if (t == 0xe) break;
        if (t == 0xd) break;
    }
    *(int *)(c + 0x490) = *(int *)((char *)a + 4);
}

// @symbol func_ov063_02119cc0
extern "C" void func_ov063_02119cc0(char *c, int unused, s16 a2, int a3)
{
    if (func_ov063_0211a8a4(c) == 0) goto reset;

    if (Vec3_HorzDist((Vec3 *)(c + 0x5c), (Vec3 *)(c + 0x51c)) >= *(int *)(c + 0x59c)) {
        *(s16 *)(c + 0x5bc) = Vec3_HorzAngle((Vec3 *)(c + 0x5c), (Vec3 *)(c + 0x51c));
        *(u16 *)(c + 0x100) = 0;
    } else if (Vec3_HorzDist((Vec3 *)(c + 0x51c), (Vec3 *)(*(char **)(c + 0x484) + 0x5c)) < *(int *)(c + 0x59c)) {
        *(s16 *)(c + 0x5bc) = *(s16 *)(c + 0x5b0);
        *(u16 *)(c + 0x100) = 0;
    } else {
        unsigned rnd = (unsigned)RandomIntInternal(&data_0209e650) >> 16 & 0x3f;
        if (*(u16 *)(c + 0x100) >= rnd + 0x5a) {
            int rnd2 = (int)((unsigned)RandomIntInternal(&data_0209e650) >> 16 & 0x3fff) - 0x2000;
            *(s16 *)(c + 0x5bc) = *(s16 *)(c + 0x94) + rnd2;
            *(u16 *)(c + 0x100) = 0;
        }
    }

    ApproachLinear(*(short *)(c + 0x94), *(s16 *)(c + 0x5bc), a2);
    *(int *)(c + 0xa8) = 0;
    func_ov063_0211a030((struct C *)c, 0xa000 - *(int *)(c + 0x588), a3);

    if (*(int *)(c + 0x98) != 0) {
        func_ov063_0211a964(c, 0);
    } else {
        func_ov063_0211a960(c);
    }
    return;

reset:
    *(int *)(c + 0x98) = 0;
    *(int *)(c + 0xa8) = 0;
    func_ov063_0211a960(c);
}

// @symbol func_ov063_02119e38
extern "C" void func_ov063_02119e38(char *thiz, int a1, short a2, int a3) {
  int thresh;
  short angle;

  if (func_ov063_0211a8a4(thiz) != 0) {
    if ((unsigned int)(*(unsigned short*)(thiz + 0x500 + 0xd4) << 0x17) >> 0x1f) {
    } else {
      if (*(unsigned short*)(thiz + 0x500 + 0xbe) != 0) {
        unsigned short *q = (unsigned short*)(thiz + 0x5be);
        *q = *q - 1;
      }
    }
    if (*(unsigned short*)(thiz + 0x500 + 0xc4) != 0) {
      unsigned short *q = (unsigned short*)(thiz + 0x5c4);
      *q = *q - 1;
    }

    {
      unsigned char st = *(unsigned char*)(thiz + 0x5cf);
      if (st == 0xf || st == 9)
        thresh = 0x7fffffff;
      else
        thresh = 0x5dc000;
    }

    if (data_0209f2f8 == 3 &&
        Vec3_HorzDist((Vec3*)(thiz + 0x51c), (Vec3*)(*(char**)(thiz + 0x484) + 0x5c)) > thresh) {
      angle = Vec3_HorzAngle((Vec3*)(thiz + 0x5c), (Vec3*)(thiz + 0x51c));
    } else {
      if (Vec3_HorzDist((Vec3*)(thiz + 0x5c), (Vec3*)(*(char**)(thiz + 0x484) + 0x5c)) <= thresh)
        angle = *(short*)(thiz + 0x5b0);
      else
        angle = Vec3_HorzAngle((Vec3*)(thiz + 0x5c), (Vec3*)(thiz + 0x51c));
    }

    ApproachLinear(*(short *)(thiz + 0x94), angle, a2);
    *(int*)(thiz + 0xa8) = 0;

    {
      char *p = *(char**)(thiz + 0x484);
      if (*(unsigned char*)(p + 0x6de) == 0) {
        int myY = *(int*)(thiz + 0x60);
        int otherY = *(int*)(p + 0x60);
        int dy = myY - otherY;
        if ((a1 << 0xc) < dy && dy < 0x1f4000 &&
            (*(int*)(thiz + 0x520) - myY) < 0xfa000) {
          *(int*)(thiz + 0xa8) = func_ov063_0211a0a8((int)thiz, myY, otherY, 0xa000, 0x2000);
        }
      }
    }

    func_ov063_0211a030((struct C *)thiz, 0xa000 - *(int*)(thiz + 0x588), a3);
    if (*(int*)(thiz + 0x98) != 0) {
      func_ov063_0211a964(thiz, 0);
      return;
    }
    func_ov063_0211a960(thiz);
    return;
  }

  *(int*)(thiz + 0x98) = 0;
  *(int*)(thiz + 0xa8) = 0;
  func_ov063_0211a960(thiz);
}

// @symbol func_ov063_0211a030
extern "C" void func_ov063_0211a030(struct C* c, int a, int b)
{
    int h = b >> 1;
    int pv = c->p484->f98;
    int m = (int)(((long long)a * h + 0x800) >> 12);
    if (pv < m) {
        c->f98 = m;
        return;
    }
    c->f98 = (int)(((long long)pv * h + 0x800) >> 12);
}

// @symbol func_ov063_0211a0a8
extern "C" int func_ov063_0211a0a8(int a0, int a1, int a2, int a3, int a4) {
    int diff = a1 - a2;
    if (diff > 0) {
        if (diff < a3) return 0;
        return -a4;
    } else {
        a3 = -a3;
        if (diff > a3) return 0;
        return a4;
    }
}

// @symbol func_ov063_0211a0dc
extern "C" int func_ov063_0211a0dc(char* c)
{
    void* r4;
    u32 id;
    Vec3 v1, v2;

    id = *(u32*)(c + 0x1a8);
    if (id == 0)
        goto ret0;

    if (*(s32*)(c + 0x1a4) & 0x207e0) {
        void* found;

        *(u32*)(c + 0x19c) |= 1;
        found = _ZN8dActor_c10FindWithIDEj(*(u32*)(c + 0x1a8));
        if (found) {
            *(void**)(c + 0x488) = found;

            {
                int isBf = (int)(*(u16*)((char*)found + 0xc) == 0xbf);
                if (isBf) {
                    if (*(s32*)((char*)found + 8) == 3) {
                        u16* p = (u16*)(c + 0x5d4);
                        *p |= 0x40;
                    }
                }
            }
        }
        return 1;
    }

    r4 = _ZN8dActor_c10FindWithIDEj(id);
    if (!r4)
        goto ret0;
    if (!(*(s32*)(c + 0x1a4) & 0x400000))
        goto ret0;
    if (*(u8*)((char*)r4 + 0x6fb) != 0)
        return 0;

    if (*(u8*)((char*)r4 + 0x6f9) != 0) {
        *(u32*)(c + 0x19c) |= 1;
        *(void**)(c + 0x488) = _ZN8dActor_c10FindWithIDEj(*(u32*)(c + 0x1a8));
        return 1;
    }

    if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(c, c + 0x184, r4) != 0) {
        _ZN6Player6BounceE5Fix12IiE(r4, 0x28000);
        func_0201267c(0x149, c + 0x74);
        return -1;
    }

    {
    int isKind_d2 = (int)(*(u16*)(c + 0xc) == 0xd2);
    if (isKind_d2) {
        if (*(u8*)((char*)r4 + 0x6de) != 0) {
            _ZN6Player6BounceE5Fix12IiE(r4, 0x28000);
            func_0201267c(0x149, c + 0x74);
            return -1;
        }

        v1.x = *(s32*)(c + 0x5c);
        v1.y = *(s32*)(c + 0x60);
        v1.z = *(s32*)(c + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r4, &v1, *(u32*)(c + 0x5ac), 0xc000, 1, 0, 1);
        func_0201267c(0x149, c + 0x74);
        return -1;
    }
    }

    if (*(s32*)(c + 0x1a4) & 0x10) {
        u8 kind = *(u8*)(c + 0x5cf);

        if (kind < 2)
            goto found_kind;
        if (kind >= 6)
            goto found_kind;
        if (kind > 9)
            goto no_kind;

found_kind:
        {
            void* found2;
            s16 v[3];

            *(u32*)(c + 0x19c) |= 1;
            found2 = _ZN8dActor_c10FindWithIDEj(*(u32*)(c + 0x1a8));
            if (!found2)
                goto ret0;
            *(void**)(c + 0x488) = _ZN8dActor_c10FindWithIDEj(*(u32*)(c + 0x1a8));

            v[0] = -0x2000;
            v[1] = 0;
            v[2] = 0;
            _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(c, v, found2, 0);

            {
                s16* ap = (s16*)(c + 0x8e);
                *ap = (s16)(*ap + 0x8000);
            }
            return 1;
        }
    }

no_kind:
    v2.x = *(s32*)(c + 0x5c);
    v2.y = *(s32*)(c + 0x60);
    v2.z = *(s32*)(c + 0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r4, &v2, *(u32*)(c + 0x5ac), 0xc000, 1, 0, 1);
    func_0201267c(0x149, c + 0x74);
    return -1;

ret0:
    return 0;
}

// @symbol func_ov063_0211a3d0
extern "C" int func_ov063_0211a3d0(char* c)
{
    Vec3 v, w;

    if (*(u16*)(c + 0x100) == 0) {
        *(s32*)(c + 0x98) = 0x28000;
        *(s16*)(c + 0x94) = *(s16*)(*(char**)(c + 0x484) + 0x8e);
        *(s32*)(c + 0x5a0) = 1;
        *(u16*)(((int)c + 0x5d4)) &= ~4;
        goto fall;
    }

    if (*(u16*)(c + 0x100) == 5)
        *(u8*)(c + 0x5c9) = 8;

    if (*(u16*)(c + 0x100) <= 0x1e) {
        if (_ZNK10dBgCh_Actr8IsOnWallEv(c + 0x1c4) == 0)
            goto fall;
    }

    if (*(u8*)(c + 0x5cf) == 0xf) {
        int x = *(s32*)(c + 0x5c);
        v.x = x;
        v.y = *(s32*)(c + 0x60);
        v.z = *(s32*)(c + 0x64);
        if ((u32)(*(u16*)(c + 0x5d4) << 23) >> 31)
            v.x = x * (u32)-1;
        ((int*)&w)[0] = ((int*)&v)[0];
        ((int*)&w)[1] = ((int*)&v)[1];
        ((int*)&w)[2] = ((int*)&v)[2];
        _ZN8dActor_c10PoofDustAtERK7Vector3(c, &w);
    } else {
        _ZN8dActor_c8PoofDustEv(c);
    }

    *(s32*)(c + 0x5a0) = 2;

    if (*(u8*)(c + 0x5cf) != 0 && *(u32*)(c + 0x490) != 0) {
        *(void**)(c + 0x48c) = _ZN8dActor_c10FindWithIDEj(*(u32*)(c + 0x490));
        if (*(void**)(c + 0x48c) != 0) {
            if (*(u8*)(c + 0x5cf) != 2) {
                *(s32*)(((int)*(char**)(c + 0x48c) + 0x180)) += 1;
            }
            func_ov063_02116244(*(char**)(c + 0x48c));
        }
        *(void**)(c + 0x48c) = 0;
    }
    return 1;

fall:
    *(s32*)(c + 0xa8) = 0x5000;
    *(s16*)(((int)c + 0x90)) += 0x800;
    *(s16*)(((int)c + 0x8e)) += 0x800;
    return 0;
}

// @symbol func_ov063_0211a564
extern "C" int func_ov063_0211a564(char *c, int arg1)
{
    func_ov063_0211adfc(c);
    if (*(u16 *)(c + 0x100) == 0)
        func_ov063_0211a810(c, 1);
    {
        u16 v = *(u16 *)(c + 0x100);
        if (v < 0x20) {
            int result = ((arg1 * data_ov063_0211e7e0[v]) << 12) / 5000;
            func_ov063_0211a76c(c, 1, result);
            goto ret0;
        }
        if (v < 0x30) {
            func_ov063_0211a718(c);
            goto ret0;
        }
    }
    (*(int *)(((int)c + 0x19c))) &= ~1;
    func_ov063_0211a6f0(c);
    *(unsigned char *)(c + 0x5cc) = 1;
    return 1;
ret0:
    return 0;
}

// @symbol func_ov063_0211a634
extern "C" int func_ov063_0211a634(char *thiz, int arg)
{
    unsigned int idx;
    int *pf;
    int rv;
    func_ov063_0211adfc(thiz);
    if (*(unsigned short*)(thiz + 0x100) == 0)
        func_ov063_0211a810(thiz, 0);
    idx = *(unsigned short*)(thiz + 0x100);
    if (idx < 0x20) {
        func_ov063_0211a76c(thiz, 0,
            ((arg * data_ov063_0211e7e0[idx]) << 12) / 5000);
        rv = 0;
        goto done;
    }
    pf = (int*)(((int)thiz + 0x19c));
    *pf &= ~1;
    func_ov063_0211a6f0(thiz);
    *(unsigned char*)(thiz + 0x5cc) = 1;
    return 1;
done:
    return rv;
}

// @symbol func_ov063_0211a6f0
extern "C" void func_ov063_0211a6f0(char *c)
{
    *(short *)(c + 0x94) = *(short *)(c + 0x5b4);
    *(unsigned short *)(((int)c + 0x5d4)) |= 4;
}

// @symbol func_ov063_0211a718
extern "C" void func_ov063_0211a718(char* o) {
    int a;
    s16 v;
    s16* p;
    a = (u16)(s16)((*(u16*)(o + 0x100) - 0x1f) << 13) >> 4;
    p = (s16*)(o + 0x8e);
    v = *p;
    *p = v + (data_02082214[a * 2 + 1] << 10) / 4096;
}

// @symbol func_ov063_0211a76c
extern "C" void func_ov063_0211a76c(char* c, int cond, int val)
{
    int ip = *(unsigned short*)(c + 0x100) + 1;

    ip = ((ip << 0x1b) >> 0x10);
    ip = (unsigned short)ip;
    ip = ip >> 4;
    ip = (ip << 1) + 1;

    *(int*)(c + 0x98) = val;
    *(int*)(c + 0xa8) = data_02082214[ip];
    *(short*)(c + 0x94) = *(short*)(c + 0x5b6);
    if (cond == 0) return;

    {
        short* p8e = (short*)(((int)c + 0x8e));
        *p8e = *p8e + data_ov063_0211e7e0[*(unsigned short*)(c + 0x100)];
        {
            short* p90 = (short*)(((int)c + 0x90));
            *p90 = *p90 + data_ov063_0211e7e0[*(unsigned short*)(c + 0x100)];
        }
    }
}

// @symbol func_ov063_0211a810
extern "C" void func_ov063_0211a810(char *r0, int cond)
{
    int *f = (int *)(r0 + 0x19c);
    unsigned short *h = (unsigned short *)(r0 + 0x5d4);
    *f |= 1;
    *h &= ~4;
    *(short *)(r0 + 0x5b4) = *(short *)(r0 + 0x94);
    if (cond != 0) {
        *(short *)(r0 + 0x5b6) = *(short *)(*(char **)(r0 + 0x484) + 0x8e);
        return;
    }
    {
        short base = *(short *)(r0 + 0x94);
        int d = (int)base - (int)*(short *)(r0 + 0x5b0);
        d = (short)d;
        d = (unsigned short)d;
        d >>= 4;
        {
            short t = data_02082214[(d << 1) + 1];
            if (t < 0)
                *(short *)(r0 + 0x5b6) = base;
            else
                *(short *)(r0 + 0x5b6) = (short)(base + 0x8000);
        }
    }
}

// @symbol func_ov063_0211a8a4
extern "C" int func_ov063_0211a8a4(char *thiz)
{
    dActor_c *a = (dActor_c*)thiz;
    int r6 = a->GetSubtraction(*(short*)(thiz + 0x5b0), *(short*)(thiz + 0x94));
    int r0 = a->GetSubtraction(*(short*)(thiz + 0x94),
                               *(short*)(*(char**)(thiz + 0x484) + 0x8e));
    int ret = 0;
    *(int*)(thiz + 0xa8) = 0;
    if (r6 > 0x1568 || r0 < 0x6b58) {
        if (*(unsigned char*)(thiz + 0x5c8) == 0x28) {
            *(unsigned char*)(thiz + 0x5c9) = 0xff;
            if (*(unsigned char*)(thiz + 0x5cf) != 0xf)
                func_0201267c(0xf8, thiz + 0x74);
            *(unsigned short*)(thiz + 0x5c0) = 0x1e;
        }
        if (*(unsigned char*)(thiz + 0x5c8) > 0xb4)
            ret = 1;
    } else {
        if (*(unsigned char*)(thiz + 0x5c8) == 0xff)
            *(unsigned char*)(thiz + 0x5c9) = 0x28;
    }
    return ret;
}

// @symbol func_ov063_0211a960
extern "C" void func_ov063_0211a960(char *c)
{
}

// @symbol func_ov063_0211a964
extern "C" void func_ov063_0211a964(char *c, int arg1)
{
    int t;
    if (*(u8 *)(c + 0x5c8) != 0xff) {
        if (arg1 == 0)
            return;
    }
    t = *(u16 *)(c + 0x5b8) >> 4;
    *(int *)(c + 0x80) = *(int *)(c + 0x584) +
        ((*(short *)((char *)data_02082214 + t * 4) << 3) / 100);
    t = *(u16 *)(c + 0x5b8) >> 4;
    *(int *)(c + 0x84) = *(int *)(c + 0x584) +
        ((-(*(short *)((char *)data_02082214 + t * 4) << 3)) / 100);
    *(int *)(c + 0x88) = *(int *)(c + 0x80);
    func_ov063_0211a960(c);
    *(int *)(c + 0x188) = *(int *)(c + 0x590) * *(int *)(c + 0x80);
    *(int *)(c + 0x18c) = *(int *)(c + 0x594) * *(int *)(c + 0x84);
}

// @symbol func_ov063_0211aa34
extern "C" void func_ov063_0211aa34(char* self)
{
    u8 cur = *(u8*)(self + 0x5c8);
    u8 tgt = *(u8*)(self + 0x5c9);
    int v;
    if (tgt != cur) {
        if (tgt > cur) {
            if (cur + 0x14 >= tgt) {
                *(u8*)(self + 0x5c8) = tgt;
            } else {
                u8 *p = (u8*)(self + 0x5c8);
                *p += 0x14;
            }
        } else {
            if (cur - 0x14 > tgt) {
                u8 *p = (u8*)(self + 0x5c8);
                *p -= 0x14;
            } else {
                *(u8*)(self + 0x5c8) = tgt;
            }
        }
    }
    if (*(u8*)(self + 0x5c8) == 0xff) {
        _ZN5Model12SetPolygonIDEi(self + 0x380, 1);
    } else if ((unsigned)(*(unsigned short*)(self + 0x5d4) << 23) >> 31) {
        _ZN5Model12SetPolygonIDEi(self + 0x380, 2);
    } else {
        _ZN5Model12SetPolygonIDEi(self + 0x380, 0x16);
    }
    v = *(int*)(self + 0x584) * 8 / 10
        + *(int*)(self + 0x584) * (*(u8*)(self + 0x5c8) * 2) / 2550;
    *(int*)(self + 0x80) = v;
    *(int*)(self + 0x84) = v;
    *(int*)(self + 0x88) = v;
    *(int*)(self + 0x188) = *(int*)(self + 0x590) * v;
    *(int*)(self + 0x18c) = *(int*)(self + 0x594) * v;
}

// @symbol func_ov063_0211ab68
extern "C" void func_ov063_0211ab68(char* obj) {
    Vec3 pos;
    Vec3 tmp;
    Vec3_16 rot;
    int i;
    int *p;

    if (!(data_ov063_0211edc0 & 1)) {
        /* first vec: plain fields → r4=0, r3=y, early arg loads, batch stores */
        data_ov063_0211ee74.x = 0;
        data_ov063_0211ee74.y = 0x32000;
        data_ov063_0211ee74.z = 0;
        func_020731dc(&data_ov063_0211ee74, (void *)_ZN7Vector3D1Ev, &data_ov063_0211ee20);

        p = (int *)&data_ov063_0211ee80;
        p[0] = 0xd2000;
        p[1] = 0x6e000;
        p[2] = 0xd2000;
        func_020731dc(&data_ov063_0211ee80, (void *)_ZN7Vector3D1Ev, &data_ov063_0211edfc);

        p = (int *)&data_ov063_0211ee8c;
        p[0] = -0xd2000;
        p[1] = 0x46000;
        p[2] = -0xd2000;
        func_020731dc(&data_ov063_0211ee8c, (void *)_ZN7Vector3D1Ev, &data_ov063_0211ee08);
        data_ov063_0211edc0 |= 1;
    }

    if (NumStars() < 15) {
        ((fBase_c *)obj)->MarkForDestruction();
        return;
    }

    for (i = 0; i < 3; i++) {
        Vec3_Add(&tmp, (Vec3*)(obj + 0x5c), &(&data_ov063_0211ee74)[i]);
        pos.x = tmp.x;
        pos.y = tmp.y;
        pos.z = tmp.z;
        rot.x = *(s16*)(obj + 0x92);
        rot.y = *(s16*)(obj + 0x94);
        rot.z = *(s16*)(obj + 0x96);
        rot.y = (u32)RandomIntInternal(&data_0209e650) >> 16;
        _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0xd1, 0xfff6, &pos, &rot, *(s8*)(obj + 0x5d0), -1);
    }
    ((fBase_c *)obj)->MarkForDestruction();
}

// @symbol func_ov063_0211ad00
extern "C" int func_ov063_0211ad00(char *c)
{
    int r4;
    int v;
    int b = (int)(*(unsigned short*)(c + 0xc) == 0xd2);
    if (b != 0 && *(unsigned char*)(c + 0x5cf) != 0xf) {
        r4 = 0x2e6000;
    } else {
        v = *(unsigned char*)(c + 0x5cf);
        if (v == 9) r4 = 0x7fffffff;
        else r4 = 0x5dc000;
    }
    v = *(unsigned char*)(c + 0x5cf);
    if (v == 0xd || v == 1) {
        if (func_ov063_021163d0(c) != 0) return 1;
        return 0;
    }
    if (func_ov063_0211adb4(c) == 0) {
        if (*(int*)(c + 0x580) < r4) return 1;
    }
    return 0;
}

// @symbol func_ov063_0211adb4
extern "C" int func_ov063_0211adb4(char *c) {
    unsigned char v = *(unsigned char*)(c + 0x5cf);
    if (v == 0xd || v == 1) {
        return func_ov063_021163d0(c) == 0 ? 1 : 0;
    }
    return 0;
}

// @symbol func_ov063_0211adfc
extern "C" void func_ov063_0211adfc(char *p)
{
    *(int *)(p + 0x98) = 0;
    *(int *)(p + 0xa8) = 0;
    *(int *)(p + 0x9c) = 0;
    *(short *)(p + 0x5c0) = 30;
}

// @symbol _ZN11daTBasket_c16CleanupResourcesEv
int daTBasket_c::CleanupResources()
{
    data_ov063_0211edec.Release();
    return 1;
}

// @symbol _ZN7daTrs_c16CleanupResourcesEv
int daTrs_c::CleanupResources()
{
    int b;
    int *cnt;

    if (mSpawnedActorID != 0) {
        mFoundActor = dActor_c::FindWithID(mSpawnedActorID);
        if (mFoundActor != 0)
            mFoundActor->MarkForDestruction();
        mFoundActor = 0;
    }
    if (mSpawnerID != 0) {
        mFoundActor = dActor_c::FindWithID(mSpawnerID);
        if (mFoundActor != 0) {
            cnt = (int *)(((int)mFoundActor + 0x5a0));
            (*cnt)++;
        }
        mFoundActor = 0;
    }
    if (mCarriedID == 0x122)
        UnloadBlueCoinModel(this);
    else if (mCarriedID == 0xd4)
        data_ov063_0211edec.Release();

    b = (actorID == 0xd1);
    if (b != 0) {
        data_ov063_0211edc4.Release();
        data_ov063_0211eddc.Release();
    } else {
        data_ov063_0211edf4.Release();
        data_ov063_0211ede4.Release();
        if (unk_5cf == 0xf) {
            UnloadKeyModels(3);
            data_ov063_0211edd4.Release();
            data_ov063_0211edcc.Release();
        }
    }
    UnloadCapModel();
    return 1;
}

// @symbol _ZN7daTrs_c16OnPendingDestroyEv
void daTrs_c::OnPendingDestroy()
{
}

// @symbol _ZN7daTrs_c6RenderEv
int daTrs_c::Render()
{
    int b = (int)(((mFlags & 0x40000) != 0));
    if (b != 0)
        return 1;

    {
        if (!mFlags_5d4.b3)
            return 1;
        if (mFlags_5d4.b1) {
            mBodyModel.Render((const Vector3 *)&mBodyScaleX);
        }
        RenderCapModel(0);
    }

    if (mOpacity < 8)
        return 1;

    {
        unsigned char st = unk_5cf;
        if (st >= 0xc && st != 0xf)
            mModelAnim.HideMaterial(0, 2);
    }

    if (mDeathState != 8 &&
        (unk_5cc == 3 ||
         unk_5cc == 3 ||
         unk_5cc == 3 ||
         unk_5cc == 3)) {
        mModelAnim.Model::Render((const Vector3 *)&mScaleX);
    } else {
        mModelAnim.Render((const Vector3 *)&mScaleX);
    }

    return 1;
}

// @symbol _ZN11daTBasket_c6RenderEv
int daTBasket_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN7daTrs_c8BehaviorEv
int daTrs_c::Behavior()
{
    char *c = (char *)this;
    Vector3 pv;
    Vector3 v1;
    Vector3 v2;
    Vector3 ve;
    int t;
    void *p;
    char *q;
    char *r1;
    char *r2;
    s32 *p19c;
    u16 *fp;
    u8 *bp;
    s32 y;
    s32 w;
    s32 z;
    s32 x;
    int d1;
    int v;

    func_0200f760(c, &mdCcAcPos_c);
    if (mSpawnedActorID != 0) {
        mFoundActor = dActor_c::FindWithID(mSpawnedActorID);
        q = (char *)mFoundActor;
        if (q != 0) {
            *(s32 *)(q + 0x5c) = mPosX;
            *(s32 *)(q + 0x60) = mPosY;
            *(s32 *)(q + 0x64) = mPosZ;
        }
        mFoundActor = 0;
    }
    if (_ZN11dCapEnemy_c11GetCapStateEv(c) == 0)
        return 1;

    t = UpdateKillByInvincibleChar(mWithMeshClsn, mModelAnim, 0);
    if (t != 0) {
        if (t == 2) {
            PoofDust();
            if (mDeathMode != 0) {
                KillAndTrackInDeathTable();
                func_0201267c(0xd5, &mCamSpacePosX);
                if (((mCapId) & 0xf) < 6) {
                    mPosX = mHomePosX;
                    mPosY = mHomePosY;
                    mPosZ = mHomePosZ;
                    mAreaId = mAreaIdx;
                    mPrevAngleX = mHomeAngleX;
                    mPrevAngleY = mHomeAngleY;
                    mPrevAngleZ = mHomeAngleZ;
                    {
                        s16 (*ap)[3] = (s16 (*)[3])(c + 0x92);
                        mAngleX = (*ap)[0];
                        mAngleY = (*ap)[1];
                        mAngleZ = (*ap)[2];
                    }
                    p = RespawnIfHasCap();
                    if (p != 0) {
                        fp = (u16 *)((char *)p + 0x5d4);
                        *fp = (*fp) & (~2);
                    }
                }
            } else {
                unk_5cc = 5;
                mSubState = 0;
                p19c = (s32 *)(c + 0x19c);
                *p19c = (*p19c) | 1;
                fp = (u16 *)(c + 0x5d4);
                *fp = (*fp) & (~8);
            }
        }
        return 1;
    }

    mAreaId = -1;
    if (mSoundCount == 0) {
        /* b7 clear: not hidden yet. */
        if ((IsAreaShowing(mAreaIdx) == 0) || (func_ov063_02116190(c) != 0)) {
            if (mFlags_5d4.b7 == 0) {
                r2 = (char *)&mPrevAngleX;
                mPosX = mHomePosX;
                fp = (u16 *)(c + 0x5d4);
                mPosY = mHomePosY;
                mPosZ = mHomePosZ;
                mPrevAngleX = mHomeAngleX;
                mPrevAngleY = mHomeAngleY;
                mPrevAngleZ = mHomeAngleZ;
                mAngleX = *(s16 *)r2;
                mAngleY = *(s16 *)(r2 + 2);
                mAngleZ = *(s16 *)(r2 + 4);
                *fp = (*fp) | 0x10;
                unk_5cc = 0;
                mHorzSpeed = 0;
                mVertAccel = 0;
                mVertSpeed = 0;
                mSubState = 0;
                mAreaId = mAreaIdx;
            }
            return 1;
        }
    }
    Unk_02005d94();
    t = UpdateYoshiEat(mWithMeshClsn);
    if (t != 0) {
        if (t == 1) {
            ve.x = mCapPosX;
            ve.y = mCapPosY;
            ve.z = mCapPosZ;
            if (GetCapEatenOffIt(ve) != 0)
                return 1;
        }
        mVertAccel = -0x2000;
        func_ov063_02119ab0(c);
        if (mEatenByYoshi != 0) {
            u16 *p100 = (u16 *)&mStateTimer;
            if (p100[2] == 0) {
                mEatenByYoshi = 0;
                ((u16 *)&mStateTimer)[2] = 0;
                mVertAccel = 0;
                mVertSpeed = 0;
                goto block_39;
            }
        }
        v = (((mFlags) & 0x40000) ? 1 : 0);
        if (v != 0) {
            u8 st = mTalkStep;
            Player *pl = (Player *)mEatingPlayer;
            switch (st) {
            case 0:
                if (pl->ShowMessage(*this, 0x15a, 0, 0, 2) != 0) {
                    bp = (u8 *)(c + 0x5d1);
                    *bp = (*bp) + 1;
                    func_0201267c(0xf8, &mCamSpacePosX);
                }
                break;
            case 1:
                if (pl->GetTalkState() == -1) {
                    pl->DropActor();
                    bp = (u8 *)(c + 0x5d1);
                    *bp = (*bp) + 1;
                }
                break;
            }
        }
        func_ov063_021166ac(c);
        mdCcAcPos_c.Clear();
        return 1;
    }
block_39:
    mTalkStep = 0;

    {
        mClosestPlayer = ClosestPlayer();
        Player *plr = mClosestPlayer;
        if (plr != 0) {
            s32 *pp = (s32 *)(((char *)plr) + 0x5c);
            pv.x = pp[0];
            pv.y = pp[1];
            pv.z = pp[2];
            /* (Vector3 *)&mPosX pun: no shared overlay accessor exists. */
            mAngleToPlayer = Vec3_HorzAngle((const Vector3 *)&mPosX, &pv);
            mDistToPlayer = Vec3_HorzDist((const Vector3 *)&mPosX, &pv);
        } else {
            r1 = c + 0x500;
            *(s16 *)(r1 + 0xb0) = mAngleY;
            mDistToPlayer = 0x2710000;
        }
    }
    mPrevState = unk_5cc;
    switch (unk_5cf) {
    case 0:
    case 1:
    case 2:
        func_ov063_021192d4(c);
        break;
    case 3:
        func_ov063_02116bf4(c);
        break;
    case 4:
        func_ov063_02116a1c(c);
        break;
    case 5:
        func_ov063_02116d38(c);
        break;
    case 6:
    case 10:
        func_ov063_021192d4(c);
        break;
    case 7:
        func_ov063_0211ab68(c);
        break;
    case 12:
    case 13:
    case 14:
        func_ov063_021189f4(c);
        break;
    case 15:
        func_ov063_021172a8(c);
        break;
    case 8:
        func_ov063_02119274(c);
        break;
    case 9:
        func_ov063_021192d4(c);
        break;
    case 11:
        func_ov063_02116fac(c);
    }

    /* b2 set: copy the yaw through. */
    if (mFlags_5d4.b2 != 0)
        mAngleY = mPrevAngleY;
    *(u16 *)&mStateTimer += 1;
    DecIfAbove0_Short(&mTimer5c0);
    if (mPrevState != unk_5cc)
        mStateTimer = 0;
    if (unk_5cf != 3) {
        UpdatePos(&mdCcAcPos_c);
        func_ov063_02119ab0(c);
        /* b0 set: clamp z. */
        if ((mFlags_5d4.b0 != 0) && (mPosZ < -0x12c000))
            mPosZ = -0x12c000;
        dBgCh_Gnd rc1;
        y = mPosY;
        z = mPosZ;
        w = y + 0x32000;
        x = mPosX;
        v1.x = x;
        v1.y = w;
        v1.z = z;
        rc1.SetObjAndPos(v1, (dActor_c*)this);
        if (rc1.DetectClsn() != 0) {
            s32 ground = rc1.clsnY + 0x2000;
            if (mPosY < ground)
                mPosY = ground;
        }
        if ((unk_5cf != 4) && (unk_5cf != 0xb)) {
            dBgCh_Gnd rc2;
            y = mPosY;
            z = mPosZ;
            w = y + 0x32000;
            x = mPosX;
            v2.x = x;
            v2.y = w;
            v2.z = z;
            rc2.SetObjAndPos(v2, (dActor_c*)this);
            d1 = (int)(actorID == 0xd1);
            if (d1 != 0) {
                if (unk_5cf < 8) {
                    /* b5 set: ground already found. */
                    if (mFlags_5d4.b5 != 0) {
                        if ((rc2.DetectClsn() == 0) ||
                            ((mPosY - rc2.clsnY) > 0x12c000)) {
                            mPosX = mLastGroundPosX;
                            mPosY = mLastGroundPosY;
                            mPosZ = mLastGroundPosZ;
                        } else {
                            mLastGroundPosX = mPosX;
                            mLastGroundPosY = mPosY;
                            mLastGroundPosZ = mPosZ;
                        }
                    } else {
                        goto ray_e;
                    }
                } else {
                    goto ray_e;
                }
            } else {
            ray_e:
                if ((rc2.DetectClsn() != 0) &&
                    ((mPosY - rc2.clsnY) < 0x12c000)) {
                    fp = (u16 *)(c + 0x5d4);
                    *fp = (*fp) | 0x20;
                    mLastGroundPosX = mPosX;
                    mLastGroundPosY = mPosY;
                    mLastGroundPosZ = mPosZ;
                }
            }
            UpdateWMClsn(mWithMeshClsn, 0);
        }
    }
    if ((((unk_5cc != 3) && (unk_5cc != 3)) && (unk_5cc != 3)) && (unk_5cc != 3))
        mModelAnim.Advance();
    func_ov063_021166ac(c);
    mdCcAcPos_c.Clear();
    if (mOpacity == 0xff) {
        mdCcAcPos_c.SetPosRelativeToActor(*(const Vector3 *)&mClsnOffX);
        mdCcAcPos_c.Update();
    }
    return 1;
}


// @symbol _ZN11daTBasket_c8BehaviorEv
int daTBasket_c::Behavior()
{
    int onGround = 0;
    int secretDone = 1;

    if (mMuteSecretSound == 0)
        secretDone = Sound::PlaySecretSound((dActor_c *)this, (u16 *)&mSoundTimer);

    if (mWithMeshClsn.JustHitGround()) {
        int vertSpeed = mVertSpeed;
        mVertSpeed = (-vertSpeed) >> 1;
        LandingDust(false);
    } else if (mWithMeshClsn.IsOnGround()) {
        onGround = 1;
        if (secretDone != 0 || (u16)mSoundTimer > 0x3c) {
            unsigned int id = mdCcAc_c.otherOwner;
            if (id != 0) {
                dActor_c *touched = dActor_c::FindWithID(id);
                if (touched != 0) {
                    if ((mdCcAc_c.hitFlags & 0x400000) != 0)
                        ((Player *)touched)->JumpIntoBooCage(*(Vector3 *)&mPosX);
                }
            }
        }
    }

    if (onGround == 0) {
        int z = mPosZ;
        int y = mPosY;
        int x = mPosX;
        unsigned int pid = (unsigned int)mParticleID;
        mParticleID = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            pid, 0x119, x, y + 0x64000, z, 0, 0);
    }

    UpdatePos(0);
    UpdateWMClsn(mWithMeshClsn, 0);
    func_ov063_021169c4((char *)this);
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN7daTrs_c13InitResourcesEv

int daTrs_c::InitResources()
{
    char *c = (char *)this;
    int cond;
    int tmp598;
    dActor_c *spawned;
    Player *pp;

    *(u16 *)&mFlags_5d4 = 0;
    mSpawnedActorID = 0;
    cond = 0;
    if (actorID == 0xd2) cond = 1;
    if (cond) {
        unk_5cf = (param1 & 0xf) + 0xc;
        if (unk_5cf == 0xf) {
            LoadKeyModels(3);
            Animation::LoadFile(data_ov063_0211edd4);
            Animation::LoadFile(data_ov063_0211edcc);
        } else if (unk_5cf == 0xc) {
            mFoundActor = dActor_c::Spawn(0xd3, param1, *(const Vector3 *)&mPosX, 0, mAreaIdx, -1);
            if (mFoundActor != 0) {
                mSpawnedActorID = mFoundActor->uniqueID;
            }
            mFoundActor = 0;
        }
        Animation::LoadFile(data_ov063_0211ede4);
        mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov063_0211edf4), 1, 1);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(BCA_File **)((char *)&data_ov063_0211ede4 + 4), 0, 0x1000, 0);
        mDataIdx = 3;
        mClsnRadius = 0xc8;
        mClsnHeight = 0x104;
        mClsnZBias = -0x14000;
        tmp598 = mClsnZBias;
        mClsnOffX = 0;
        mClsnOffY = 0;
        mClsnOffZ = tmp598;
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &mClsnOffX, mClsnRadius << 0xc, mClsnHeight << 0xc, 0x200000, 0x207e0);
        if (unk_5cf != 0xf) {
            _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0xdc000, 0xdc000, 0, 0);
        } else {
            _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0xc8000, 0xb4000, 0, 0);
        }
    } else {
        unk_5cf = param1 & 0xf;
        Animation::LoadFile(data_ov063_0211eddc);
        mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov063_0211edc4), 1, 0x16);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(BCA_File **)((char *)&data_ov063_0211eddc + 4), 0, 0x1000, 0);
        mDataIdx = 1;
        if (unk_5cf == 5) {
            mClsnZBias = -0x24000;
            tmp598 = mClsnZBias;
            mClsnOffX = 0;
            mClsnOffY = 0;
            mClsnOffZ = tmp598;
            mClsnRadius = 0x43;
            mClsnHeight = 0x5a;
        } else {
            mClsnZBias = -0x14000;
            tmp598 = mClsnZBias;
            mClsnOffX = 0;
            mClsnOffY = 0;
            mClsnOffZ = tmp598;
            mClsnRadius = 0x4a;
            mClsnHeight = 0x64;
        }
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &mClsnOffX, mClsnRadius << 0xc, mClsnHeight << 0xc, 0x200000, 0x207e0);
        if (data_0209f2f8 == 0xc && mPosX == 0xbb8000 && mAreaId == 2) {
            mFlags_5d4.b0 = 1;
        }
        _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);
    }

    mCarriedID = 0x187;
    if (unk_5cf == 5) {
        mFlags_5d4.b1 = 1;
        mCarriedID = 0xd4;
        Model::LoadFile(data_ov063_0211edec);
    } else if (unk_5cf == 0 || unk_5cf == 1 || unk_5cf == 2 || unk_5cf == 6 || (unsigned)(unsigned char)(unk_5cf + 0xf8) <= 3) {
        mdCcAcPos_c.vulnFlags |= 0x8000;
        mFlags_5d4.b1 = 1;
        if (unk_5cf == 6) {
            mCarriedID = 0x120;
        } else if ((unsigned)(unsigned char)(unk_5cf + 0xf6) <= 1) {
            mCarriedID = 0x121;
        } else {
            mCarriedID = 0x122;
            LoadBlueCoinModel(this);
        }
    }

    mCapId = 6;
    mCapPosX = 0;
    mCapPosY = 0x60000;
    mCapPosZ = 0;
    cond = 0;
    if (actorID == 0xd1) cond = 1;
    if (cond && unk_5cf != 8) {
        unsigned char capIdx;
        mHadBank1Cap = (param1 >> 0xc) & 0xf;
        capIdx = (param1 >> 8) & 0xf;
        AddCap(capIdx);
        if ((mCapId & 7) < 6) {
            /* int on the store side only: spelling both sides identically
               lets mwccarm CSE the field address (addlt r2,r4,#8 + [r2]),
               one instruction the ROM does not have -- it wants [r4,#8] direct.
               Same lever as daKrb_c::OnTurnIntoEgg in src/actors/daKrb_c.cpp. */
            *(int *)((char *)this + 8) = param1 & 0xfff;
        }
        if (DestroyIfCapNotNeeded() == 0) {
            return 0;
        }
    }

    if ((unsigned)(unsigned char)(unk_5cf + 0xf6) <= 1) {
        if ((unsigned)NumStars() < 3) {
            MarkForDestruction();
            return 0;
        }
        if (unk_5cf == 0xb && (unsigned)NumStars() >= 0xf) {
            MarkForDestruction();
            return 0;
        }
        if (IsStarCollectedInCurLevel(1) != 0) {
            mCarriedID = 0x120;
        }
    }

    if (mShadowModel1.InitCylinder() == 0) return 0;
    if (mShadowModel2.InitCylinder() == 0) return 0;

    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mHomeAngleX = mAngleX;
    mHomeAngleY = mAngleY;
    mHomeAngleZ = mAngleZ;
    mStateTimer = 0;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mOpacity = 0xff;
    func_02035800(&mWithMeshClsn);
    unk_5cc = 0;
    unk_5b2 = mPrevAngleY;
    unk_5b8 = 0;
    mFlags_5d4.b2 = 1;
    mBigBooID = 0;
    mSpawnerID = 0;
    mCachedActorID = 0;
    mTalkPlayer = 0;
    mChildDeaths = 0;
    unk_5b8 = 0;
    mDeathMode = data_ov063_0211e22c[unk_5cf];
    mHurtDamage = data_ov063_0211e1ec[unk_5cf];
    unk_5a8 = 0;
    mFlags_5d4.b3 = 1;
    mTimer180 = 0;
    mTerminalVelocity = -0x3c000;
    mAreaIdx = mAreaId;
    mSoundCount = 0;
    mTimer5c2 = 0;
    mTimer5c4 = 0;
    mLeashDist = ((param1 >> 8) & 0xff) * 0x64000;
    unk_5d3 = 0;
    mBodyScaleX = 0xc00;
    mBodyScaleY = 0xc00;
    mBodyScaleZ = 0xc00;

    if (unk_5cf == 5) {
        if ((unsigned)NumStars() < 0xf) {
            MarkForDestruction();
            return 1;
        }
        if (data_0209f264 == 0) {
            spawned = dActor_c::Spawn(mCarriedID, 0, *(const Vector3 *)&mPosX, 0, mAreaIdx, -1);
            if (spawned != 0) {
                /* A byte on the carried actor (0xd4). */
                *(unsigned char *)((char *)spawned + 0x37e) = 1;
                MarkForDestruction();
                return 1;
            }
        }
        mBodyModel.SetFile((BMD_File *)*(void **)((char *)&data_ov063_0211edec + 4), 1, -1);
    } else if (unk_5cf == 0 || unk_5cf == 1 || unk_5cf == 2 || unk_5cf == 6 || (unsigned)(unsigned char)(unk_5cf + 0xf8) <= 3) {
        unsigned short t = mCarriedID;
        if (t == 0x122) {
            mBodyModel.SetFile((BMD_File *)*(void **)((char *)&data_ov002_0210d9c8 + 4), 1, -1);
        } else if (t == 0x121) {
            mBodyModel.SetFile((BMD_File *)*(void **)((char *)&data_ov002_0210d9f8 + 4), 1, -1);
        } else {
            mBodyModel.SetFile((BMD_File *)*(void **)((char *)&data_ov002_0210d9b8 + 4), 1, -1);
        }
        if (unk_5cf == 0xb) {
            mAreaId = -1;
            pp = ClosestPlayer();
            if (pp != 0 && pp->mPosZ > (int)0xffaec000) {
                mFlags_5d4.b4 = 1;
            }
        }
    } else {
        if ((unsigned)unk_5cf < 0xc) {
            mdCcAcPos_c.flags |= 1;
        }
        if (unk_5cf == 4) {
            mAreaId = -1;
            pp = ClosestPlayer();
            if (pp != 0 && pp->mPosZ > (int)0xffaec000) {
                mFlags_5d4.b4 = 1;
            }
        } else if (unk_5cf == 7) {
            mFlags_5d4.b1 = 1;
            mCarriedID = 0x120;
        } else if (unk_5cf == 0xe) {
            mAreaId = -1;
        }
    }

    mClsnBaseX = 0;
    mClsnBaseY = 0;
    mClsnBaseZ = 0;
    Matrix4x3_FromTranslation(&data_020a0e68, mPosX, mPosY, mPosZ);
    MulMat4x3Mat4x3(*(Matrix4x3 **)((char *)this + 0x394), &data_020a0e68, &data_020a0e68);
    mClsnBaseX = data_020a0e68.m[9];
    mClsnBaseY = data_020a0e68.m[10];
    mClsnBaseZ = data_020a0e68.m[11];
    SubVec3((Vector3 *)&mClsnBaseX, (Vector3 *)&mPosX, (Vector3 *)&mClsnBaseX);
    mTalkStep = 0;
    mTargetAngleY = mPrevAngleY;
    mParticle1 = 0;
    mParticle0 = mParticle1;
    return 1;
}


// @symbol _ZN11daTBasket_c13InitResourcesEv
int daTBasket_c::InitResources()
{
    BMD_File *bmd = (BMD_File *)Model::LoadFile(data_ov063_0211edec);
    if (mModel.SetFile(bmd, 1, -1) == 0)
        return 0;
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    mVertAccel = -0x4000;
    mTerminalVelocity = -0x46000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x64000, 0x64000, 0x200004, 0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x64000, 0x64000, 0, 0);
    mWithMeshClsn.SetLimMovFlag();
    mParticleID = 0;
    mSoundTimer = 0;
    mMuteSecretSound = 0;
    return 1;
}

// @symbol _ZN11daTrsIcon_c13InitResourcesEv
int daTrsIcon_c::InitResources()
{
    mStarID = (param1 >> 8) & 0xf;
    mTrackStarID = -1;
    mTrackStarID = TrackStar(mStarID, 2);
    return 1;
}

// @symbol _ZN7daTrs_c13OnYoshiTryEatEv
int daTrs_c::OnYoshiTryEat() {
    unsigned short v = actorID;
    int r;
    if (v == 0xd1) r = 1; else r = 0;
    if (r != 0) r = 7; else r = 0;
    return r;
}

// @symbol daTrsIcon_c_classInit
extern "C" daTrsIcon_c *daTrsIcon_c_classInit()
{
    return new daTrsIcon_c();
}

// @symbol daTBasket_c_classInit
extern "C" daTBasket_c *daTBasket_c_classInit()
{
    return new daTBasket_c();
}

// @symbol daTrs_c_classInit_BOSS_TERESA
extern "C" daTrs_c *daTrs_c_classInit_BOSS_TERESA()
{
    return new daTrs_c();
}

// @symbol daTrs_c_classInit_TERESA
extern "C" daTrs_c *daTrs_c_classInit_TERESA()
{
    return new daTrs_c();
}

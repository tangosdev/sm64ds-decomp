//cpp
/* daFPkn_c -- the fire piranha plant (PAKUN2 / FIREPAKUN / FIREPAKUN_S).
 * ov084 .text 0x0212d248..0x0212ea18, eighteen functions: D1, D0, nine
 * state helpers, then the seven vtable methods.
 *
 * NAME: daFPkn_c is the cartridge's RTTI spelling. The word before the
 * vtable address point 0x02130b28 (0x02130b24) relocates to _ZTI8daFPkn_c
 * at 0x02130ac0, which reads [__si_class_type_info, _ZTS8daFPkn_c
 * (0x02130ab4, "8daFPkn_c"), _ZTI12dEnemyBase_c (ov002 0x021081c0)]. The
 * class carried the coined name FirePiranhaPlantBig until this change.
 *
 * THE DESTRUCTOR IS THE KEY FUNCTION. It is defined first under
 * `#pragma defer_codegen off`, so mwccarm emits each function as it is
 * parsed and the file is written in ROM order: D1 (0x0212d248) and D0
 * (0x0212d288) first, then a D2 the cartridge has no home for (licensed
 * as deadstrip), and this object carries _ZTV8daFPkn_c and the RTTI chain
 * as vague linkage. The ROM keeps the table; the promotion is text-only.
 *
 * The run's left neighbour is daRedBombhei_c_classInit (0x0212d200), another
 * class's factory. The three daFPkn_c classInit factories start at
 * 0x0212ea18 and stay one-function C sources.
 */

#include "decl_common.h"
#include "daFPkn_c.h"
#include "SharedFilePtr.h"
#include "Player.h"

#pragma defer_codegen off

/* Second word is the loaded BCA. Same two words as daPkn_c's PknSharedFile. */
struct PknSharedFile {
    int id;
    void *file;
};
typedef char PknSharedFile_size_must_be_0x8[sizeof(PknSharedFile) == 0x8 ? 1 : -1];

extern "C" {
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *f, int a, int b, unsigned short cc);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *a, int r, int h, unsigned int e, unsigned int g);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *self, void *a, const Vector3 *v, int r, int h, unsigned int e, unsigned int g);
void LoadBlueCoinModel(void *c);
void UnloadBlueCoinModel(void *);
extern SharedFilePtr data_ov084_02130dfc;
extern SharedFilePtr *data_ov084_021302f4[];
extern SharedFilePtr data_ov002_0210da38;
extern PknSharedFile data_ov084_02130df4;
}

/* The nine state helpers. Behavior dispatches mState 0..3 to 0212e4e0,
 * 0212e010, 0212ddbc and 0212dc30, then runs 0212d86c (damage) and 0212d564
 * (fire position); 0212e010 calls 0212d2dc, 0212d42c and 0212d560.
 *
 * Leftover: they stay extern "C" functions over a char * receiver, as the
 * C shards they came from were written. Their callees stay mangled externs
 * for the same reason: a C-style body has no typed object to make a member
 * call through. Retyping them as daFPkn_c members is remaining work. */
typedef struct FPknVec3 { int x, y, z; } FPknVec3;
#define AT(p, off) ((void*)(int)((char*)(p) + (off)))
struct Locals {
    s16 acc[3];
    int tmp[3];
};

extern "C" {
u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 a, u32 b, Fix12i c, Fix12i d, Fix12i e, const void *f, void *g);
void *_ZN8Particle6System12FromUniqueIDEj(u32 id);
void func_02012694(u32 id, void *pos);
void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, void *pos);
void _ZN12dEnemyBase_c9SpawnCoinEv(void *self);
void _ZN8dActor_c24KillAndTrackInDeathTableEv(void *self);
void *_ZN8dActor_c10FindWithIDEj(u32 id);
void _ZN6Player16IncMegaKillCountEv(void *p);
int _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(void *self, void *clsn, void *player);
void _ZN6Player6BounceE5Fix12IiE(void *p, int fix);
int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, const void *pos, u32 a, int fix, u32 b, u32 c, u32 d);
int _Z14ApproachLinearRiii(int *cur, int target, int step);
int _Z14ApproachLinearRsss(short *cur, short target, short step);
int _ZNK9Animation12WillHitFrameEi(void *anim, int frame);
int _ZN9Animation8FinishedEv(void *anim);
void func_0201267c(unsigned int id, const void *pos);
void *_ZN8dActor_c13ClosestPlayerEv(void *self);
short Vec3_HorzAngle(const void *a, const void *b);
void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int id, unsigned int a, const void *pos, const void *rot, int e, int f);
void _ZN7fBase_c18MarkForDestructionEv(void *self);
void _ZN8dActor_c13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(
    void *self, const void *pos, const void *v16, int a, int b, u32 g);
void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void *self, const void *pos, unsigned int a, int b, short c);
void _ZN8dActor_c17TrackInDeathTableEv(void *self);
int _ZN8dActor_c13DistToCPlayerEv(void *self);
int IsStarCollectedInCurLevel(unsigned int flag);
void SetStarMarker(int i, void *self, int v);
void *_ZN8dActor_c15FindWithActorIDEjPS_(unsigned int id, void *prev);
void Matrix4x3_FromRotationY(void *m, int angle);
void Vec3_Asr(void *d, void *s, int sh);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void MulMat4x3Mat4x3(void *d, void *a, void *b);
void Vec3_LslInPlace(void *v, int sh);
void SubVec3(void *a, void *b, void *c);
void func_ov084_0212d2dc(char *c);
void func_ov084_0212d42c(char *self);
void func_ov084_0212d560(void *self);

extern PknSharedFile data_ov084_02130e24;   /* death animation */
extern int data_ov084_0213029c[];
extern int data_ov084_021302c4[];
extern void *data_ov084_02130e1c[];
extern PknSharedFile data_ov084_02130e14;   /* lunge animation */
extern void *data_ov084_02130e04[];
extern u8 data_ov084_02130294[];
extern s32 data_020a0e68[];
extern s16 data_02082214[];

/* One vtable store and four destructor calls, every one a consequence of
 * `struct daFPkn_c : dEnemyBase_c` and the members that declaration types,
 * destroyed in reverse declaration order, then dEnemyBase_c::~dEnemyBase_c.
 * That body is the evidence for the header: each member's size closes
 * exactly on the next one's offset. D0 ends in dEnemyBase_c's inline
 * operator delete, reachable because dEnemyBase_c is the immediate base. */
// @symbol _ZN8daFPkn_cD1Ev
// @symbol _ZN8daFPkn_cD0Ev
daFPkn_c::~daFPkn_c()
{
}

// @symbol func_ov084_0212d2dc
void func_ov084_0212d2dc(char* c)
{
    void* o;
    if (*(void **)(c + 0x170) != data_ov084_02130e24.file)
        return;

    *(u32*)(c + 0x224) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(c + 0x224), 0xfc, *(int*)(c + 0x5c), *(int*)(c + 0x60) + 0x1e000, *(int*)(c + 0x64), 0, 0);
    if (*(u32*)(c + 0x224) != 0) {
        o = _ZN8Particle6System12FromUniqueIDEj(*(u32*)(c + 0x224));
        if (o != 0) {
            *(int*)((char*)o + 0x50) = (short)(Fix12i)(((long long)(*(int*)(c + 0x210)) * 0x2800 + 0x800) >> 12);
        }
    }

    *(u32*)(c + 0x228) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(c + 0x228), 0xfd, *(int*)(c + 0x5c), *(int*)(c + 0x60) + 0x1e000, *(int*)(c + 0x64), 0, 0);
    if (*(u32*)(c + 0x228) == 0)
        return;
    o = _ZN8Particle6System12FromUniqueIDEj(*(u32*)(c + 0x228));
    if (o == 0)
        return;
    *(int*)((char*)o + 0x50) = (short)(Fix12i)(((long long)(*(int*)(c + 0x210)) * 0x2800 + 0x800) >> 12);
}

// @symbol func_ov084_0212d42c
void func_ov084_0212d42c(char *self)
{
    int space[3];
    int idx, fac, spd, s, m, x, y, z, zero, bias;
    long long prod;
    volatile int *ytbl;
    volatile int *xtbl;
    s16 *st;

    if (*(void **)(self + 0x170) != data_ov084_02130e24.file)
        return;
    idx = (int)((unsigned)(*(int *)(self + 0x168) << 4) >> 16);
    if (idx >= 0xa)
        return;

    x = *(int *)(self + 0x5c);
    st = data_02082214;
    *(volatile int *)&space[0] = x;
    y = *(int *)(self + 0x60);
    xtbl = data_ov084_0213029c;
    *(volatile int *)&space[1] = y;
    z = *(int *)(self + 0x64);
    bias = 0x800;
    *(volatile int *)&space[2] = z;

    {
        unsigned short ang = *(unsigned short *)(self + 0x8e);
        fac = xtbl[idx];
        spd = *(int *)(self + 0x204);
        s = st[(ang >> 4) << 1];
        ytbl = data_ov084_021302c4;
        zero = 0;
        m = fac * s;
        prod = (long long)m * spd + bias;
        x = x + (int)(prod >> 12);
        *(volatile int *)&space[0] = x;
    }
    {
        unsigned short ang = *(unsigned short *)(self + 0x8e);
        s = st[((ang >> 4) << 1) + 1];
        spd = *(int *)(self + 0x204);
        m = fac * s;
        prod = (long long)m * spd + bias;
        z = z + (int)(prod >> 12);
        *(volatile int *)&space[2] = z;
    }
    y = y + *(int *)(self + 0x204) * ytbl[idx];
    *(volatile int *)&space[1] = y;
    *(u32 *)(self + 0x224) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32 *)(self + 0x224), 0xfb, space[0], space[1], z, (void *)zero, (void *)zero);
}

/* Empty. func_ov084_0212e010 passes this in r0, so it takes an unused
 * pointer; the body is still one bx lr. */
// @symbol func_ov084_0212d560
void func_ov084_0212d560(void *)
{
}

// @symbol func_ov084_0212d564
void func_ov084_0212d564(char* c)
{
    struct Locals locals;
    int sx, sy, sz, v, flag;
    int *px, *py, *pz;

    Matrix4x3_FromRotationY(c + 0x12c, *(s16*)(c + 0x8e));
    *(s32*)(c + 0x150) = *(s32*)(c + 0x5c) >> 3;
    *(s32*)(c + 0x154) = *(s32*)(c + 0x60) >> 3;
    *(s32*)(c + 0x158) = *(s32*)(c + 0x64) >> 3;

    if ((u32)(*(s32*)(c + 0x1ec) - 2) > 1) return;
    if (*(s32*)(c + 0x204) != *(s32*)(c + 0x210)) return;

    locals.acc[0] = 0;
    locals.acc[1] = 0;
    locals.acc[2] = 0;
    {
        char* base = *(char**)(c + 0x120);
        int i;
        for (i = 0; i < 5; i++) {
            u8* p = data_ov084_02130294 + i;
            locals.acc[0] = (s16)(locals.acc[0] + *(s16*)(base + *p * 0x34 + 0x1a));
            locals.acc[1] = (s16)(locals.acc[1] + *(s16*)(base + *p * 0x34 + 0x1c));
            locals.acc[2] = (s16)(locals.acc[2] + *(s16*)(base + *p * 0x34 + 0x1e));
        }
    }

    *(s32*)(c + 0x1f8) = 0;
    *(s32*)(c + 0x1fc) = 0;
    *(s32*)(c + 0x200) = 0;

    Vec3_Asr(locals.tmp, c + 0x5c, 3);
    Matrix4x3_FromTranslation(
        data_020a0e68,
        locals.tmp[0],
        locals.tmp[1],
        locals.tmp[2]
    );
    MulMat4x3Mat4x3(
        *(char**)(c + 0x124) + 0x120,
        data_020a0e68,
        data_020a0e68
    );
    *(s32*)(c + 0x1f8) = data_020a0e68[0x24 / 4];
    *(s32*)(c + 0x1fc) = data_020a0e68[0x28 / 4];
    *(s32*)(c + 0x200) = data_020a0e68[0x2c / 4];
    Vec3_LslInPlace(c + 0x1f8, 3);
    SubVec3(c + 0x1f8, c + 0x5c, c + 0x1f8);

    sx = data_02082214[((u16)locals.acc[0] >> 4) * 2] * 0x32;
    sy = data_02082214[((u16)locals.acc[1] >> 4) * 2];

    px = (int*)(int)(c + 0x1f8);
    *px = *px + (int)(((s64)sx * sy + 0x800) >> 12);

    flag = (*(u16*)(c + 0xc) == 0xfb);
    if (flag != false) {
        py = (int*)(int)(c + 0x1fc);
        v = data_02082214[((u16)locals.acc[0] >> 4) * 2 + 1] * 0x32;
        *py = *py - (0x19000 - v);
    } else {
        py = (int*)(int)(c + 0x1fc);
        v = data_02082214[((u16)locals.acc[0] >> 4) * 2 + 1] * 0x32;
        *py = *py - (0x32000 - v);
    }

    pz = (int*)(int)(c + 0x200);
    sz = data_02082214[((u16)locals.acc[1] >> 4) * 2 + 1];
    *pz = *pz + (int)(((s64)sx * sz + 0x800) >> 12);

    *(s32*)(c + 0x1f8) =
        (int)(((s64)*(s32*)(c + 0x1f8) * *(s32*)(c + 0x204) + 0x800) >> 12);
    *(s32*)(c + 0x1fc) =
        (int)(((s64)*(s32*)(c + 0x1fc) * *(s32*)(c + 0x204) + 0x800) >> 12);
    *(s32*)(c + 0x200) =
        (int)(((s64)*(s32*)(c + 0x200) * *(s32*)(c + 0x204) + 0x800) >> 12);
}

// @symbol func_ov084_0212d86c
void func_ov084_0212d86c(char *r5)
{
    char *r4;
    int t;
    int flags;
    int pos1[3];
    int pos2[3];
    u32 id;

    id = *(u32 *)(r5 + 0x198);
    if (id == 0)
        goto second;

    flags = *(int *)(r5 + 0x194) & 0x66ff0;
    if (flags != 0) {
        t = (int)(*(u16 *)(r5 + 0xc) == 0xfb);
        if (t != 0)
            func_02012694(0x1e, r5 + 0x74);
        else
            _ZN5Sound9PlayBank0EjRK7Vector3(0xa, r5 + 0x74);
    activate_path:
        t = (int)(*(u16 *)(r5 + 0xc) == 0xfc);
        if (t != 0) {
            *(u8 *)(r5 + 0x108) = 1;
            _ZN12dEnemyBase_c9SpawnCoinEv(r5);
            _ZN8dActor_c24KillAndTrackInDeathTableEv(r5);
            _ZN5Sound9PlayBank0EjRK7Vector3(0xa, r5 + 0x74);
        } else {
            *(int *)(r5 + 0x1ec) = 1;
            *(u8 *)(r5 + 0x21d) = 0xa;
            *(u16 *)(r5 + 0x218) = 0x1f40;
            {
                u32 *p18c = (u32 *)(r5 + 0x18c);
                u32 *pb0 = (u32 *)(r5 + 0xb0);
                *p18c |= 1u;
                *pb0 &= ~0x10000000u;
            }
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(r5 + 0x110, data_ov084_02130e24.file, 0x40000000, 0x1000, 0);
            *(u8 *)(r5 + 0x21e) = 0;
            *(int *)(r5 + 0x224) = 0;
        }
        if ((*(int *)(r5 + 0x194) & 0x10) == 0)
            goto second;
        _ZN5Sound9PlayBank0EjRK7Vector3(0xa, r5 + 0x74);
        r4 = (char *)_ZN8dActor_c10FindWithIDEj(*(u32 *)(r5 + 0x198));
        if (r4 == 0)
            goto second;
        _ZN6Player16IncMegaKillCountEv(r4);
        func_02012694(0x1d, r5 + 0x74);
        goto second;
    }

    r4 = (char *)_ZN8dActor_c10FindWithIDEj(id);
    if (r4 == 0)
        goto second;
    t = (int)(*(u16 *)(r4 + 0xc) == 0xbf);
    if (t == 0)
        goto second;
    if (*(u8 *)(r4 + 0x6f9) != 0) {
        _ZN5Sound9PlayBank0EjRK7Vector3(0xa, r5 + 0x74);
        goto activate_path;
    }
    t = (int)(*(u16 *)(r5 + 0xc) == 0xfc);
    if (t != 0) {
        if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(r5, r5 + 0x174, r4) != 0) {
            _ZN5Sound9PlayBank0EjRK7Vector3(0xb6, r5 + 0x74);
            _ZN6Player6BounceE5Fix12IiE(r4, 0x28000);
            goto activate_path;
        }
    }
    if (*(u8 *)(r4 + 0x6fb) != 0)
        goto second;
    t = (int)(*(u16 *)(r5 + 0xc) == 0xfb);
    if (t != 0)
        goto second;
    pos1[0] = *(int *)(r5 + 0x5c);
    pos1[1] = *(int *)(r5 + 0x60);
    pos1[2] = *(int *)(r5 + 0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r4, pos1, 2, 0xc000, 1, 0, 1);

second:
    id = *(u32 *)(r5 + 0x1cc);
    if (id == 0)
        return;
    r4 = (char *)_ZN8dActor_c10FindWithIDEj(id);
    if (r4 == 0)
        return;
    t = (int)(*(u16 *)(r4 + 0xc) == 0xbf);
    if (t == 0)
        return;

    flags = *(int *)(r5 + 0x1c8) & 0x66ff0;
    if (flags != 0) {
        if ((flags & 0x10) != 0) {
            _ZN6Player16IncMegaKillCountEv(r4);
            func_02012694(0x1d, r5 + 0x74);
        } else {
            t = (int)(*(u16 *)(r5 + 0xc) == 0xfb);
            if (t != 0)
                func_02012694(0x1e, r5 + 0x74);
            else
                _ZN5Sound9PlayBank0EjRK7Vector3(0xa, r5 + 0x74);
        }
        *(int *)(r5 + 0x1ec) = 1;
        *(u8 *)(r5 + 0x21d) = 0xa;
        *(u16 *)(r5 + 0x218) = 0x1f40;
        {
            u32 *p18c = (u32 *)(r5 + 0x18c);
            u32 *pb0 = (u32 *)(r5 + 0xb0);
            *p18c |= 1u;
            *pb0 &= ~0x10000000u;
        }
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(r5 + 0x110, data_ov084_02130e24.file, 0x40000000, 0x1000, 0);
        *(u8 *)(r5 + 0x21e) = 0;
        *(int *)(r5 + 0x224) = 0;
        return;
    }

    if (*(u8 *)(r4 + 0x6f9) != 0)
        return;
    if (*(u8 *)(r4 + 0x6fb) != 0)
        return;
    pos2[0] = *(int *)(r5 + 0x5c);
    pos2[1] = *(int *)(r5 + 0x60);
    pos2[2] = *(int *)(r5 + 0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r4, pos2, 2, 0xc000, 1, 0, 1);
}

// @symbol func_ov084_0212dc30
void func_ov084_0212dc30(char *c)
{
    void *player;
    struct Vector3 v;
    short angle;
    void *spawned;
    int *p;

    if (_Z14ApproachLinearRiii((int*)(c + 0x204), *(int*)(c + 0x210), *(int*)(c + 0x214)) == 0) {
        goto tail;
    }

    if (_ZNK9Animation12WillHitFrameEi(c + 0x160, 0x10) ||
        _ZNK9Animation12WillHitFrameEi(c + 0x160, 0x20) ||
        _ZNK9Animation12WillHitFrameEi(c + 0x160, 0x34) ||
        _ZNK9Animation12WillHitFrameEi(c + 0x160, 0x4b)) {
        func_0201267c(0xc0, (struct Vector3*)(c + 0x74));
    }

    angle = *(short*)(c + 0x8e);
    player = _ZN8dActor_c13ClosestPlayerEv(c);
    p = (int*)AT(player, 0x5c);
    v.x = p[0];
    v.y = p[1];
    v.z = p[2];
    if (player != 0) {
        angle = Vec3_HorzAngle((struct Vector3*)(c + 0x5c), &v);
    }
    _Z14ApproachLinearRsss((short*)(c + 0x8e), angle, 0x400);

    if (*(unsigned char*)(c + 0x21e) == 1) {
        spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0xfa, 0, (struct Vector3*)(c + 0x5c), (void*)(c + 0x8c), *(signed char*)(c + 0xcc), -1);
        if (spawned == 0) return;

        *(unsigned char*)(c + 0x21e) = 2;
        func_ov084_0212ec04((char*)spawned, (short)((unsigned int)(*(int*)(c + 0x168) << 4) >> 16));
        *(int*)AT(c, 0x18c) |= 1;
        *(int*)AT(c, 0x1c0) |= 1;
        return;
    }

    _ZN7fBase_c18MarkForDestructionEv(c);
    return;

tail:
    *(int*)AT(c, 0x18c) |= 1;
}

// @symbol func_ov084_0212ddbc
void func_ov084_0212ddbc(char *c)
{
    struct Vector3 pos;
    s16 ang;
    int b;
    int speed;
    int idx;
    int dx;
    int dz;
    int y;
    int tmp;
    void *player;

    if (_Z14ApproachLinearRiii((s32 *)(c + 0x204), *(s32 *)(c + 0x210), *(s32 *)(c + 0x214)) == 0)
        goto cold;

    if (_ZN9Animation8FinishedEv(c + 0x160) != 0) {
        b = (int)(*(u16 *)(c + 0xc) == 0xfc);
        if (b != 0)
            func_0201267c(0xe3, c + 0x74);
        else
            func_0201267c(0x120, c + 0x74);
        *(s32 *)(c + 0x1ec) = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, data_ov084_02130e1c[1], 0, 0x1000, 0);
    } else {
        if (*(u16 *)(c + 0x100) < 0x3a) {
            ang = *(s16 *)(c + 0x8e);
            player = _ZN8dActor_c13ClosestPlayerEv(c);
            if (player != 0)
                ang = Vec3_HorzAngle(c + 0x5c, (char *)player + 0x5c);
            _Z14ApproachLinearRsss((s16 *)(c + 0x8e), ang, 0x400);
        }
    }

    if (_ZNK9Animation12WillHitFrameEi(c + 0x160, 0x3a) == 0)
        return;

    b = (int)(*(u16 *)(c + 0xc) == 0xfc);
    if (b != 0)
        func_0201267c(0x105, c + 0x74);
    else
        func_0201267c(0x122, c + 0x74);

    pos.x = *(s32 *)(c + 0x5c);
    y = *(s32 *)(c + 0x60);
    pos.y = y;
    pos.z = *(s32 *)(c + 0x64);
    speed = *(s32 *)(c + 0x210);
    idx = *(u16 *)(c + 0x8e) >> 4;
    tmp = speed * 0x3c;
    dx = (s32)(((s64)tmp * data_02082214[idx * 2] + 0x800) >> 12);
    pos.x = pos.x + dx;

    idx = *(u16 *)(c + 0x8e) >> 4;
    speed = *(s32 *)(c + 0x210);
    tmp = speed * 0x3c;
    dz = (s32)(((s64)tmp * data_02082214[idx * 2 + 1] + 0x800) >> 12);
    pos.z = pos.z + dz;

    speed = *(s32 *)(c + 0x210);
    pos.y = y + speed * 0x5a;
    *(s16 *)(c + 0x8c) = 0x1000;

    _ZN8dActor_c13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(
        c, &pos, c + 0x8c, 0x1e000, 0xa000, 3);
    return;

cold:
    if (*(s32 *)(c + 0x204) <= (*(s32 *)(c + 0x210) >> 1))
        return;
    {
        u32 *p18c = (u32 *)(c + 0x18c);
        u32 *pb0 = (u32 *)(c + 0xb0);
        *p18c &= ~1u;
        *pb0 |= 0x10000000u;
    }
}

// @symbol func_ov084_0212e010
void func_ov084_0212e010(char* self)
{
    FPknVec3 buf1;
    FPknVec3 buf2;
    char* other;
    int dist;
    int b;

    if (*(u8*)(self + 0x21d) != 0) {
        short* hp = (short*)(self + 0x8e);
        *hp = (s16)(*hp + *(s16*)(self + 0x218));
        _Z14ApproachLinearRsss((short*)(self + 0x218), 0, 0xc8);
        func_ov084_0212d42c(self);
        if (_ZN9Animation8FinishedEv(self + 0x160) == 0)
            return;
        (*(u8*)((int)self + 0x21d))--;
        if (*(u8*)(self + 0x21d) != 0)
            return;
        func_02012694(0x11f, self + 0x74);
        *(int*)(self + 0x228) = 0;
        *(int*)(self + 0x224) = *(int*)(self + 0x228);
        return;
    }

    func_ov084_0212d2dc(self);
    if (_Z14ApproachLinearRiii((int*)(self + 0x204), 0, *(int*)(self + 0x214)) == 0)
        return;
    {
        u32* p0 = (u32*)(self + 0xb0);
        u32* p1 = (u32*)(self + 0x18c);
        *p0 &= ~0x10000000;
        *p1 |= 1;
    }

    if (*(u8*)(self + 0x21c) != 0) {
        *(u8*)(self + 0x21c) = 0;
        b = 0;
        if (*(u16*)(self + 0xc) == 0xfb)
            b = 1;
        if (b != false) {
            other = (char*)_ZN8dActor_c10FindWithIDEj(*(u32*)(self + 0x1f0));
            if (other == 0)
                return;
            (*(u8*)((int)other + 0x21a))--;
            if (*(u8*)(self + 0x21e) != 0)
                return;
            (*(u8*)((int)other + 0x21b))++;
            if (*(u8*)(self + 0x220) != 0) {
                buf1.x = *(int*)(self + 0x5c);
                buf1.y = *(int*)(self + 0x60);
                buf1.z = *(int*)(self + 0x64);
                _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(self, &buf1, 2, 0xa000, 0);
            }
            if (*(u8*)(other + 0x21b) == 5) {
                _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0xb2, *(u8*)(self + 0x21f) | 0x40, (FPknVec3*)(self + 0x5c), 0, *(signed char*)(self + 0xcc), -1);
                _ZN8dActor_c24KillAndTrackInDeathTableEv(other);
                _ZN8dActor_c24KillAndTrackInDeathTableEv(self);
                return;
            }
            func_ov084_0212d560(self);
            if (*(int*)(self + 0x1e8) != 1) {
                _ZN8dActor_c24KillAndTrackInDeathTableEv(self);
                return;
            }
            _ZN8dActor_c17TrackInDeathTableEv(self);
            *(int*)(self + 0x1ec) = 4;
            return;
        }
        b = *(u16*)(self + 0xc) == 0xfd;
        if (b == false)
            return;
        if (*(u8*)(self + 0x21e) != 0)
            return;
        buf2.x = *(int*)(self + 0x5c);
        buf2.y = *(int*)(self + 0x60);
        buf2.z = *(int*)(self + 0x64);
        _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(self, &buf2, 1, 0xa000, 0);
        _ZN8dActor_c24KillAndTrackInDeathTableEv(self);
        return;
    }

    dist = _ZN8dActor_c13DistToCPlayerEv(self);
    b = *(u16*)(self + 0xc) == 0xfb;
    if (b != false) {
        other = (char*)_ZN8dActor_c10FindWithIDEj(*(u32*)(self + 0x1f0));
        if (other == 0)
            return;
    }
    if (*(u16*)(self + 0x100) <= 0x64)
        return;
    if (dist <= 0x64000)
        return;
    if (dist >= 0x320000)
        return;
    b = *(u16*)(self + 0xc) == 0xfb;
    if (b != false) {
        if (*(u8*)(other + 0x21a) >= 2)
            return;
    }
    b = *(u16*)(self + 0xc) == 0xfc;
    if (b != false)
        func_0201267c(0x104, self + 0x74);
    else
        func_0201267c(0x121, self + 0x74);
    b = 1;
    *(u8*)(self + 0x21c) = 1;
    if (*(u16*)(self + 0xc) != 0xfb)
        b = 0;
    if (b != false)
        (*(u8*)((int)other + 0x21a))++;
    b = *(u16*)(self + 0xc) == 0xfd;
    if (b != false) {
        *(int*)(self + 0x1ec) = 3;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x110, data_ov084_02130e14.file, 0x40000000, 0x1000, 0);
    } else {
        *(int*)(self + 0x1ec) = 2;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x110, data_ov084_02130e04[1], 0x40000000, 0x1000, 0);
        *(int*)(self + 0x168) = 0;
    }
    b = *(u16*)(self + 0xc) == 0xfb;
    if (b != false) {
        if (*(signed char*)(other + 0x221) >= 0) {
            int v;
            if (IsStarCollectedInCurLevel(*(u8*)(self + 0x21f)) != 0)
                v = 3;
            else
                v = 2;
            SetStarMarker(*(signed char*)(other + 0x221), self, v);
            *(int*)(other + 0x1f4) = *(int*)(self + 4);
        }
    }
    dist = (int)_ZN8dActor_c13ClosestPlayerEv(self);
    if (dist == 0)
        return;
    *(s16*)(self + 0x8e) = Vec3_HorzAngle((FPknVec3*)(self + 0x5c), (FPknVec3*)(dist + 0x5c));
}

// @symbol func_ov084_0212e4e0
int func_ov084_0212e4e0(char* c)
{
    void* p;
    if (*(int*)(c + 0x1e8) == 0) {
        *(signed char*)(c + 0x221) = -1;
        *(int*)(c + 0x1f0) = *(int*)(c + 4);
        *(int*)(c + 0x1f4) = *(int*)(c + 4);
        *(int*)(c + 0x1e8) = 1;
        p = 0;
        for (;;) {
            p = _ZN8dActor_c15FindWithActorIDEjPS_(0xfb, p);
            if (p == 0) break;
            if (p != c) {
                *(int*)((char*)p + 0x1e8) = 2;
                *(int*)((char*)p + 0x1f0) = *(int*)(c + 4);
            }
        }
    }
    *(int*)(c + 0x1ec) = 1;
    return 1;
}
}

// @symbol _ZN8daFPkn_c16CleanupResourcesEv
int daFPkn_c::CleanupResources()
{
    data_ov084_02130dfc.Release();
    for (int i = 0; i < 6; i++)
        data_ov084_021302f4[i]->Release();
    data_ov002_0210da38.Release();
    UnloadBlueCoinModel(this);
    return 1;
}

// @symbol _ZN8daFPkn_c6RenderEv
/* Leftover: Render. Folding the scale-zero test and the 0x40000 flag
   into one condition DIFFs; the flag stays in its own temporary. */
int daFPkn_c::Render()
{
    int v = mScale;
    int b;
    if (v == 0 || (b = (mFlags & 0x40000) != 0, b != 0)) {
        return 1;
    }
    Vector3 s;
    s.x = v;
    s.y = v;
    s.z = v;
    mModelAnim.Render(&s);
    return 1;
}

// @symbol _ZN8daFPkn_c8BehaviorEv
int daFPkn_c::Behavior()
{
    MakeVanishLuigiWork(mdCcAc_c);
    int b = (mFlags & 0x60000) != 0;
    if (b != 0) {
        func_ov084_0212d564(((char *)this));
        return 1;
    }
    mModelAnim.Advance();
    int s = mState;
    switch (s) {
    case 0:
        func_ov084_0212e4e0(((char *)this));
        break;
    case 1:
        func_ov084_0212e010(((char *)this));
        break;
    case 2:
        func_ov084_0212ddbc(((char *)this));
        break;
    case 3:
        func_ov084_0212dc30(((char *)this));
        break;
    case 4:
        break;
    }
    /* Leftover: Behavior. mStateTimer is s16, so ++ sign-extends (ldrsh).
       The ROM increments the halfword unsigned. */
    {
        unsigned short *p = (unsigned short *)&mStateTimer;
        *p = *p + 1;
        if (s != mState)
            *p = 0;
    }
    func_ov084_0212d86c(((char *)this));
    func_ov084_0212d564(((char *)this));
    mdCcAc_c.Clear();
    mdCcAc_c.radius = mScale * mClsnRadiusFactor;
    mdCcAc_c.height = mScale * mClsnHeightFactor;
    mdCcAc_c.Update();
    mdCcAcPos_c.Clear();
    int b2 = actorID == 0xfc;
    if (b2 == 0
        && (unsigned int)(mState - 2) <= 1
        && mScale == mMaxScale) {
        mdCcAcPos_c.SetPosRelativeToActor(mClsnOffset);
        mdCcAcPos_c.Update();
    }
    return 1;
}

// @symbol _ZN8daFPkn_c13InitResourcesEv
int daFPkn_c::InitResources()
{
    int i;
    Vector3 v;
    int id;
    int cond;

    mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov084_02130dfc), 1, -1);

    for (i = 0; i < 6; i++)
        Animation::LoadFile(*data_ov084_021302f4[i]);

    Model::LoadFile(data_ov002_0210da38);
    LoadBlueCoinModel(this);

    /* Leftover: InitResources. ModelAnim::SetAnim and the two dCc Init
       calls take Fix12<int> by value; spelling them that way DIFFs, so the
       calls stay the scalar externs. */
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, data_ov084_02130df4.file, 0x40000000, 0x1000, 0);

    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mdCcAc_c, this, 0, 0, 0x200001, 0x66fe0);

    v.x = 0;
    v.y = 0;
    v.z = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mdCcAcPos_c, this, &v, 0x4b000, 0x64000, 0x200002, 0x66fe0);

    mScale = 0;
    mRespawnMode = 0;
    mState = 0;
    mGroupLeaderID = 0;
    unk_1f4 = 0;
    mGroupAliveCount = 0;
    mGroupDefeatedCount = 0;
    unk_21c = 0;
    unk_21d = 0;
    mSuppressDeathReward = 1;
    unk_228 = 0;
    unk_224 = unk_228;

    id = actorID;
    cond = (id == 0xfc);
    if (cond != 0) {
        mClsnRadiusFactor = 0x3c;
        mClsnHeightFactor = 0xaa;
        mMaxScale = 0x800;
        mScaleRate = 0x52;
        mState = 1;
        mdCcAc_c.vulnFlags |= 0x8000;
    } else {
        cond = (id == 0xfd);
        if (cond != 0) {
            mClsnRadiusFactor = 0x28;
            mClsnHeightFactor = 0xaa;
            mMaxScale = 0x1000;
            mScaleRate = 0xa4;
            mState = 1;
        } else {
            mClsnRadiusFactor = 0x28;
            mClsnHeightFactor = 0x96;
            mMaxScale = 0x2000;
            mScaleRate = 0x147;
            mdCcAcPos_c.radius = 0x64000;
            mdCcAcPos_c.height = 0x64000;
            if (GetBitInDeathTable() != 0)
                mAlive = 0;
            else
                mAlive = 1;
        }
    }

    mStarID = (unsigned char)(param1 & 0xf);
    return 1;
}

// @symbol _ZN8daFPkn_c16OnAimedAtWithEggEv
s32 daFPkn_c::OnAimedAtWithEgg() {
    if (mdCcAc_c.flags & 1)
        return mScale * 100;
    int reach = mScale * mClsnHeightFactor >> 1;
    int floor = mScale * 100;
    if (reach <= floor)
        reach = floor;
    return reach;
}

// @symbol _ZN8daFPkn_c13OnTurnIntoEggER6Player
void daFPkn_c::OnTurnIntoEgg(Player &player)
{
    GivePlayerCoins(player, 1, 0);
    KillAndTrackInDeathTable();
}

// @symbol _ZN8daFPkn_c13OnYoshiTryEatEv
/* Leftover: OnYoshiTryEat keeps the two-step flag. A single
   `return actorID == 0xfc ? 4 : 0` DIFFs. */
s32 daFPkn_c::OnYoshiTryEat() {
    int r;
    if (actorID == 0xfc)
        r = 1;
    else
        r = 0;
    if (r != 0)
        r = 4;
    else
        r = 0;
    return r;
}

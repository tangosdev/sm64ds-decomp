//cpp
/**
 * Chief Chilly (KING_DONKETU 218) -- ov073/daKing_Donketu_c.
 *
 * Snowman's Land's ice-bully boss. Twelve file-scope state records;
 * ChiefChilly_ChangeState stores the pointer and dispatches. Three
 * knock-downs, two waypoint sets, a ground-ray arena guard.
 *
 * daKing_Donketu_c_classInit is reconstructed (RTTI daKing_Donketu_c,
 * KING_DONKETU registry). Retail does not store that spelling.
 * Historical alias: ChiefChilly_Spawn.
 *
 * This TU is 43 of 47. Behavior / InitResources / OnAimedAtWithEgg /
 * classInit stay in their own shards -- file-global opt_propagation off
 * on Behavior recompiles eight other members (this TU).
 *
 * deslop
 * Leftover: BlendModelAnim::SetAnim / Particle::System::New /
 *   Particle::System::NewSimple / Particle::RunningSlidingDustAt /
 *   Player::Hurt / Sound::ChangeMusicVolume / DropShadowRadHeight
 *   stay mangled in this TU -- Fix12<int> by value (wall 6az); a
 *   method call homes the argument. Particle.h has no System::NewSimple.
 *   Particle::System::FromUniqueID is on Particle__System.h but this
 *   TU pokes sys+0x44 after the call. Callers: func_ov073_0211f144 /
 *   0211f2c0 / 0211f494 / 0211f61c / 0211fa74 / 0211fe8c / 0212000c /
 *   021200e0 / 021203ac / 02120610 / 02120ad8 / 02120c08 / 02120c7c /
 *   02120ed0 / 0212122c / 02121538 / 021215cc.
 * Leftover: dActor_c::FindWithID / ClosestPlayer / HorzAngleToCPlayer /
 *   DistToCPlayer / Spawn / PoofDustAt / HugeLandingDustAt /
 *   JumpedOnByPlayer / FindWithActorID stay mangled -- helpers are
 *   C-linkage offset soup (`char *this`). Named members already call
 *   CleanupResources / Render / OnPendingDestroy.
 * Leftover: Camera::SetLookAt / SetPos stay mangled (void *cam, offset
 *   soup). Camera::SetFlag_3 is a real method but is not on Camera.h;
 *   this TU's talk/cutscene helpers.
 * Leftover: fBase_c::MarkForDestruction / Animation::Finished /
 *   Animation::WillHitFrame / dBgCh_Actr::IsOnGround stay mangled
 *   (this+0x35c / this+0x150). Player::StartTalk / ShowMessage /
 *   GetTalkState / GetHurtState / Unk_020c6a10 stay mangled. Sound::PlayLong
 *   is on Sound.h but Layer3 Load/Stop is not. Message::EndTalk /
 *   PrepareTalk stay mangled. SaveData::IsCharacterUnlocked stays mangled.
 *   cstd::atan2 stays mangled (Fix12-by-value). func_ov073_0211f494.
 * Leftover: func_ov073_* helpers stay linker names (offset soup, PMF
 *   dispatch through data_ov073_021233*). Not coined except
 *   ChiefChilly_ChangeState, the English gloss for this class's
 *   dispatcher. struct C is complete here: mwccarm 2004/b56 picks
 *   pointer-to-member from completeness (this TU).
 * Leftover: data_ov073_02123280..b8 stay BCA_File*[2] so [1] is the
 *   BCA SetAnim reads; CleanupResources puns each to SharedFilePtr
 *   for Release. ov073 sinit constructs them; this TU does not own
 *   .bss. data_ov002_0210da30 is ov002; Cleanup Release. Naming
 *   belongs in ov002.
 * Leftover: data_ov073_02123330 / 350 / 360 / 370 / 3b0 / 3c0 / 3f0
 *   and decl_common's 021233e0 / 02123410 are sinit-owned state
 *   records this TU does not own.
 * Leftover: data_02082214 is the NitroSDK FX_SinCosTable_;
 *   func_ov073_0211f494 indexes it. Naming belongs with the SDK table.
 * Leftover: data_0209f318 camera / data_020a0e68 scratch matrix /
 *   data_0209e650 RNG seed are arm9 globals. func_02012694 (sound) /
 *   func_0200d8c8 (camera shake) / func_0200fa8c / func_02011cfc.
 * Leftover: Vec3_HorzAngle / VertAngle / HorzLen / Sub / Lsl / Asr /
 *   Matrix4x3_FromRotationY / FromTranslation /
 *   ApplyInPlaceToRotationX / ApplyInPlaceToRotationXYZExt /
 *   MulVec3Mat4x3 / MulMat4x3Mat4x3 / ApproachLinear: no shared
 *   header this TU can take without a campaign.
 * Leftover: g_profile_KING_DONKETU lives outside this TU (S14).
 * Leftover: no return new -- classInit stays in src/d_a_king_donketu.cpp
 *   (one of the four unabsorbed members).
 * Leftover: reverse order is not used. `#pragma defer_codegen off` is
 *   load-bearing: out-of-line D1 then D0 then homeless D2 matches the
 *   cartridge (deferred codegen emits D2, D0, D1).
 * Leftover: sizeof wrap in the header (0x504).
 * Leftover: `#pragma opt_loop_invariants off` is file-global and
 *   load-bearing for func_ov073_0211f61c (this TU).
 * Leftover: struct C / CB / V3 / Vec3 / Mtx43 / Mat4x3 / Base / Bool
 *   shadows are load-bearing (PMF dispatch, Render vcall, POD triples).
 * Leftover: common.h first -- BlendModelAnim.h's nested Matrix4x3
 *   would win and size-DIFF the 12-word copies (this TU).
 */

#pragma defer_codegen off
#pragma opt_loop_invariants off

#include "common.h"
#include "daKing_Donketu_c.h"
#include "decl_common.h"
#include "decl_Message.h"
#include "types.h"
#include "SharedFilePtr.h"

struct C;
typedef int (C::*PMF)();
struct C { char pad[0x37c]; PMF *pp; };

struct CB {
    char pad0[0x8e];
    short field_8e;
    char pad90[0x94-0x90];
    short field_94;
    char pad96[0x98-0x96];
    int field_98;
    char pad9c[0x35c-0x9c];
    char anim_35c;
    char pad35d[0x4c9-0x35d];
    unsigned char field_4c9;
    char pad4ca[0x4cb-0x4ca];
    unsigned char field_4cb;
    char pad4cc[0x4d4-0x4cc];
    int field_4d4, field_4d8, field_4dc;
};

struct V3 { int x; int y; int z; };
typedef struct { int x, y, z; } Vec3;

enum Bool { FALSE, TRUE };

typedef struct Mtx43 { Vec3 r0, r1, r2, t; } Mtx43;
struct Mat4x3 { int m[12]; };

struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void M(void*); };

extern "C" {

extern char data_ov073_02123330[];
extern char data_ov073_02123350[];
extern char data_ov073_02123360[];
extern char data_ov073_02123370[];
extern char data_ov073_021233b0[];
extern char data_ov073_021233c0[];
extern char data_ov073_021233f0[];

int ChiefChilly_ChangeState(C *c, PMF *p);

extern struct BCA_File *data_ov073_02123280[2];
extern struct BCA_File *data_ov073_02123288[2];
extern struct BCA_File *data_ov073_02123290[2];
extern struct BCA_File *data_ov073_02123298[2];
extern struct BCA_File *data_ov073_021232a0[2];
extern struct BCA_File *data_ov073_021232a8[2];
extern struct BCA_File *data_ov073_021232b0[2];
extern struct BCA_File *data_ov073_021232b8[2];
extern struct BCA_File *data_ov002_0210da30[2];
extern short Vec3_HorzAngle(const Vector3* a, const Vector3* b);
extern void Matrix4x3_FromRotationY(void* m, short ang);
extern void MulVec3Mat4x3(const void* in, void* m, void* out);
extern unsigned int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int, unsigned int, Fix12i, Fix12i, Fix12i, const void*, void*);
extern void* _ZN8Particle6System12FromUniqueIDEj(unsigned int id);
extern void func_0200d8c8(void *cam, void *v, int strength);
extern void MulMat4x3Mat4x3(void *dst, void *a, void *b);
extern void Vec3_Lsl(void *d, void *s, int sh);
extern void _ZN8dActor_c17HugeLandingDustAtER7Vector3b(void *self, void *v, int b);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void *data_0209f318;
extern struct Matrix4x3 data_020a0e68;
extern void Vec3_Sub(Vec3 *out, Vec3 *a, Vec3 *b);
extern int _ZN4cstd5atan2E5Fix12IiES1_(Fix12i y, Fix12i x);
extern Fix12i Vec3_HorzLen(Vec3 *v);
extern short data_02082214[];
extern u16 data_0209e650;
extern u16 DecIfAbove0_Short(void* p);
extern void* _ZN8dActor_c10FindWithIDEj(u32 id);
extern void func_ov073_0211f494(void *pa, void *pb);
extern int _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(void* c, void* clsn, void* player);
extern void func_02012694(int a, void* b);
extern int RandomIntInternal(u16* seed);
extern void* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 a, u32 b, const Vector3* v, void* rot, s32 e, s32 f);
extern int _ZN8SaveData19IsCharacterUnlockedEj(u32 id);
extern int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, const Vector3* v, u32 a, Fix12i f, u32 b, u32 c, u32 d);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void* thiz, struct BCA_File* f, int i, int j, Fix12i fx, u16 k);
extern void _ZN6Camera9SetFlag_3Ev(void* cam);
extern void _Z14ApproachLinearRiii(int* p, int t, int s);
extern void _ZN7Message7EndTalkEv(void);
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int a, int b);
extern short _ZN8dActor_c18HorzAngleToCPlayerEv(void* self);
extern void* _ZN8dActor_c15FindWithActorIDEjPS_(unsigned int id, void* prev);
extern void _ZN8dActor_c10PoofDustAtERK7Vector3(void* self, void* pos);
extern void _ZN7fBase_c18MarkForDestructionEv(void* self);
extern void* _ZN8dActor_c13ClosestPlayerEv(void* actor);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int c, struct Vector3* v, unsigned int d);
extern void _ZN6Camera9SetLookAtERK7Vector3(void* cam, struct Vector3* v);
extern void _ZN6Camera6SetPosERK7Vector3(void* cam, struct Vector3* v);
extern int _ZN6Player9StartTalkER7fBase_cb(void* self, void* actor, int b);
extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void* self, void* actor, unsigned int msg, const struct Vector3* pos, unsigned int a, unsigned int b);
extern void _Z14ApproachLinearRsss(short* a, short b, short c);
extern s16 Vec3_VertAngle(const void* a, const void* b);
extern void _ZN6Player12Unk_020c6a10Ej(void* self, u32 a);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* self);
extern void func_ov073_0211f2c0(void *self, int strength);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, int angX);
extern int func_ov073_0211f61c(void *c);
extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int a, int b, int c);
extern int _ZN9Animation8FinishedEv(void* anim);
extern int _ZN8dActor_c13DistToCPlayerEv(void *self);
extern int _ZN6Player12GetHurtStateEv(void *self);
extern int _ZNK9Animation12WillHitFrameEi(void *self, int f);
extern int _ZN6Player12GetTalkStateEv(void* self);
extern void Vec3_Asr(Vec3* d, Vec3* s, int sh);
extern void Matrix4x3_FromTranslation(struct Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* thiz, void* sm, void* m, int rad, int h, unsigned int u);
extern void UnloadKeyModels(int i);
extern void func_ov073_021215cc(void *self);
}

// @symbol _ZN16daKing_Donketu_cD1Ev
// @symbol _ZN16daKing_Donketu_cD0Ev
daKing_Donketu_c::~daKing_Donketu_c()
{
}

// @symbol func_ov073_0211f144
extern "C" {
void func_ov073_0211f144(void* self) {
    char* c = (char*)self;

    Vector3 pos, in, out;
    int y;
    ((int*)&pos)[0] = *(int*)(c + 0x5c);
    y = *(int*)(c + 0x60);
    ((int*)&pos)[1] = y;
    ((int*)&pos)[2] = *(int*)(c + 0x64);
    ((int*)&pos)[1] = y + (int)(((long long)*(int*)(c + 0x80) * 0x1e000 + 0x800) >> 12);

    Vec3_HorzAngle((Vector3*)(c + 0x5c), (Vector3*)(c + 0x3d8));
    in.z = 0; in.z = 0xc8000; in.x = 0; in.y = 0;
    out.x = 0; out.y = 0; out.z = 0;
    Matrix4x3_FromRotationY(&data_020a0e68, *(short*)(c + 0x94));
    MulVec3Mat4x3(&in, &data_020a0e68, &out);
    pos.x = pos.x + out.x;
    pos.z = pos.z + out.z;
    *(unsigned int*)(c + 0x4f8) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*(unsigned int*)(c + 0x4f8), 0x77, pos.x, pos.y, pos.z, 0, 0);
    *(unsigned int*)(c + 0x4fc) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*(unsigned int*)(c + 0x4fc), 0x78, pos.x, pos.y, pos.z, 0, 0);
    if (*(unsigned int*)(c + 0x4f8) != 0) {
        void* sys = _ZN8Particle6System12FromUniqueIDEj(*(unsigned int*)(c + 0x4f8));
        if (sys != 0) *(int*)((char*)sys + 0x44) = *(int*)(c + 0x80) * 0x14;
    }
    if (*(unsigned int*)(c + 0x4fc) == 0) return;
    {
        void* sys = _ZN8Particle6System12FromUniqueIDEj(*(unsigned int*)(c + 0x4fc));
        if (sys != 0) *(int*)((char*)sys + 0x44) = *(int*)(c + 0x80) * 0x14;
    }
}
}

// @symbol func_ov073_0211f2c0
extern "C" {
void func_ov073_0211f2c0(void *self, int strength)
{
  char *c = (char *)self;
  struct V3 src;
  volatile struct V3 pv;
  struct V3 *new_var;
  struct V3 in;
  struct V3 res;
  struct V3 out;
  struct V3 dv;
  int ty;
  int tx;
  int tz;
  int x;
  int y;
  int z;
  func_0200d8c8(data_0209f318, c + 0x5c, strength);
  data_020a0e68 = *((struct Matrix4x3 *) (c + 0x328));
  MulMat4x3Mat4x3(((char *) (*((void **) (c + 0x320)))) + ((*((int *) (c + 0x4bc))) * 0x30), &data_020a0e68, &data_020a0e68);
  *((int *) ((((int) c) + 0x4bc))) ^= 1;
  ty = *((int *) (((char *) (&data_020a0e68)) + 0x28));
  tx = *((int *) (((char *) (&data_020a0e68)) + 0x24));
  src.y = ty;
  tz = *((int *) (((char *) (&data_020a0e68)) + 0x2c));
  src.z = tz;
  src.x = tx;
  Vec3_Lsl(&out, &src, 3);
  x = out.x;
  y = out.y;
  z = out.z;
  src.x = x;
  src.y = y;
  src.z = z;
  if ((*((void **) (c + 0x37c))) == (&data_ov073_02123360))
  {
    dv.x = x;
    dv.y = y;
    dv.z = z;
    _ZN8dActor_c17HugeLandingDustAtER7Vector3b(c, &dv, 1);
    return;
  }
  pv.x = *((int *) (c + 0x5c));
  pv.y = *((int *) (c + 0x60));
  pv.z = *((int *) (c + 0x64));
  {
    unsigned char *b = (unsigned char *) c;
    void *anim = *((void **) (b + 0x37c));
    if (anim == (&data_ov073_021233c0))
    {
      goto do_mtx;
    }
    if (anim != (&data_ov073_021233f0))
    {
      goto spawn;
    }
  }
  do_mtx:
  in.z = 0;

  in.z = -0xe6000;
  in.x = 0;
  in.y = 0;
  res.x = 0;
  res.y = 0;
  res.z = 0;
  new_var = &res;
  Matrix4x3_FromRotationY(&data_020a0e68, *((short *) (c + 0x8e)));
  MulVec3Mat4x3(&in, &data_020a0e68, &res);

  {
    int px = pv.x;
    int rx = (*new_var).x;
    px = px + rx;
    rx = px;
    int pz = pv.z;
    int rz = res.z;
    {
      int t = pz + rz;
      rz = t;
    }
    pv.x = rx;
    pv.z = rz;
  }

  spawn:
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x88, pv.x, pv.y, pv.z);

  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x89, pv.x, pv.y, pv.z);
}
}

// @symbol func_ov073_0211f494
extern "C" {
void func_ov073_0211f494(void *pa, void *pb)
{
    char *a = (char *)pa;
    char *b = (char *)pb;
    Vec3 t, p, q, d;
    int ay, ax, vx, tmp, cos_ax, sin_ax, cos_ay, sin_ay, rx, ry, rz;
    int y0;
    int *bp = (int *)(((int)b + 0x5c));

    p.x = *(int *)(a + 0x5c);
    y0 = *(int *)(a + 0x60);
    p.y = y0;
    {
        Vec3 *pq = &q;
        int zval = *(int *)(a + 0x64);
        Vec3 *pp = &p;
        pp->z = zval;
        pq->x = bp[0];
        pq->y = bp[1];
        pq->z = bp[2];
        {
            int qy = pq->y + 0x46000;
            int h = *(int *)(a + 0x118);
            vx = *(int *)(a + 0x114);
            pp->y = y0 + h;
            pq->y = qy;
            Vec3_Sub(&d, pq, pp);
        }
    }
    t.x = d.x; t.y = d.y; t.z = d.z;
    ay = _ZN4cstd5atan2E5Fix12IiES1_(t.x, t.z);
    ax = _ZN4cstd5atan2E5Fix12IiES1_(t.y, Vec3_HorzLen(&t));
    ax = (int)((unsigned short)ax >> 4);
    ay = (int)((unsigned short)ay >> 4);
    sin_ax = data_02082214[ax * 2 + 1];
    cos_ax = data_02082214[ax * 2];
    tmp = (int)(((long long)vx * sin_ax + 0x800) >> 12);
    ry = (int)(((long long)vx * cos_ax + 0x800) >> 12);
    cos_ay = data_02082214[ay * 2];
    sin_ay = data_02082214[ay * 2 + 1];
    rx = (int)(((long long)tmp * cos_ay + 0x800) >> 12);
    rz = (int)(((long long)tmp * sin_ay + 0x800) >> 12);
    p.x += rx; p.y += ry; p.z += rz;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x84, p.x, p.y, p.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x85, p.x, p.y, p.z);
}
}

// @symbol func_ov073_0211f61c
extern "C" s32 func_ov073_0211f61c(void* self)
{
    char* c = (char*)self;
    void* r5;
    s32 r4;
    u32 id;

    if (DecIfAbove0_Short(c + 0x4cc) != 0)
        return 0;
    id = *(u32*)(c + 0x134);
    if (id == 0)
        return 0;
    r5 = _ZN8dActor_c10FindWithIDEj(id);
    if (!r5)
        return 0;

    r4 = 0;
    if (*(s32*)(c + 0x130) & 0x6000) {
        func_ov073_0211f494(c, c);
        *(s32*)(c + 0x98) = 0x20000;
        r4 = 1;
    }

    {
        int isBf = (int)(*(u16*)((char*)r5 + 0xc) == 0xbf);
        if (isBf == 1) {
            if (*(u8*)((char*)r5 + 0x703) != 0) {
                s32 flags = *(s32*)(c + 0x130) & 0x107e0;
                if (flags) {
                    *(s32*)(c + 0x98) = 0x41000;
                    r4 = 1;
                }
            }
            if (r4 == 0) {
                s32 flags = *(s32*)(c + 0x130) & 0x50380;
                if (flags) {
                    func_ov073_0211f494(c, c);
                    *(s32*)(c + 0x98) = 0x2d000;
                    r4 = 1;
                }
            }
            if (r4 == 0) {
                if ((*(s32*)(c + 0x130) & 0x70) || (*(u8*)((char*)r5 + 0x6f9) != 0)
                    || (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(c, c + 0x110, r5) != 0)) {
                    func_ov073_0211f494(c, c);
                    *(s32*)(c + 0x98) = 0x20000;
                    r4 = 1;
                }
            }
        }
    }

    if (r4 == 0) {
        if (*(s32*)(c + 0x130) & 0x400) {
            func_ov073_0211f494(c, c);
            *(s32*)(c + 0x98) = 0x3d000;
            r4 = 1;
        }
    }

    if (r4 != 0) {
        s32 count;
        s32 i;
        s32 shortY;
        Vector3 v;
        s32 rnd;

        *(u16*)(c + 0x4cc) = 0x10;
        func_02012694(0x16b, c + 0x74);
        rnd = RandomIntInternal(&data_0209e650);
        if (((rnd >> 8) & 7) == 0) {
            v.x = *(s32*)(c + 0x5c);
            v.y = *(s32*)(c + 0x60);
            v.z = *(s32*)(c + 0x64);
            count = (rnd >> 0xc) & 3;
            v.y = v.y + 0x64000;
            if (count == 0)
                count = 1;
            i = 0;
            if (count > 0) {
                do {
                    void* actor = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x120, 0, &v, 0, *(s8*)(c + 0xcc), -1);
                    if (actor != 0) {
                        rnd = RandomIntInternal(&data_0209e650);
                        shortY = ((s32)((((u32)rnd >> 8) & 0xf) << 0x1c)) >> 0x10;
                        *(s16*)((char*)actor + 0x92) = 0;
                        *(s16*)((char*)actor + 0x94) = (s16)shortY;
                        *(s16*)((char*)actor + 0x96) = 0;
                        *(s32*)((char*)actor + 0x98) = 0xa000;
                        if (_ZN8SaveData19IsCharacterUnlockedEj(2) != 0) {
                            *(s32*)(void*)(int)(c + 0x4c0) += 1;
                            if (*(s32*)(c + 0x4c0) > 0x1e) {
                                void* actor2 = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x115, 0, &v, 0, *(s8*)(c + 0xcc), -1);
                                if (actor2 != 0) {
                                    *(s16*)((char*)actor2 + 0x92) = 0;
                                    *(s16*)((char*)actor2 + 0x94) = (s16)shortY;
                                    *(s16*)((char*)actor2 + 0x96) = 0;
                                    *(s32*)((char*)actor2 + 0x98) = 0xa000;
                                }
                                *(s32*)(c + 0x4c0) = 0;
                            }
                        }
                    }
                    i = i + 1;
                } while (i < count);
            }
        }
        {
            void* anim = *(void**)(c + 0x37c);
            if (anim != &data_ov073_021233d0 && anim != &data_ov073_021233a0) {
                ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_021233a0));
            }
        }
        return 1;
    }

    {
        Vector3 v2;
        v2.x = *(s32*)(c + 0x5c);
        v2.y = *(s32*)(c + 0x60);
        v2.z = *(s32*)(c + 0x64);
        if (_ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r5, &v2, 0, 0x14000, 1, 0, 1) == 0)
            goto done0;
    }
    {
        s32* pv = (s32*)((u32)r5 + 0x5c);
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x8a, pv[0], pv[1], pv[2]);
    }
    if (*(void**)(c + 0x37c) == &data_ov073_021233d0) {
        func_02012694(0x16b, c + 0x74);
        return 1;
    }
    {
        s16* py = (s16*)((u32)c + 0x94);
        *py = (s16)(*py + 0x8000);
    }
    func_02012694(0x16a, c + 0x74);
    ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123360));
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x30c, data_ov073_021232a0[1], 4, 0x40000000, 0x1000, 0);
    *(s32*)(c + 0x368) = 0x1000;
    *(u8*)(c + 0x4c8) = 2;
    *(s32*)(c + 0x4d0) = 0x2000;
    *(s16*)(c + 0x100) = 0x1e;
done0:
    return 0;
}

// @symbol func_ov073_0211fa74
extern "C" {
int func_ov073_0211fa74(char* c) {
    void* cam;
    void* spawned;
    void* found;
    volatile struct Vector3_16 rot;
    struct Vector3 pos;
    cam = data_0209f318;
    _ZN6Camera9SetFlag_3Ev(cam);
    _Z14ApproachLinearRiii((int*)(c + 0x80), 0, 0x80);
    *(int*)(c + 0x88) = *(int*)(c + 0x80);
    *(int*)(c + 0x84) = *(int*)(c + 0x88);
    func_ov073_0211f144(c);
    if (*(int*)(c + 0x80) >= 0x100) goto end;
    pos.x = *(int*)(c + 0x3d8);
    pos.y = *(int*)(c + 0x3dc);
    pos.z = *(int*)(c + 0x3e0);
    _ZN7Message7EndTalkEv();
    _ZN5Sound22StopLoadedMusic_Layer3Ev();
    func_02011cfc();
    _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x15666);
    pos.y = pos.y + 0x190000;
    func_0200fa8c(c, 1);

    {
        unsigned short a = *(unsigned short*)(c + 0x8c);
        unsigned short b = *(unsigned short*)(c + 0x8e);
        rot.x = b ? a : a;
        rot.y = b;
        rot.z = *(unsigned short*)(c + 0x90);
        rot.y = (unsigned short)_ZN8dActor_c18HorzAngleToCPlayerEv(c);
    }

    spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x11a, 4, &pos, 0, *(signed char*)(c + 0xcc), -1);
    found = _ZN8dActor_c15FindWithActorIDEjPS_(0x13d, 0);
    func_02012694(0xbb, c + 0x74);
    if (found != 0) {
        struct Vector3 fp;
        int pv = (int)(((int)found + 0x5c));
        fp.x = *(int*)pv;
        fp.y = *(int*)(pv + 4);
        fp.z = *(int*)(pv + 8);
        _ZN8dActor_c10PoofDustAtERK7Vector3(c, &fp);
        _ZN7fBase_c18MarkForDestructionEv(found);
    }
    if (spawned != 0) {
        *(int*)(((int)cam + 0x154)) &= ~8;
        _ZN7fBase_c18MarkForDestructionEv(c);
    }
end:
    return 1;
}
}

// @symbol func_ov073_0211fbec
extern "C" {
int func_ov073_0211fbec(void)
{
    return 1;
}
}

// @symbol func_ov073_0211fbf4
extern "C" {
extern void func_ov073_0211f144(void*);
int func_ov073_0211fbf4(char* c){
  void* pl = *(void**)(c+0x3e4);
  *(unsigned int*)(c+0x500) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(unsigned int*)(c+0x500), 3, 0x170, (struct Vector3 *)(c+0x74), 0);
  _ZN6Camera9SetFlag_3Ev(data_0209f318);
  func_ov073_0211f144(c);
  if(_ZN6Player12GetTalkStateEv(pl) == -1){
    ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123370));
  }
  return 1;
}
}

// @symbol func_ov073_0211fc70
extern "C" {
int func_ov073_0211fc70(void)
{
    return 1;
}
}

// @symbol func_ov073_0211fc78
extern "C" {
int func_ov073_0211fc78(char* c) {
    struct Vector3 msgpos[2];
    struct Vector3 la, ps, in, out;
    void* player;
    void* cam;

    player = _ZN8dActor_c13ClosestPlayerEv(c);
    *(unsigned int*)(c + 0x500) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(unsigned int*)(c + 0x500), 3, 0x170, (struct Vector3*)(c + 0x74), 0);
    if (player == 0) return 1;

    func_ov073_0211f144(c);

    {
        int pv = (int)((int)player + 0x5c);
        msgpos[0].x = *(int*)pv;
        msgpos[0].y = *(int*)(pv + 4);
        msgpos[0].z = *(int*)(pv + 8);
    }
    cam = data_0209f318;
    msgpos[1].x = *(int*)(c + 0x5c);
    msgpos[1].y = *(int*)(c + 0x60);
    msgpos[1].z = *(int*)(c + 0x64);

    _ZN6Camera9SetFlag_3Ev(cam);

    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;

    la.x = *(int*)(c + 0x3d8);
    la.y = *(int*)(c + 0x3dc);
    la.z = *(int*)(c + 0x3e0);
    ps.x = *(int*)(c + 0x5c);
    ps.y = *(int*)(c + 0x60);
    ps.z = *(int*)(c + 0x64);

    la.y -= 0x300000;
    in.z = -0x55e000;

    Matrix4x3_FromRotationY(&data_020a0e68, *(short*)(c + 0x8e));
    MulVec3Mat4x3(&in, &data_020a0e68, &out);

    ps.x = ps.x + out.x;
    ps.y = ps.y + 0x200000;
    ps.z = ps.z + out.z;

    _ZN6Camera9SetLookAtERK7Vector3(cam, &la);
    _ZN6Camera6SetPosERK7Vector3(cam, &ps);

    if (player != 0) {
        int msg;
        *(void**)(c + 0x3e4) = player;
        msg = (short)(*(int*)((char*)*(void**)(c + 0x3e4) + 8) + 0xe7);
        _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x14, 0x15666);
        _ZN7Message11PrepareTalkEv();
        if (_ZN6Player9StartTalkER7fBase_cb(*(void**)(c + 0x3e4), c, 1)) {
            _ZN6Camera9SetFlag_3Ev(cam);
            if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(*(void**)(c + 0x3e4), c, msg, &msgpos[1], 0, 2)) {
                func_02012694(0x12a, (void*)(c + 0x74));
                ChiefChilly_ChangeState((C *)(c), (PMF *)(&data_ov073_02123410));
            }
        }
    }
    return 1;
}
}

// @symbol func_ov073_0211fe84
extern "C" {
int func_ov073_0211fe84(void)
{
    return 1;
}
}

// @symbol func_ov073_0211fe8c
extern "C" {
int func_ov073_0211fe8c(char* c) {
    struct Vector3 look, pos, in, out;
    void* player;
    void* cam;

    *(unsigned int*)(c + 0x500) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(unsigned int*)(c + 0x500), 3, 0x170, (struct Vector3*)(c + 0x74), 0);

    player = _ZN8dActor_c13ClosestPlayerEv(c);
    if (player != 0 && *(unsigned char*)((char*)player + 0x6de) == 0) {
        int angle;
        _ZN6Player9StartTalkER7fBase_cb(player, c, 1);
        angle = _ZN8dActor_c18HorzAngleToCPlayerEv(c);
        *(short*)((char*)player + 0x8c) = 0;
        *(short*)((char*)player + 0x8e) = angle + 0x8000;
        *(short*)((char*)player + 0x90) = 0;
    }

    func_ov073_0211f144(c);

    cam = data_0209f318;
    _ZN6Camera9SetFlag_3Ev(cam);

    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;

    look.x = *(int*)(c + 0x5c);
    look.y = *(int*)(c + 0x60);
    look.z = *(int*)(c + 0x64);
    pos.x = *(int*)(c + 0x3d8);
    pos.y = *(int*)(c + 0x3dc);
    pos.z = *(int*)(c + 0x3e0);

    look.y += 0x100000;
    in.z = -0x59c000;

    Matrix4x3_FromRotationY(&data_020a0e68, *(short*)(c + 0x8e));
    MulVec3Mat4x3(&in, &data_020a0e68, &out);

    pos.y -= 0x100000;
    pos.x = pos.x + out.x;
    pos.z = pos.z + out.z;

    _ZN6Camera9SetLookAtERK7Vector3(cam, &look);
    _ZN6Camera6SetPosERK7Vector3(cam, &pos);

    if (*(unsigned short*)(c + 0x100) == 0) {
        ChiefChilly_ChangeState((C *)(c), (PMF *)(&data_ov073_021233e0));
    }
    return 1;
}
}

// @symbol func_ov073_0212000c
extern "C" short func_ov073_0212000c(char *c){
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x30c, data_ov073_02123280[1], 4, 0, 0x1000, 0);
    *(short*)(c+0x100)=0x32;
    return 1;
}

// @symbol func_ov073_0212005c
extern "C" int func_ov073_0212005c(char* c){
  if(((Animation*)(c+0x35c))->Finished()){
    *(unsigned char*)(c+0x4c5)=0xff;
    ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123360));
  }
  return 1;
}

// @symbol func_ov073_02120098
struct BCA_File;
extern "C" {
int func_ov073_02120098(char* c){
  *(char*)(c+0x4ca) = 0;
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x30c, data_ov073_02123290[1], 4, 0x40000000, 0x1000, 0);
  return 1;
}
}

// @symbol func_ov073_021200e0
extern "C" {
int func_ov073_021200e0(u8* thiz)
{
    u16 state = *(u16*)(thiz + 0x100);
    if (state != 0) {
        if (state == 1) {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(thiz + 0x30c, data_ov073_021232b0[1], 4, 0x40000000, 0x1000, 0);
            *(int*)(thiz + 0x368) = 0x1000;
            *(int*)(thiz + 0xa8) = 0x46000;
            *(u8*)(thiz + 0x4c5) = 0xff;
            *(int*)(thiz + 0x9c) = -0x3000;
            if (*(u8*)(thiz + 0x4ca) != 0) {
                func_02012694(0x16f, thiz + 0x74);
            }
        }
        return 1;
    }
    /* state == 0 */
    if (*(int*)(thiz + 0x4b4) == 0) {
        Vec3 offset;
        Vec3 rotated;
        s16 horz;
        _Z14ApproachLinearRsss((short*)(thiz + 0x8c), 0, 0x2000);
        horz = Vec3_HorzAngle((const Vector3 *)(thiz + 0x5c), (const Vector3 *)(thiz + 0x3d8));
        Vec3_VertAngle((const void*)(thiz + 0x5c), (const void*)(thiz + 0x3d8));
        _Z14ApproachLinearRsss((short*)(thiz + 0x94), horz, 0x800);
        offset.x = 0;
        offset.y = 0;
        offset.z = 0;
        rotated.x = 0;
        rotated.y = 0;
        rotated.z = 0;
        if (*(u8*)(thiz + 0x4cb) == 2) {
            offset.z = 0x32000;
        } else {
            offset.z = 0x1e000;
        }
        Matrix4x3_FromRotationY(&data_020a0e68, horz);
        MulVec3Mat4x3(&offset, &data_020a0e68, &rotated);
        *(int*)(thiz + 0xa4) = rotated.x;
        *(int*)(thiz + 0xac) = rotated.z;
        if (*(int*)(thiz + 0xa8) < 0) {
            int id = *(int*)(thiz + 0x134);
            if (id != 0) {
                void* actor = _ZN8dActor_c10FindWithIDEj((u32)id);
                if (actor != 0) {
                    enum Bool eq = (enum Bool)(*(u16*)((u8*)actor + 0xc) == 0xbf);
                    if (eq != FALSE) {
                        Vec3 pos = *(Vec3*)((u8*)actor + 0x5c);
                        if (*(int*)(thiz + 0x60) > pos.y) {
                            _ZN6Player12Unk_020c6a10Ej(actor, 1);
                        }
                    }
                }
            }
        }
    }
    /* 0x1a8 */
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(thiz + 0x150) != 0) {
        if (*(int*)(thiz + 0x4b4) == 0) {
            *(int*)(thiz + 0x98) = 0;
            *(int*)(thiz + 0xa4) = 0;
            *(int*)(thiz + 0xa8) = 0;
            *(int*)(thiz + 0xac) = 0;
            func_ov073_0211f2c0(thiz, 0x1388000);
            func_02012694(0x16d, thiz + 0x74);
        }
        if (*(u8*)(thiz + 0x4cb) == 1 && *(u8*)(thiz + 0x4ca) == 0) {
            *(u8*)(thiz + 0x4c5) = 0;
            if (*(int*)(thiz + 0x4b4) == 0) {
                _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(thiz + 0x30c, data_ov073_02123290[1], 4, 0x40000000, 0x1000, 0);
            }
            *(int*)(((int)thiz + 0x4b4)) += 1;
            if (*(int*)(thiz + 0x4b4) < 0x82) {
                return 1;
            }
        }
        ChiefChilly_ChangeState((C *)(thiz), (PMF *)(data_ov073_02123380));
    }
    *(s16*)(thiz + 0x8e) = *(s16*)(thiz + 0x94);
    return 1;
}
}

// @symbol func_ov073_02120390
extern "C" {
int func_ov073_02120390(char *p)
{
    *(int *)(p + 0x4b4) = 0;
    *(short *)(p + 0x100) = 10;
    return 1;
}
}

// @symbol func_ov073_021203ac
extern "C" {
int func_ov073_021203ac(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    int *src;
    int v[3];

    if (*(unsigned short *)(c + 0x100) == 0)
        goto mainblock;

    if (*(unsigned short *)(c + 0x100) == 1) {

        int neg = 0x5000;
        *(int *)(c + 0xa8) = 0x5a000;
        *(int *)(c + 0x9c) = -neg;
        {
            unsigned char *p = (unsigned char *)(c + 0x4c4);
            *p = *p + 1;
            *p = *p & 7;
        }

        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x30c, data_ov073_021232a8[1], 4, 0, 0x1000, 0);
        *(int *)(c + 0x368) = 0x2000;
    }
    return 1;

mainblock:
    if (*(unsigned char *)(c + 0x4cb) == 2) {
        src = (int *)(c + 0x448 + *(unsigned char *)(c + 0x4c4) * 0xc);
        v[0] = src[0];
        v[1] = src[1];
        v[2] = src[2];
    } else {
        src = (int *)(c + 0x3e8 + *(unsigned char *)(c + 0x4c4) * 0xc);
        v[0] = src[0];
        v[1] = src[1];
        v[2] = src[2];
    }
    {
        short hz = Vec3_HorzAngle((const Vector3 *)(c + 0x5c), (const Vector3 *)v);
        short vt = Vec3_VertAngle(c + 0x5c, v);
        int in[3];
        int out[3];
        _Z14ApproachLinearRsss((short *)(c + 0x94), hz, 0x800);
        in[0] = 0; in[1] = 0; in[2] = 0;
        out[0] = 0; out[1] = 0; out[2] = 0;
        in[2] = 0x50000;
        Matrix4x3_FromRotationY(&data_020a0e68, hz);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, vt);
        MulVec3Mat4x3(in, &data_020a0e68, out);
        *(int *)(c + 0xa4) = out[0];
        *(int *)(c + 0xac) = out[2];
    }
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x150) != 0) {
        *(unsigned char *)(c + 0x4c5) = *(unsigned char *)(c + 0x4c4);
        *(int *)(c + 0x98) = 0;
        *(int *)(c + 0xa4) = 0;
        *(int *)(c + 0xa8) = 0;
        *(int *)(c + 0xac) = 0;
        func_ov073_0211f2c0(c, 0x7d0000);
        func_02012694(0x16d, c + 0x74);
        {
            int *cnt = (int *)(c + 0x4b8);
            *cnt = *cnt + 1;
        }
        if (*(int *)(c + 0x4b8) > 7) {
            ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123340));
        } else {
            ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123320));
        }
    }
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
    return 1;
}
}

// @symbol func_ov073_021205f0
extern "C" {
int func_ov073_021205f0(char *p)
{
    *(int *)(p + 0x368) = 0;
    *(int *)(p + 0x4b4) = 0;
    *(short *)(p + 0x100) = 2;
    return 1;
}
}

// @symbol func_ov073_02120610
extern "C" {
    int func_ov073_02120610(void *thiz)
    {
        unsigned char *c = (unsigned char *)thiz;
        int *src;
        int v[3];

        if (*(unsigned short *)(c + 0x100) == 0)
            goto mainblock;

        if (*(unsigned short *)(c + 0x100) == 1) {
            *(int *)(c + 0xa8) = 0x5a000;
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x30c, data_ov073_021232a8[1], 4, 0, 0x1000, 0);
            *(int *)(c + 0x368) = 0x2000;
        }
        return 1;

    mainblock:
        _Z14ApproachLinearRsss((short *)(c + 0x8c), 0, 0x2000);
        if (*(unsigned char *)(c + 0x4cb) == 2) {
            src = (int *)(c + 0x448 + *(unsigned char *)(c + 0x4c4) * 0xc);
            v[0] = src[0];
            v[1] = src[1];
            v[2] = src[2];
        } else {
            src = (int *)(c + 0x3e8 + *(unsigned char *)(c + 0x4c4) * 0xc);
            v[0] = src[0];
            v[1] = src[1];
            v[2] = src[2];
        }
        {
            short hz = Vec3_HorzAngle((const Vector3 *)(c + 0x5c), (const Vector3 *)v);
            short vt = Vec3_VertAngle(c + 0x5c, v);
            int in[3];
            int out[3];
            in[2] = 0;
            in[0] = 0; in[1] = 0; out[0] = 0; out[1] = 0; out[2] = 0;
            in[2] = 0x28000;
            Matrix4x3_FromRotationY(&data_020a0e68, hz);
            Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, vt);
            MulVec3Mat4x3(in, &data_020a0e68, out);
            *(int *)(c + 0xa4) = out[0];
            if (*(int *)(c + 0x4b4) != 0 || *(int *)(c + 0xa8) < 0) {
                *(int *)(c + 0xa8) = out[1];
                *(int *)(c + 0x4b4) = 1;
            }
            *(int *)(c + 0xac) = out[2];
        }
        if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x150) != 0) {
            *(unsigned char *)(c + 0x4c5) = *(unsigned char *)(c + 0x4c4);
            *(int *)(c + 0x98) = 0;
            *(int *)(c + 0xa4) = 0;
            *(int *)(c + 0xa8) = 0;
            *(int *)(c + 0xac) = 0;
            func_ov073_0211f2c0(c, 0x7d0000);
            func_02012694(0x16d, c + 0x74);
            *(int *)(c + 0x4b8) = 1;
            ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123320));
        }
        *(short *)(c + 0x8e) = *(short *)(c + 0x94);
        return 1;
    }
}

// @symbol func_ov073_0212081c
extern "C" {
int func_ov073_0212081c(char* self)
{
    *(unsigned short*)(self + 0x100) = 0xa;
    *(int*)(self + 0x4b4) = 0;
    *(int*)(self + 0x4b8) = 0;
    *(unsigned char*)(self + 0x4c5) = 0xff;
    return 1;
}
}

// @symbol func_ov073_02120844
extern "C" {
int func_ov073_02120844(int *t)
{
    _Z14ApproachLinearRsss((short *)((char*)t + 0x8c), -0x4000, 0x400);
    t[0x140] = _ZN5Sound8PlayLongEjjjRK7Vector3s(t[0x140], 3, 0x170, (struct Vector3 *)((char*)t + 0x74), 0);
    if (t[0xf7] > t[0x18] && _ZNK10dBgCh_Actr10IsOnGroundEv((char*)t + 0x150)) {
        func_ov073_0211f2c0(t, 0x7d0000);
        func_02012694(0x16c, (char*)t + 0x74);
        t[0x26] = 0;
        ChiefChilly_ChangeState((C *)(t), (PMF *)(data_ov073_021233b0));
    }
    return 1;
}
}

// @symbol func_ov073_021208e4
extern "C" {
int func_ov073_021208e4(char *r0) {
    *(int *)(r0 + 0x98) = 0x14000;
    *(int *)(r0 + 0xa8) = 0x1e000;
    *(int *)(r0 + 0x9c) = -0x3000;
    *(int *)(r0 + 0x500) = 0;
    return 1;
}
}

// @symbol func_ov073_02120910
extern "C" {
int func_ov073_02120910(char *c)
{
    Vector3 in;
    Vector3 out;

    *(int *)(c + 0x9c) = 0;
    *(int *)(c + 0x98) = 0;
    *(int *)(c + 0xa4) = 0;
    *(int *)(c + 0xa8) = 0;
    *(int *)(c + 0xac) = 0;

    if (*(unsigned short *)(c + 0x100) == 0) {
        *(c + 0x4cb) = 1;
        ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123340));
        *(c + 0x4ca) = 1;
        *(short *)(c + 0x94) = *(short *)(c + 0x8e);
        return 1;
    }

    *(unsigned int *)(c + 0x500) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(unsigned int *)(c + 0x500), 3, 0x16e, (Vector3 *)(c + 0x74), 0);

    in.z = 0;
    in.z = 0x78000;
    in.x = 0;
    in.y = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    Matrix4x3_FromRotationY(&data_020a0e68, *(short *)(c + 0x8e) + 0x8000);
    MulVec3Mat4x3(&in, &data_020a0e68, &out);
    in.x = *(int *)(c + 0x4a8) + out.x;
    in.y = *(int *)(c + 0x4ac);
    in.z = *(int *)(c + 0x4b0) + out.z;
    Vec3_ApproachHorz(c + 0x5c, &in, 0x14000);
    *(short *)(c + 0x8e) = Vec3_HorzAngle((const Vector3 *)(c + 0x5c), (const Vector3 *)(c + 0x3d8));
    *(short *)(c + 0x94) = *(short *)(c + 0x8e) + 0x8000;
    *(int *)(((int)c + 0x4b4)) += 0x500;
    _Z14ApproachLinearRsss((short *)(c + 0x8c),
        ((s64)*(int *)(c + 0x4d0) * data_02082214[((unsigned short)(short)*(int *)(c + 0x4b4) >> 4) * 2] + 0x800) >> 12,
        0x400);
    if (func_ov073_0211f61c(c)) {
        ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_021233f0));
    }
    return 1;
}
}

// @symbol func_ov073_02120ad8
extern "C" {
int func_ov073_02120ad8(int *t)
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((char *)t + 0x30c, data_ov073_02123288[1], 4, 0, 0x1000, 0);
    t[0xda] = 0x4000;
    *(short *)((char *)t + 0x8c) = 0;
    t[0x12d] = 0;
    t[0x27] = 0;
    t[0x26] = 0;
    t[0x29] = 0;
    t[0x2a] = 0;
    t[0x2b] = 0;
    t[0x134] = -0x1000;
    t[0x12a] = t[0x13b];
    t[0x12b] = t[0x13c];
    t[0x12c] = t[0x13d];
    t[0x140] = 0;
    *(short *)((char *)t + 0x100) = 0x5a;
    return 1;
}
}

// @symbol func_ov073_02120b78
extern "C" {
int func_ov073_02120b78(char* c){
    _Z14ApproachLinearRsss((short*)(c+0x8c), -0x4000, 0x400);
    if(*(int*)(c+0x3dc) > *(int*)(c+0x60)){
        if(_ZNK10dBgCh_Actr10IsOnGroundEv(c+0x150)){
            func_ov073_0211f2c0(c, 0xfa0000);
            *(int*)(c+0x98) = 0;
            *(short*)(c+0x94) = Vec3_HorzAngle((const Vector3 *)(c+0x5c), (const Vector3 *)(c+0x3d8));
            *(short*)(c+0x8e) = *(short*)(c+0x94);
            func_02012694(0x16c, c+0x74);
            ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123400));
        }
    }
    return 1;
}
}

// @symbol func_ov073_02120c08
extern "C" {
int func_ov073_02120c08(int *t)
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((char *)t + 0x30c, data_ov073_02123288[1], 4, 0, 0x1000, 0);
    t[0xda] = 0x4000;
    t[0x26] = 0x14000;
    t[0x2a] = 0x1e000;
    *(short *)((char *)t + 0x8c) = 0;
    t[0x27] = -0x3000;
    t[0x12d] = 0;
    return 1;
}
}

// @symbol func_ov073_02120c7c
extern "C" int func_ov073_02120c7c(CB* c)
{
    int a = c->field_98; if (a < 0) a = -a;
    if (a > 0xa) {
        int i = 0;
        CB* p = c;
        do {
            _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(p->field_4d4, p->field_4d8, p->field_4dc);
            i++;
            p = (CB*)((char*)p + 0xc);
        } while (i < 2);
    }
    if (c->field_4c9 == 1) {
        *(unsigned char*)((int)c + 0x4cb) -= 1;
        if (c->field_4cb != 0)
            ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_021233c0));
        else
            ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_021233d0));
        return 1;
    }
    _Z14ApproachLinearRiii(&c->field_98, 0, 0x1000);
    if (_ZN9Animation8FinishedEv(&c->anim_35c)) {
        int b = c->field_98; if (b < 0) b = -b;
        if (b < 0xa) {
            c->field_98 = 0;
            c->field_94 = c->field_8e;
            ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123360));
        }
    }
    return 1;
}

// @symbol func_ov073_02120d80
extern "C" {

int func_ov073_02120d80(char *c)
{
    int fix;
    unsigned short t;
    short ang;

    fix = 0x1000;
    t = 0;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x30c, data_ov073_021232a8[1], 4, 0x40000000, fix, t);
    *(int *)(c + 0x368) = fix;
    ang = _ZN8dActor_c18HorzAngleToCPlayerEv(c);
    *(short *)(c + 0x94) = ang;
    *(short *)(((int)c + 0x94)) =
        (short)((int)*(short *)(((int)c + 0x94)) + 0x8000);
    return 1;
}
}

// @symbol func_ov073_02120dec
extern "C" {
int func_ov073_02120dec(char *c) {
    _Z14ApproachLinearRsss((short *)(c + 0x94), *(short *)(c + 0x4c6), 0x500);
    if (AngleDiff(*(short *)(c + 0x4c6), *(short *)(c + 0x8e)) < 0x100) {
        ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123360));
        *(short *)(c + 0x100) = 0x1e;
    }
    *(short *)(c + 0x8c) = *(short *)(c + 0x92);
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
    *(short *)(c + 0x90) = *(short *)(c + 0x96);
    return 1;
}
}

// @symbol func_ov073_02120e60
extern "C" {
int func_ov073_02120e60(char* c){
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x30c, data_ov073_02123288[1], 4, 0, 0x1000, 0);
  *(int*)(c+0x368) = 0x2000;
  *(short*)(c+0x4c6) = Vec3_HorzAngle((const Vector3 *)(c+0x5c), (const Vector3 *)(c+0x3d8));
  *(int*)(c+0x4b4) = 0;
  *(short*)(c+0x94) = *(short*)(c+0x8e);
  return 1;
}
}

// @symbol func_ov073_02120ed0
extern "C" {
int func_ov073_02120ed0(void *self)
{
    u8 *c = (u8 *)self;

    if (c[0x4c9] == 1) {
        ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123390));
        return 1;
    }

    switch (c[0x4c8]) {
    case 0:
        if (*(u16 *)(c + 0x100) == 0) {
            *(s16 *)(c + 0x4c6) = _ZN8dActor_c18HorzAngleToCPlayerEv(c);
            *(int *)(c + 0x4b4) = 0;
            if (AngleDiff(*(s16 *)(c + 0x94), *(s16 *)(c + 0x4c6)) <= 0x2000) {
                *(s16 *)(c + 0x100) = 0x1e;
                break;
            } else {
                *(s16 *)(c + 0x100) = 0;
                *(int *)(c + 0x4d0) = 0x1000;
                (*(u8 *)(((int)c + 0x4c8)))++;
                func_02012694(0x169, c + 0x74);
                break;
            }
        } else {
            _Z14ApproachLinearRiii((int *)(c + 0x98), 0x1e000, 0x3000);
            if (*(int *)(c + 0x4b4) == 0) {
                if (_ZN8dActor_c13DistToCPlayerEv(c) < 0x1f4000) {
                    *(int *)(c + 0x4b4) = 1;
                    if ((((unsigned int)RandomIntInternal(&data_0209e650) >> 0x18) & 7) == 0) {
                        c[0x4c8] = 3;
                        *(int *)(c + 0x98) = 0;
                        *(s16 *)(c + 0x100) = 0xf;
                    }
                }
            }
            _Z14ApproachLinearRsss((s16 *)(c + 0x94), *(s16 *)(c + 0x4c6), 0x1d0);
            *(s16 *)(c + 0x8c) = *(s16 *)(c + 0x92);
            *(s16 *)(c + 0x8e) = *(s16 *)(c + 0x94);
            *(s16 *)(c + 0x90) = *(s16 *)(c + 0x96);
            break;
        }

    case 3:
        if (*(u16 *)(c + 0x100) == 0) {
            *(int *)(c + 0x4d0) = 0x1000;
            c[0x4c8] = 1;
        }
        break;

    case 1:
    case 2:
    {
        int d;
        int i;
        u8 *r5;
        *(int *)(c + 0x4b4) = 0;
        d = *(int *)(c + 0x98);
        if (d < 0) d = -d;
        if (d > 0xa) {
            r5 = c;
            i = 0;
            do {
                _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(
                    *(int *)(r5 + 0x4d4), *(int *)(r5 + 0x4d8), *(int *)(r5 + 0x4dc));
                i++;
                r5 += 0xc;
            } while (i < 2);
        }
        _Z14ApproachLinearRiii((int *)(c + 0x98), 0, *(int *)(c + 0x4d0));
        if (c[0x4c8] == 1) {
            *(s16 *)(c + 0x4c6) = _ZN8dActor_c18HorzAngleToCPlayerEv(c);
            _Z14ApproachLinearRsss((s16 *)(c + 0x8e), *(s16 *)(c + 0x4c6), 0x500);
        }
        d = *(int *)(c + 0x98);
        if (d < 0) d = -d;
        if (d >= 0xa) break;
        if (*(u16 *)(c + 0x100) != 0) break;
        if (c[0x4c8] == 2) {
            u8 *p = (u8 *)_ZN8dActor_c13ClosestPlayerEv(c);
            if (p != 0) {
                int t;
                if (_ZN6Player12GetHurtStateEv(p) == 4) goto hz;
                if (_ZN6Player12GetHurtStateEv(p) == 5) goto hz;
                t = p[0x709] ? 1 : 0;
                if (t == 1) {
hz:
                    *(s16 *)(c + 0x4c6) = Vec3_HorzAngle((Vector3 *)(c + 0x5c), (Vector3 *)(c + 0x3d8));
                } else {
                    *(s16 *)(c + 0x4c6) = _ZN8dActor_c18HorzAngleToCPlayerEv(c);
                }
            }
        }
        *(int *)(c + 0x98) = 0;
        *(s16 *)(c + 0x94) = *(s16 *)(c + 0x8e);
        *(s16 *)(c + 0x100) = 0x1e;
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
            c + 0x30c, data_ov073_02123288[1], 4, 0, 0x1000, 0);
        *(int *)(c + 0x368) = 0x2000;
        *(int *)(c + 0x4b4) = 0;
        c[0x4c8] = 0;
        break;
    }
    }

    if (c[0x4c8] == 0) {
        if (_ZNK9Animation12WillHitFrameEi(c + 0x35c, 0) != 0 ||
            _ZNK9Animation12WillHitFrameEi(c + 0x35c, 0xe) != 0) {
            func_ov073_0211f2c0(c, 0x3e8000);
            func_02012694(0x168, c + 0x74);
        }
    }
    return 1;
}
}

// @symbol func_ov073_0212122c
extern "C" {
int func_ov073_0212122c(int *t)
{
    *(short *)((char *)t + 0x100) = 0;
    *(char *)((char *)t + 0x4c8) = 0;
    t[0x134] = 0x2000;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((char *)t + 0x30c, data_ov073_02123288[1], 4, 0, 0x1000, 0);
    t[0xda] = 0x2000;
    return 1;
}
}

// @symbol func_ov073_0212128c
extern "C" {
int func_ov073_0212128c(char* c)
{
    struct Vector3 la;
    struct Vector3 ps;
    void* cam;
    void* player;

    cam = data_0209f318;
    player = *(void**)(c + 0x3e4);
    _ZN6Camera9SetFlag_3Ev(cam);

    la.x = *(int*)(c + 0x5c);
    la.y = *(int*)(c + 0x60);
    la.z = *(int*)(c + 0x64);
    ps.x = *(int*)(c + 0x5c);
    ps.y = *(int*)(c + 0x60);
    ps.z = *(int*)(c + 0x64);
    la.y = la.y + 0x70000;
    la.z = la.z - 0x2a0000;
    ps.x = ps.x - 0x300000;
    ps.y = ps.y + 0x20000;
    ps.z = ps.z + 0xffa34000;

    _ZN6Camera9SetLookAtERK7Vector3(cam, &la);
    _ZN6Camera6SetPosERK7Vector3(cam, &ps);

    if (_ZN6Player12GetTalkStateEv(player) == -1) {
        *(int*)(((int)cam + 0x154)) &= ~8;
        _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2d);
        func_02011d08();
        ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123360));
    }
    return 1;
}
}

// @symbol func_ov073_02121378
extern "C" {
int func_ov073_02121378(char *p)
{
    p[1224] = 0; return 1;
}
}

// @symbol func_ov073_02121388
extern "C" {
int func_ov073_02121388(char* c) {
    struct Vector3 vplayer;
    struct Vector3 vmsg;
    struct Vector3 la;
    struct Vector3 ps;
    void* player;
    void* cam;
    int msg;
    char* p;

    player = _ZN8dActor_c13ClosestPlayerEv(c);
    if (player == 0) return 1;

    {
        int pv = (int)((int)player + 0x5c);
        vplayer.x = *(int*)pv;
        vplayer.y = *(int*)(pv + 4);
        vplayer.z = *(int*)(pv + 8);
    }
    cam = data_0209f318;
    vmsg.x = *(int*)(c + 0x5c);
    vmsg.y = *(int*)(c + 0x60);
    vmsg.z = *(int*)(c + 0x64);

    _ZN6Camera9SetFlag_3Ev(cam);

    la.x = *(int*)(c + 0x5c);
    la.y = *(int*)(c + 0x60);
    la.z = *(int*)(c + 0x64);
    ps.x = *(int*)(c + 0x5c);
    ps.y = *(int*)(c + 0x60);
    ps.z = *(int*)(c + 0x64);
    la.y = la.y + 0x70000;
    la.z = la.z - 0x2a0000;
    ps.x = ps.x - 0x300000;
    ps.y = ps.y + 0x20000;
    ps.z = ps.z + 0xffa34000;

    _ZN6Camera9SetLookAtERK7Vector3(cam, &la);
    _ZN6Camera6SetPosERK7Vector3(cam, &ps);

    vmsg.y = vmsg.y + 0x64000;
    _Z14ApproachLinearRsss((short*)(c + 0x94), Vec3_HorzAngle((struct Vector3*)(c + 0x5c), &vplayer), 0x800);

    *(void**)(c + 0x3e4) = player;
    p = *(char**)(c + 0x3e4);
    msg = (short)(*(int*)(p + 8) + 0xe3);
    if (_ZN6Player9StartTalkER7fBase_cb(p, c, 1)) {
        if (*(unsigned char*)(c + 0x4c8) == 0) {
            _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2c);
            *(unsigned char*)(c + 0x4c8) = 0;
        }

        if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(*(void**)(c + 0x3e4), c, msg, &vmsg, 0, 2)) {
            func_02012694(0x12a, c + 0x74);
            ChiefChilly_ChangeState((C *)(c), (PMF *)(data_ov073_02123350));
        }
    }
    return 1;
}
}

// @symbol func_ov073_02121538
extern "C" {
int func_ov073_02121538(char *c){
  *(char*)(c+0x4c8)=0;
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x30c, data_ov073_021232b8[1], 4, 0, 0x1000, 0);
  return 1;
}
}

// @symbol ChiefChilly_ChangeState
extern "C" int ChiefChilly_ChangeState(C *c, PMF *p) { c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }

// @symbol func_ov073_021215cc
extern "C" {
void func_ov073_021215cc(void* self)
{
    char* c = (char*)self;
    int sh;
    Vec3 v;
    Vec3 out;
    void* m;
    Mtx43* saved;
    int i;
    Vec3* p;
    char* q;

    Vec3_Asr(&v, (Vec3*)(c + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, *(s16*)(c + 0x8c), *(s16*)(c + 0x8e), *(s16*)(c + 0x90));

    p = (Vec3*)(c + 0x4d4);
    *(struct Matrix4x3*)(c + 0x328) = data_020a0e68;
    saved = (Mtx43*)(c + 0x328);
    sh = 3;
    i = 0;
    q = c;
    for (; i < 2; i++) {
        data_020a0e68 = *(struct Matrix4x3 *)saved;
        MulMat4x3Mat4x3(*(Mtx43**)(c + 0x320) + (i + 2), &data_020a0e68, &data_020a0e68);
        p->x = data_020a0e68.m[9];
        p->y = data_020a0e68.m[10];
        p->z = data_020a0e68.m[11];
        Vec3_Lsl(&out, p, sh);
        *(int*)(q + 0x4d4) = out.x;
        *(int*)(q + 0x4d8) = out.y;
        *(int*)(q + 0x4dc) = out.z;
        p++;
        q += 0xc;
    }

    m = *(void**)(c + 0x37c);
    if (m == (void*)data_ov073_021233b0) return;
    if (m == (void*)&data_ov073_021233e0) return;
    if (m == (void*)&data_ov073_02123410) return;
    if (m == (void*)data_ov073_021233f0) return;
    if (m == (void*)data_ov073_02123370) return;

    Matrix4x3_FromTranslation(&data_020a0e68, *(int*)(c + 0x5c) >> 3, (*(int*)(c + 0x60) - 0xa000) >> 3, *(int*)(c + 0x64) >> 3);
    *(struct Matrix4x3*)(c + 0x3a8) = data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x380, c + 0x3a8, 0x12c000, 0x3e8000, 0xf);
}
}

// @symbol _ZN16daKing_Donketu_c16CleanupResourcesEv
int daKing_Donketu_c::CleanupResources(){
  UnloadKeyModels(4);
  ((SharedFilePtr *)data_ov073_02123280)->Release();
  ((SharedFilePtr *)data_ov073_021232a0)->Release();
  ((SharedFilePtr *)data_ov073_02123288)->Release();
  ((SharedFilePtr *)data_ov073_021232a8)->Release();
  ((SharedFilePtr *)data_ov073_02123290)->Release();
  ((SharedFilePtr *)data_ov073_021232b0)->Release();
  ((SharedFilePtr *)data_ov073_021232b8)->Release();
  ((SharedFilePtr *)data_ov002_0210da30)->Release();
  ((SharedFilePtr *)data_ov073_02123298)->Release();
  return 1;
}

// @symbol _ZN16daKing_Donketu_c16OnPendingDestroyEv
void daKing_Donketu_c::OnPendingDestroy()
{
}

// @symbol _ZN16daKing_Donketu_c6RenderEv
int daKing_Donketu_c::Render()
{
  ((Base *)&mBlendModelAnim)->M((char*)&mScaleX);
  return 1;
}

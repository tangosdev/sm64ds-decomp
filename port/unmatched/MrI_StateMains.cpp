/* HOST COPIES of the two MR. I bodies src/ has NO TU for: the per-frame MAIN
 * halves of MrI's states 1 and 2.
 *
 *   func_ov071_021211e0  ov071 0x021211e0, 0x314 bytes  -- state 1 MAIN, the
 *                        chase/aim/fire core
 *   func_ov071_02120d30  ov071 0x02120d30, 0x3dc bytes  -- state 2 MAIN, the
 *                        spin-down / pop / reward core
 *
 * WHY HOST COPIES AND NOT SLICE LINES: there is nothing to slice. Every other
 * body in MrI's block (ROM 0x02120668..0x02121a6c) has a src/ TU; these two
 * alone do not. Measured, not carried: config/arm9/overlays/ov071/delinks.txt
 * has no entry whose `.text start:` is either address, and the set difference
 * between the func_ov071_* names in config/arm9/overlays/ov071/symbols.txt and
 * the func_ov071_* files in src/ is exactly {func_ov071_02120d30,
 * func_ov071_021211e0} -- two names, no others.
 *
 * ---- HOW THE SPANS WERE RE-DERIVED ----------------------------------------
 * NOT carried from the brief. config/arm9/overlays/ov071/symbols.txt:
 *     func_ov071_02120d30 kind:function(arm,size=0x3dc) addr:0x02120d30
 *     func_ov071_021211e0 kind:function(arm,size=0x314) addr:0x021211e0
 * and each size closes exactly on the next symbol (0x0212110c and 0x021214f4).
 *
 * THE OVERLAY BASE IS 0x0211f000 -- which IS overlays.yaml's
 * `base_address: 34729984` (34729984 == 0x0211f000; the yaml is correct).
 * A wrong hand-converted hex gloss (0x0211f600) circulated in this lane's
 * brief; reading bytes at THAT base would have produced garbage for every
 * listing below. The base is pinned three
 * ways, all agreeing: config/arm9/overlays/ov071/delinks.txt opens
 * `.text start:0x0211f000` and closes `.bss end:0x02123100`; the image span
 * 0x02122f80 - 0x0211f000 = 0x3f80 = 16256 is exactly the yaml's own
 * `ram_size` AND the byte length of extracted/overlays/overlay_0071.bin; and
 * the bss span 0x02123100 - 0x02122f80 = 0x180 = 384 is exactly the yaml's
 * `bss_size`. At the glossed 0x0211f600 the symbol func_ov071_0211f0a4 would
 * sit BELOW the overlay. File offset used throughout: addr - 0x0211f000.
 *
 * ---- VERIFICATION RECORD ---------------------------------------------------
 * Method: disassemble the ROM span out of extracted/overlays/overlay_0071.bin
 * (capstone, ARM) with config/arm9/overlays/ov071/relocs.txt and both symbol
 * tables applied, then transcribe per instruction. Every constant, member
 * offset, branch target and call target below is read from that listing; none
 * is inferred. Size accounting closes on both:
 *
 *   func_ov071_02120d30: 0x3dc = 0x3a0 code (232 ARM instructions,
 *       0x02120d30..0x021210d0) + 0x3c literal pool (15 words at 0x021210d0)
 *   func_ov071_021211e0: 0x314 = 0x2fc code (191 ARM instructions,
 *       0x021211e0..0x021214dc) + 0x18 literal pool (6 words at 0x021214dc)
 *
 * LITERAL POOL of func_ov071_02120d30 @0x021210d0 -- every word is used below:
 *     021210d0  80008001   the signed magic for the /0x1fffe pair
 *     021210d4  00000119   func_0201267c sound id (the spin loop)
 *     021210d8  0001fffe   the divisor itself
 *     021210dc  0000013a   Particle::System::New effect id
 *     021210e0  0000013b   Particle::System::NewUnkCallback818 effect id
 *     021210e4  00007fff   the +0x50 lifetime both systems get
 *     021210e8  02082214   data_02082214, arm9 s16 [sin, cos] pairs
 *     021210ec  00000215   the +0x215 member offset (DecIfAbove0_Byte)
 *     021210f0  00000106   actor id 262 = MR_I
 *     021210f4  00000107   actor id 263 = BIG_MR_I
 *     021210f8  00000122   actor id 290, the thing MR_I leaves behind
 *     021210fc  00000217   the +0x217 member offset (the star ref)
 *     02121100  00000125   Particle::System::NewSimple effect id
 *     02121104  00000126   Particle::System::NewSimple effect id
 *     02121108  0209f2f8   data_0209f2f8, arm9 bss, the current level
 *   (0x124, the third NewSimple id, is a `mov r0,#0x124` immediate, not pooled.)
 *
 * LITERAL POOL of func_ov071_021211e0 @0x021214dc -- likewise all used:
 *     021214dc  000008fc   the UpdateAngle delta for the Y turn
 *     021214e0  00000213   the +0x213 member offset (DecIfAbove0_Byte)
 *     021214e4  02082214   data_02082214
 *     021214e8  00000107   actor id 263 = BIG_MR_I (selects the big shot)
 *     021214ec  00000165   func_0201267c sound id (the shot)
 *     021214f0  005dc000   the leash distance, Fix12i
 *
 * THE ONE DIVISION. func_ov071_02120d30 spells `x / 0x1fffe` and `x % 0x1fffe`
 * over *(int *)(self+0x1f8) with mwcc's magic-multiply sequence (smull by
 * 0x80008001, add x, asr #16, add the sign bit; the remainder is then
 * x - q*0x1fffe). That sequence was CHECKED to equal C truncating division by
 * 0x1fffe, not assumed: evaluated against the C result over the edge cases
 * (0, +-1, +-131069..131071, +-262139, +-(1<<20), +-0x7fffffff) and 20000
 * random values in +-(1<<28) -- 0 mismatches. So `/` and `%` are the exact
 * transcription and the magic does not need to be reproduced.
 *
 * CALL TARGETS: nineteen distinct, every one resolved BY ADDRESS from the
 * listing, and every arity taken from the ROM's own register/stack use rather
 * than from a C prototype (C linkage would hide an arity mismatch silently):
 *
 *   0203ae58  _Z14ApproachLinearRiii      (int*, int, int) -> int, r0..r2
 *   0203af90  _Z11UpdateAngleRssis        (short*, short, int, short), r0..r3
 *   0203add4  DecIfAbove0_Byte            (u8*) -> int, r0
 *   0203b7ac  Vec3_HorzAngle              (Vector3*, Vector3*) -> s16
 *   0203b770  Vec3_VertAngle              (Vector3*, Vector3*) -> s16
 *   0203cfdc  Vec3_Dist                   (Vector3*, Vector3*) -> int
 *   0201267c  func_0201267c               (int, void*), r0/r1
 *   02022d80  Particle::System::New              7 args: r0..r3 + sp[0,4,8]
 *   02022bb4  Particle::System::NewUnkCallback818 6 args: r0..r3 + sp[0,4]
 *   02022e68  Particle::System::FromUniqueID     (u32) -> void*, r0
 *   02022e98  Particle::System::NewSimple        (u32, int, int, int), r0..r3
 *   02010e2c  Actor::Spawn                6 args: r0..r3 + sp[0,4]
 *   0200fe3c  Actor::PoofDust             (this), r0
 *   0200ff14  Actor::UntrackAndSpawnStar  5 args: r0..r3 + sp[0]
 *   0200f9b8  Actor::KillAndTrackInDeathTable (this), r0
 *   02043824  ActorBase::MarkForDestruction   (this), r0
 *   02015c3c  Animation::Advance          (this), r0
 *   02015bcc  Animation::Finished         (this) -> int, r0
 *   0200f658  Actor::DetectRaycastClsn    (this, Vector3*, Vector3*, bool)
 *   plus six ov071 siblings, all matched src and all in this lane's slice:
 *   02120a20, 021209c8, 02120860, 0212070c, 02120b14, 02121634.
 *
 * The two arm9 data symbols were already mounted before this lane: the trig
 * table as `short data_02082214[]` and the level byte as
 * `signed char data_0209f2f8` (the spellings unmatched/Flamethrower_Behavior.c
 * and unmatched/Actor_ClosestPlayer_OverlayReaders.cpp already use).
 *
 * HOST COPY, not matched src: when the decomp banks either real TU, that body
 * retires for a slice line.
 */
#include <cstdio>

extern "C" {

/* ---- arm9 helpers, signatures pinned by the ROM's register use ------------ */
int  _Z14ApproachLinearRiii(int *cur, int target, int step);
void _Z11UpdateAngleRssis(short *cur, short target, int rate, short delta);
int  DecIfAbove0_Byte(unsigned char *p);
short Vec3_HorzAngle(const void *a, const void *b);
short Vec3_VertAngle(const void *a, const void *b);
int  Vec3_Dist(const void *a, const void *b);
void func_0201267c(int id, void *p);

unsigned _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned uniqueID, unsigned effectID, int x, int y, int z,
    const void *dir, void *callback);
void *_ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
    unsigned uniqueID, unsigned effectID, int x, int y, int z, const void *dir);
void *_ZN8Particle6System12FromUniqueIDEj(unsigned uniqueID);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned id, int x, int y,
                                                    int z);

void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned actorID,
                                                   unsigned param1,
                                                   const void *pos,
                                                   const void *rot,
                                                   int areaID, int deathTableID);
void _ZN5Actor8PoofDustEv(void *self);
void _ZN5Actor19UntrackAndSpawnStarERajRK7Vector3j(void *self, signed char *ref,
                                                   unsigned b, const void *pos,
                                                   unsigned j);
void _ZN5Actor24KillAndTrackInDeathTableEv(void *self);
void _ZN9ActorBase18MarkForDestructionEv(void *self);
void _ZN9Animation7AdvanceEv(void *anim);
int  _ZN9Animation8FinishedEv(void *anim);
int  _ZN5Actor17DetectRaycastClsnER7Vector3S1_b(void *self, void *from,
                                                void *to, bool flag);

extern short data_02082214[];     /* arm9: s16 [sin, cos] pairs, 4096 = 1.0 */
extern signed char data_0209f2f8; /* arm9 bss: the current level */

/* ---- MrI's own siblings, all matched src (this lane's slice) -------------- */
int  func_ov071_02120a20(void *self);
void func_ov071_021209c8(void *self);
void func_ov071_02120860(void *self);
int  func_ov071_0212070c(void *self);
void func_ov071_02120b14(void *self);
void func_ov071_02121634(void *self, int state);   /* MrI_SetState */

int func_ov071_021211e0(char *self);
int func_ov071_02120d30(char *self);

}  /* extern "C" */

namespace {
struct V3  { int x, y, z; };
struct V16 { short x, y, z; };

/* The ROM's Fix12 multiply-accumulate: a 32x32 -> 64 signed product, rounded
   with +0x800 and taken >> 12 (the `umull/mla/mla/adds/adc/lsr/orr` run). */
inline int fx12(long long a, long long b)
{
    return (int)((a * b + 0x800) >> 12);
}
}  /* namespace */

/* =========================================================================
 * func_ov071_021211e0 -- ov071 0x021211e0, 0x314 bytes, MrI state 1 MAIN.
 *
 * Reached every frame while MrI is awake and hunting: it turns the eye toward
 * the tracked actor at +0x1ec, and when the wind-up counter at +0x214 hits 7
 * it fires actor 0x108 (264, MR_I_PROJECTILE) out of a muzzle point pushed
 * 0x50000 forward along the Y angle. Then it runs the shared movement helper
 * and drops back to state 0 on any of three leash conditions, or to state 2
 * when func_ov071_0212070c reports the hit.
 *
 * Stack frame in the ROM is 0x44: sp+0x08 V16 shot angles, sp+0x10 V3 target,
 * sp+0x1c V3 aim, sp+0x28 V3 muzzle, sp+0x34 V3 ray end; sp+0x00/0x04 are the
 * outgoing stack halves of Actor::Spawn. The five locals below are those.
 * ========================================================================= */
int func_ov071_021211e0(char *self)
{
    V3  target;   /* sp+0x10 */
    V16 shotAng;  /* sp+0x08 */
    V3  aim;      /* sp+0x1c */
    V3  muzzle;   /* sp+0x28 */
    V3  rayEnd;   /* sp+0x34 */
    char *t;

    /* 021211ec..02121210: target = tracked->pos, lifted 0x78000 in Y */
    t = *(char **)(self + 0x1ec);
    target.x = *(int *)(t + 0x5c);
    target.y = *(int *)(t + 0x60) + 0x78000;
    target.z = *(int *)(t + 0x64);

    /* 02121214: the ROM calls Vec3_HorzAngle here and DISCARDS r0 -- r0 is
       overwritten at 0212121c before any use. Kept so the call sequence is the
       ROM's; the function is pure, so keeping it is behaviourally free. */
    (void)Vec3_HorzAngle(self + 0x5c, &target);

    /* 02121220..02121234: pitch toward the target, rate 2, delta 0x320 */
    _Z11UpdateAngleRssis((short *)(self + 0x8c),
                         Vec3_VertAngle(self + 0x5c, &target), 2, 0x320);
    /* 02121240..02121254: yaw toward the target, rate 2, delta 0x8fc */
    _Z11UpdateAngleRssis((short *)(self + 0x8e),
                         Vec3_HorzAngle(self + 0x5c, &target), 2, 0x8fc);

    /* 02121258..021212a0: once the yaw has settled onto the stored facing,
       tick the +0x213 timer down and re-arm from func_ov071_02120a20 */
    if ((short)(*(short *)(self + 0x8e) - *(short *)(self + 0x20c)) == 0) {
        if (DecIfAbove0_Byte((unsigned char *)(self + 0x213)) == 0) {
            if (func_ov071_02120a20(self))
                *(unsigned char *)(self + 0x213) = 0x53;
        }
        *(unsigned char *)(self + 0x212) = 0xf0;
    }

    /* 021212a4..02121408: the shot, only on wind-up step 7 */
    if (*(unsigned char *)(self + 0x214) == 7) {
        int ay, sinv, cosv;

        /* 021212b0..021212d0: aim point = tracked->pos lifted 0x4b000 */
        t = *(char **)(self + 0x1ec);
        aim.x = *(int *)(t + 0x5c);
        aim.y = *(int *)(t + 0x60) + 0x4b000;
        aim.z = *(int *)(t + 0x64);

        /* 021212d4..021212f4: muzzle starts at MrI's own position */
        muzzle.x = *(int *)(self + 0x5c);
        muzzle.y = *(int *)(self + 0x60);
        muzzle.z = *(int *)(self + 0x64);

        /* 021212f8..02121314: the shot inherits MrI's three angles */
        shotAng.x = *(short *)(self + 0x8c);
        shotAng.y = *(short *)(self + 0x8e);
        shotAng.z = *(short *)(self + 0x90);

        /* 02121318..0212138c: push the muzzle 0x50000 along the Y angle.
           The ROM indexes data_02082214 as [sin, cos] pairs: sin at
           (angY >> 4) * 2, cos at (angY >> 4) * 2 + 1, angY read UNSIGNED. */
        ay = *(unsigned short *)(self + 0x8e);
        sinv = data_02082214[(ay >> 4) * 2];
        cosv = data_02082214[((ay >> 4) * 2) + 1];
        muzzle.x = *(int *)(self + 0x5c) + fx12(sinv, 0x50000);
        muzzle.z = *(int *)(self + 0x64) + fx12(cosv, 0x50000);

        /* 02121390..02121394: the shot's pitch is muzzle -> aim */
        shotAng.x = Vec3_VertAngle(&muzzle, &aim);

        /* 02121398..021213d4: BIG_MR_I (263) fires param 1, MR_I (262) param 0 */
        _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
            0x108,
            (*(unsigned short *)(self + 0xc) == 0x107) ? 1u : 0u,
            &muzzle, &shotAng,
            *(signed char *)(self + 0xcc), -1);

        /* 021213d8..02121408 */
        func_0201267c(0x165, self + 0x74);
        *(unsigned char *)(self + 0x216) = 0x2e;
        *(unsigned char *)(self + 0x212) = 0xf0;
        *(unsigned char *)(self + 0x213) = 0x53;
        *(int *)(self + 0x1f4) = 0;
        func_ov071_021209c8(self);
    }

    /* 0212140c..02121410 */
    func_ov071_02120860(self);

    /* 02121414..02121434: the ray end is the tracked actor lifted 0x8c000 */
    t = *(char **)(self + 0x1ec);
    rayEnd.x = *(int *)(t + 0x5c);
    rayEnd.y = *(int *)(t + 0x60) + 0x8c000;
    rayEnd.z = *(int *)(t + 0x64);

    /* 02121438..021214c0: one of four exits, first match wins */
    if (func_ov071_0212070c(self)) {
        func_ov071_02121634(self, 2);
    } else if (Vec3_Dist(self + 0x5c, &target) > 0x5dc000) {
        func_ov071_02121634(self, 0);
    } else if (*(unsigned char *)(*(char **)(self + 0x1ec) + 0x6fb) != 0) {
        func_ov071_02121634(self, 0);
    } else if (_ZN5Actor17DetectRaycastClsnER7Vector3S1_b(self, &rayEnd,
                                                          self + 0x5c, false)) {
        func_ov071_02121634(self, 0);
    }

    /* 021214c4..021214cc */
    func_ov071_02120b14(self);
    return 1;
}

/* =========================================================================
 * func_ov071_02120d30 -- ov071 0x02120d30, 0x3dc bytes, MrI state 2 MAIN.
 *
 * The defeat sequence, run every frame once state 2 is entered: the eye spins
 * up (the yaw integrator at +0x1f8 with its /0x1fffe sound tick), two particle
 * systems are kept alive on the body, and a four-step machine on +0x214 winds
 * the spin down, plays out the animation, shrinks the actor, and finally pops
 * it -- MR_I (262) leaving actor 0x122 behind, BIG_MR_I (263) paying out the
 * tracked star and three burst effects. Both ends run the same tail.
 *
 * Stack frame is 0x1c: sp+0x0c is the V3 handed to Actor::Spawn in the 262
 * branch; sp+0x00..0x08 are outgoing stack args.
 * ========================================================================= */
int func_ov071_02120d30(char *self)
{
    V3 spawnPos;   /* sp+0x0c */

    /* 02120d3c..02120d4c: ease the spin rate toward the stored target */
    _Z14ApproachLinearRiii((int *)(self + 0x98), *(short *)(self + 0x20e),
                           0x12c);

    /* 02120d50..02120d74: advance the yaw and the accumulator by that rate */
    *(short *)(self + 0x8e) =
        (short)(*(short *)(self + 0x8e) + *(int *)(self + 0x98));
    *(int *)(self + 0x1f8) += *(int *)(self + 0x98);

    /* 02120d78..02120d9c: one spin sound per whole turn.
       02120da0..02120dcc: keep the accumulator inside one turn. */
    if (*(int *)(self + 0x1f8) / 0x1fffe != 0)
        func_0201267c(0x119, self + 0x74);
    *(int *)(self + 0x1f8) %= 0x1fffe;

    /* 02120dd0..02120e60: while both handles are live, refresh both systems
       on the body and stamp each with the 0x7fff lifetime */
    if (*(unsigned *)(self + 0x204) != 0 && *(unsigned *)(self + 0x208) != 0) {
        void *a, *b;

        *(unsigned *)(self + 0x204) =
            _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                *(unsigned *)(self + 0x204), 0x13a, *(int *)(self + 0x5c),
                *(int *)(self + 0x60), *(int *)(self + 0x64), 0, 0);
        *(unsigned *)(self + 0x208) = (unsigned)(size_t)
            _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
                *(unsigned *)(self + 0x208), 0x13b, *(int *)(self + 0x5c),
                *(int *)(self + 0x60), *(int *)(self + 0x64), 0);

        a = _ZN8Particle6System12FromUniqueIDEj(*(unsigned *)(self + 0x204));
        b = _ZN8Particle6System12FromUniqueIDEj(*(unsigned *)(self + 0x208));
        if (a)
            *(int *)((char *)a + 0x50) = 0x7fff;
        if (b)
            *(int *)((char *)b + 0x50) = 0x7fff;
    }

    /* 02120e64..02120e80: the ROM's `cmp #3 / addls pc, pc, r0, lsl #2` table.
       Anything above 3 falls straight through to the common return. */
    switch (*(unsigned char *)(self + 0x214)) {

    /* ---- 02120e84..02120ef8: step 0, wobble the pitch and bleed the spin -- */
    case 0: {
        int phase = *(unsigned short *)(self + 0x210);
        *(short *)(self + 0x8c) =
            (short)fx12(*(int *)(self + 0x1fc),
                        data_02082214[(phase >> 4) * 2]);
        *(short *)(self + 0x210) =
            (short)(*(short *)(self + 0x210) + 0xe000);
        _Z14ApproachLinearRiii((int *)(self + 0x1fc), 0, 0x1b);
        if (DecIfAbove0_Byte((unsigned char *)(self + 0x215)) == 0)
            ++*(unsigned char *)(self + 0x214);
        break;
    }

    /* ---- 02120efc..02120f20: step 1, let the animation play out ----------- */
    case 1:
        _ZN9Animation7AdvanceEv(self + 0x124);
        if (_ZN9Animation8FinishedEv(self + 0x124))
            ++*(unsigned char *)(self + 0x214);
        break;

    /* ---- 02120f24..02120fa8: step 2, shrink to nothing -------------------- */
    case 2: {
        int s;
        if (_Z14ApproachLinearRiii((int *)(self + 0x1f0), 0xa4, 0xa4))
            ++*(unsigned char *)(self + 0x214);
        s = *(int *)(self + 0x1f0);
        *(int *)(self + 0x80) = s;
        *(int *)(self + 0x84) = s;
        *(int *)(self + 0x88) = s;
        /* Both id arms store the same product in the ROM (0x55 either way);
           transcribed as the two arms the listing actually spells. */
        if (*(unsigned short *)(self + 0xc) == 0x106)
            *(int *)(self + 0x178) = *(int *)(self + 0x1f0) * 0x55;
        else if (*(unsigned short *)(self + 0xc) == 0x107)
            *(int *)(self + 0x178) = *(int *)(self + 0x1f0) * 0x55;
        break;
    }

    /* ---- 02120fac..021210bc: step 3, pay out and die ---------------------- */
    case 3:
        if (*(unsigned short *)(self + 0xc) == 0x106) {
            /* 02120fc8..0212100c: MR_I drops actor 0x122 and a dust puff */
            spawnPos.x = *(int *)(self + 0x5c);
            spawnPos.y = *(int *)(self + 0x60) + 0x78000;
            spawnPos.z = *(int *)(self + 0x64);
            _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                0x122, 2, &spawnPos, 0,
                *(signed char *)(self + 0xcc), -1);
            _ZN5Actor8PoofDustEv(self);
        } else if (*(unsigned short *)(self + 0xc) == 0x107) {
            /* 0212102c..0212108c: BIG_MR_I pays the star and three bursts */
            _ZN5Actor19UntrackAndSpawnStarERajRK7Vector3j(
                self, (signed char *)(self + 0x217),
                (unsigned)((*(int *)(self + 8) & 0xf) & 0xff),
                self + 0x5c, 4);
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                0x124, *(int *)(self + 0x5c), *(int *)(self + 0x60),
                *(int *)(self + 0x64));
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                0x125, *(int *)(self + 0x5c), *(int *)(self + 0x60),
                *(int *)(self + 0x64));
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                0x126, *(int *)(self + 0x5c), *(int *)(self + 0x60),
                *(int *)(self + 0x64));
        }
        /* 02121090..021210bc: the tail runs for every id, including neither */
        func_0201267c(0xc4, self + 0x74);
        if (data_0209f2f8 == 0x2e)
            _ZN5Actor24KillAndTrackInDeathTableEv(self);
        else
            _ZN9ActorBase18MarkForDestructionEv(self);
        break;

    default:
        break;
    }

    /* 021210c0..021210cc */
    return 1;
}

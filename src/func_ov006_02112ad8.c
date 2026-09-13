#include "types.h"

/**
 * Live table physics for one Smartball minigame ball.
 *
 * cMgSmartball_ball_c::SaveSnapshot calls this once the ball is no longer
 * queued (mIsWaiting clear). It casts a 32-direction ring around the ball,
 * walks it out of walls, bounces off bumpers and the spring, and swallows
 * it into a hole.
 *
 * This file stays C: the real class has virtuals and cannot be included
 * from here. Ball / Mgr are C shadows of cMgSmartball_ball_c and
 * dScMgSmartball_c. Field names match notes/smartball-provenance.md.
 *
 * deslop
 * Leftover: the symbol is still func_ov006_02112ad8 (no RTTI for methods).
 * Leftover: #pragma opt_strength_reduction off (drop DIFFs 53 words).
 * Leftover: pos1..pos12 copies of the probe point (passing &work DIFFs).
 *   event0..3, specialDelta0..2, hazardPos/2, entityPos/2 are the same kind.
 * Leftover: goto final_checks on a kinoko hit (skips pos7..pos10).
 * Leftover: state.iteration is a rematerialized zero and also the
 *   "kinoko nearby" flag (set to 1). zero/zero48/zero4c/zeroAngle
 *   are rematerialized zeros; angle40/index44/baseZ are offset names.
 * Leftover: hit/hitA/hitB/hitC are wall / exit-pocket / ball-vs-ball
 *   (see the comment at the bounce site). mCurrent0/1 are pos x/z.
 * Leftover: func_02012718 is declared void*(id) here; decl_common
 *   and the sibling func_020126ac take int.
 * Leftover: volatile V2 savedVel is a match hack.
 * Leftover: RandomIntInternal >> 16 & 0x7fff * 0x10 >> 15 << 12
 *   tower (3x) and the angle one (2x); bounce loops use
 *   (s16)(angle / 0x20) for i * 0x800.
 */

typedef struct V2 {
    int x;
    int z;
} V2;

typedef struct Mgr {
    char pad00[8];
    int flags08;
    char pad0c[0x4668 - 0x0c];
    int ballCount;      /* 0x4668 -- tracked balls */
    int holeCount;      /* 0x466c -- ana (holes) */
    int kinokoCount;    /* 0x4670 -- mushrooms */
    char pad4674[0x4684 - 0x4674];
    void *spring;       /* 0x4684 */
    void *balls[13];    /* 0x4688 */
    void *kinoko[25];   /* 0x46bc */
    void *holes[1];     /* 0x4720 */
} Mgr;

typedef struct Ball {
    char pad00[4];
    Mgr *mpManager;
    int mCurrent0;
    int mCurrent1;
    int mSnapshot0;
    int mSnapshot1;
    char pad18[8];
    int mVel0;
    int mVel1;
    int mRadius;
    int mIndex;
    u8 mIsActive;     /* 0x30 */
    char pad31[0x09];
    u8 state3a;
    u8 state3b;
    char pad3c[8];
    int hitX;
    int hitZ;
    u8 hit[32];
    u8 hitA[32];
    u8 hitB[32];
    u8 hitC[32];
    u8 anyHit;
    u8 specialHit;
    u8 nearby[25];
    char pade7[0x11];
    int targetIndex;
    char padfc[0x124 - 0xfc];
    int soundTimer;
    u8 soundPlayed;
} Ball;

typedef struct State {
    int hitTile;
    int hitOther;    /* func_ov006_021120d4 */
    int hitExit;
    int hitKinoko;
    int hitWing;
    int hitSpring;
    int resolveCount;
    int iteration;
    int depth;
    int zeroAngle;
    int zero;
    int angle40;
    int index44;
    int zero48;
    int zero4c;
    int baseZ;
} State;

extern void func_ov006_0211470c(V2 *out, void *obj);
extern s64 func_0203d5bc(V2 *p);
extern void func_0203d388(V2 *p, int angle);
extern int func_0203d570(V2 *a, V2 *b);
extern int func_ov006_021126b4(Ball *self, V2 *p); /* out of bounds */
extern int func_ov006_02112504(Ball *self, V2 *p); /* table tiles */
extern int func_ov006_021122e0(Ball *self, V2 *p); /* exit pocket */
extern int func_ov006_02112190(Ball *self, V2 *p); /* kinoko */
extern int func_ov006_02112030(Ball *self, V2 *p);
extern int func_ov006_02111f8c(Ball *self, V2 *p);
extern int func_ov006_02111ee8(Ball *self, V2 *p); /* pushswitch */
extern int func_ov006_02111e90(Ball *self, V2 *p); /* wing */
extern int func_ov006_0211248c(Ball *self, V2 *p); /* spring */
extern int func_ov006_021120d4(Ball *self, V2 *p);
extern void Vec2_Sub(V2 *out, V2 *a, V2 *b);
extern int func_0203d434(V2 *p);
extern void func_0203d630(V2 *p, int scale);
extern int Vec2_Len(V2 *p);
extern void func_0203d480(V2 *out, V2 *in);
extern int RandomIntInternal(int *seed);
extern void func_ov006_021146f4(V2 *out, void *obj);
extern void func_02012718(void *id, int x);
extern int func_020126e8(int x);
extern void func_020126ac(int id, int type, int volume, int arg3, int arg4);
extern void func_ov006_02111b90(Ball *self, int id, V2 *vel);
extern int func_ov006_021115c4(void *obj);  /* hole already eating? */
extern void func_ov006_021115cc(void *obj); /* swallow into hole */
extern void func_ov006_02111dcc(Ball *self, int value);

extern int data_0209d4b8;

#define FIX_MUL(a, b) ((int)(((s64)(a) * (b) + 0x800) >> 12))
#define GET_BALL(mgr, i) ((i) >= 0xd ? (void *)0 : (mgr)->balls[i])
#define GET_HOLE(mgr, i) ((i) >= (mgr)->holeCount ? (void *)0 : (mgr)->holes[i])

#pragma opt_strength_reduction off
void func_ov006_02112ad8(Ball *self)
{
    State state;
    V2 work;
    volatile V2 savedVel;
    V2 dist;
    V2 delta;
    V2 otherPos;
    V2 normal;
    V2 unit;
    V2 response;
    V2 radial;
    V2 hazardDelta;
    V2 correction;
    V2 pos0;
    V2 pos1;
    V2 pos2;
    V2 pos3;
    V2 pos4;
    V2 pos5;
    V2 pos6;
    V2 pos7;
    V2 pos8;
    V2 pos9;
    V2 pos10;
    V2 pos11;
    V2 pos12;
    V2 entityPos;
    V2 entityDelta;
    V2 entityPos2;
    V2 radialDelta;
    V2 moveDelta;
    V2 normalized;
    V2 specialPos;
    V2 specialDelta0;
    V2 specialDelta1;
    V2 specialDelta2;
    V2 event0;
    V2 event1;
    V2 event2;
    V2 event3;
    V2 hazardPos;
    V2 hazardPos2;
    int blocked;
    int i;
    int otherBall;
    int angle;
    int responseScale;
    int dot;
    int volume;
    void *other;
    Mgr *mgr;
    s64 square;
    int correctionZ;
    int correctionX;

    savedVel.x = self->mVel0;
    state.hitTile = 0;
    savedVel.z = self->mVel1;
    self->targetIndex = -1;
    state.hitOther = 0;
    state.hitExit = 0;
    state.hitKinoko = 0;
    state.hitWing = 0;
    state.hitSpring = 0;
    state.iteration = 0;
    state.resolveCount = 0;
    i = 0;
    state.iteration = 0;
    for (; i < 0x20; i++) {
        self->hitA[i] = (u8)state.iteration;
        self->hitC[i] = (u8)state.iteration;
        self->hitB[i] = (u8)state.iteration;
    }
    self->anyHit = (u8)state.iteration;
    self->specialHit = (u8)state.iteration;
    state.iteration = 0;

    /* Mark which mushrooms sit close enough to test this frame. */
    if (self->mCurrent1 >= 0x77000) {
        int clearIndex = state.iteration;
        for (; clearIndex < 0x19; clearIndex++)
            self->nearby[clearIndex] = 0;
    } else {
        int probeIndex = state.iteration;
        for (; probeIndex < 0x19; probeIndex++) {
            self->nearby[probeIndex] = 0;
            if (probeIndex < self->mpManager->kinokoCount) {
                dist.x = self->mCurrent0;
                dist.z = self->mCurrent1;
                mgr = self->mpManager;
                func_ov006_0211470c(&pos0,
                    probeIndex >= mgr->kinokoCount
                        ? (void *)0 : mgr->kinoko[probeIndex]);
                dist.x -= pos0.x;
                dist.z -= pos0.z;
                if (func_0203d5bc(&dist) <= 0x900000LL) {
                    self->nearby[probeIndex] = 1;
                    state.iteration = 1;
                }
            }
        }
    }

    state.depth = 0;
    state.zero = 0;
    state.angle40 = 0;
    state.index44 = 0;
    state.zero4c = 0;
    state.baseZ = 0x800;
    state.zero48 = 0;
    state.zeroAngle = 0;

    /* Walk the ball out of whatever it is overlapping, up to 32 times. */
    do {
        /* 32 feelers, one every 0x800 (1/32 of a turn). */
        angle = state.zeroAngle;
        blocked = angle;
        i = angle;
        for (; i < 0x20; i++) {
            self->hit[i] = (u8)state.zero;
            work.x = state.zero;
            work.z = state.zero;
            work.z += self->mRadius;
            func_0203d388(&work, angle);
            dot = func_0203d570(&work, (V2 *)&self->mVel0);
            if (dot >= 0) {
                work.x += self->mCurrent0;
                work.z += self->mCurrent1;
                if ((u8)self->mpManager->flags08 == 0) {
                    pos1 = work;
                    if (func_ov006_021126b4(self, &pos1) != 0) {
                        self->hit[i] = 1;
                        self->hitA[i] = 1;
                        self->anyHit = 1;
                        blocked = 1;
                    } else {
                        pos2 = work;
                        if (func_ov006_02112504(self, &pos2) != 0) {
                            self->hit[i] = 1;
                            self->hitA[i] = 1;
                            self->anyHit = 1;
                            state.hitTile = 1;
                            blocked = 1;
                        } else {
                            pos3 = work;
                            if (func_ov006_021122e0(self, &pos3) != 0) {
                                self->hit[i] = 1;
                                self->hitA[i] = 1;
                                self->anyHit = 1;
                                self->hitB[i] = 1;
                                state.hitExit = 1;
                                blocked = 1;
                            }
                        }
                    }
                } else {
                    pos4 = work;
                    if (func_ov006_021126b4(self, &pos4) != 0) {
                        self->hit[i] = 1;
                        self->hitA[i] = 1;
                        self->anyHit = 1;
                        blocked = 1;
                    } else {
                        pos5 = work;
                        if (func_ov006_021122e0(self, &pos5) != 0) {
                            self->hit[i] = 1;
                            self->hitA[i] = 1;
                            self->anyHit = 1;
                            self->hitB[i] = 1;
                            state.hitExit = 1;
                            blocked = 1;
                        } else {
                            if (state.iteration == 1) {
                                pos6 = work;
                                if (func_ov006_02112190(self, &pos6) != 0) {
                                    self->hit[i] = 1;
                                    self->hitA[i] = 1;
                                    self->anyHit = 1;
                                    self->specialHit = 1;
                                    state.hitKinoko = 1;
                                    blocked = 1;
                                    goto final_checks;
                                }
                            }
                            pos7 = work;
                            if (func_ov006_02112030(self, &pos7) != 0) {
                                self->hit[i] = 1;
                                self->hitA[i] = 1;
                                self->anyHit = 1;
                                blocked = 1;
                            } else {
                                pos8 = work;
                                if (func_ov006_02111f8c(self, &pos8) != 0) {
                                    self->hit[i] = 1;
                                    self->hitA[i] = 1;
                                    self->anyHit = 1;
                                    blocked = 1;
                                } else {
                                    pos9 = work;
                                    if (func_ov006_02111ee8(self, &pos9) != 0) {
                                        self->hit[i] = 1;
                                        self->hitA[i] = 1;
                                        self->anyHit = 1;
                                        blocked = 1;
                                    } else {
                                        pos10 = work;
                                        if (func_ov006_02111e90(self, &pos10) != 0) {
                                            self->hit[i] = 1;
                                            self->hitA[i] = 1;
                                            self->anyHit = 1;
                                            blocked = 1;
                                            state.hitWing = 1;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            } else {
                work.x += self->mCurrent0;
                work.z += self->mCurrent1;
            }

final_checks:
            if (blocked == 0) {
                pos11 = work;
                if (func_ov006_0211248c(self, &pos11) != 0) {
                    self->hit[i] = 1;
                    self->hitA[i] = 1;
                    self->anyHit = 1;
                    blocked = 1;
                    if (angle == 0)
                        state.hitSpring = 1;
                } else {
                    pos12 = work;
                    if (func_ov006_021120d4(self, &pos12) != 0) {
                        self->hit[i] = 1;
                        self->hitA[i] = 1;
                        self->anyHit = 1;
                        blocked = 1;
                        state.hitOther = 1;
                    }
                }
            }
            angle = (s16)(angle + 0x800);
        }

        /* Ball-vs-ball: if another live ball is close, mark that ring. */
        if (state.resolveCount < 0x11 && self->state3a == 0) {
            for (otherBall = 0; otherBall < self->mpManager->ballCount;
                otherBall++) {
                if (otherBall != self->mIndex) {
                    other = GET_BALL(self->mpManager, otherBall);
                    if (((Ball *)other)->state3a == 0) {
                        other = GET_BALL(self->mpManager, otherBall);
                        if (((Ball *)other)->mIsActive != 0) {
                            other = GET_BALL(self->mpManager, otherBall);
                            func_ov006_0211470c(&entityPos, other);
                            Vec2_Sub(&entityDelta, (V2 *)&self->mCurrent0,
                                &entityPos);
                            delta = entityDelta;
                            if (func_0203d5bc(&delta) <= 0x100000LL) {
                                angle = state.angle40;
                                mgr = self->mpManager;
                                other = GET_BALL(mgr, otherBall);
                                func_ov006_0211470c(&entityPos2, other);
                                otherPos = entityPos2;
                                if (self->mCurrent1 < 0x90000 ||
                                    self->mCurrent0 <= otherPos.x) {
                                    i = state.index44;
                                    for (; i < 0x20; i++) {
                                        work.z = self->mRadius;
                                        work.x = 0;
                                        func_0203d388(&work, angle);
                                        work.x += self->mCurrent0;
                                        work.z += self->mCurrent1;
                                        Vec2_Sub(&radialDelta, &work, &otherPos);
                                        delta = radialDelta;
                                        if (func_0203d5bc(&delta) < 0x40000LL) {
                                            self->targetIndex = otherBall;
                                            self->hit[i] = 1;
                                            self->hitA[i] = 1;
                                            self->hitC[i] = 1;
                                            blocked = 1;
                                        }
                                        angle = (s16)(angle + 0x800);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        /* Push the ball out along the sum of hit normals. */
        normal.x = state.zero48;
        normal.z = state.zero48;
        angle = state.zero48;
        i = state.zero48;
        for (; i < 0x20; i++) {
            if (self->hit[i] == 1) {
                unit.x = state.zero4c;
                unit.z = state.baseZ;
                func_0203d388(&unit, angle);
                normal.x += unit.x;
                normal.z += unit.z;
            }
            angle = (s16)(angle + 0x800);
        }
        func_0203d434(&normal);
        func_0203d630(&normal, state.baseZ);
        self->mCurrent0 -= normal.x;
        self->mCurrent1 -= normal.z;
        Vec2_Sub(&moveDelta, (V2 *)&self->mCurrent0,
            (V2 *)&self->mSnapshot0);
        dot = Vec2_Len(&moveDelta);
        state.depth += 0x80;
        state.resolveCount++;
    } while (blocked == 1 && state.resolveCount < 0x21 && state.depth <= dot);

    /* Bounce: exit-pocket hits use hitB; everything else uses hitA except
       ball-vs-ball (hitC). */
    i = 0;
    response.x = 0;
    response.z = 0;
    if (state.hitExit == 1) {
        angle = 0;
        for (; i < 0x20; i++, angle += 0x10000) {
            if (self->hitB[i] == 1) {
                radial.x = 0;
                radial.z = 0x1000;
                func_0203d388(&radial, (s16)(angle / 0x20));
                response.x -= radial.x;
                response.z -= radial.z;
            }
        }
    } else {
        angle = 0;
        for (; i < 0x20; i++, angle += 0x10000) {
            if (self->hitA[i] == 1 && self->hitC[i] == 0) {
                radial.x = 0;
                radial.z = 0x1000;
                func_0203d388(&radial, (s16)(angle / 0x20));
                response.x -= radial.x;
                response.z -= radial.z;
            }
        }
    }

    if (response.x != 0 || response.z != 0) {
        func_0203d480(&normalized, &response);
        dot = FIX_MUL(self->mVel0, response.x) + FIX_MUL(self->mVel1, response.z);
        if (state.hitSpring == 1) {
            responseScale = FIX_MUL(dot, 0x1800);
        } else if (state.hitTile == 1) {
            u32 randomScale;
            u32 randomAngle;
            responseScale = FIX_MUL(dot, 0x1400);
            randomScale = ((((((u32)RandomIntInternal(&data_0209d4b8) >> 16) &
                0x7fff) * 0x10) >> 15) << 12);
            responseScale += FIX_MUL(responseScale, randomScale >> 6);
            randomAngle = (u32)RandomIntInternal(&data_0209d4b8);
            func_0203d388(&response,
                (s16)(((((randomAngle >> 16) & 0x7fff) << 12) >> 15) - 0x800));
        } else if (state.hitKinoko == 1) {
            u32 randomScale;
            u32 randomAngle;
            self->mVel0 = 0;
            self->mVel1 = 0;
            responseScale = -0x2000;
            randomScale = ((((((u32)RandomIntInternal(&data_0209d4b8) >> 16) &
                0x7fff) * 0x10) >> 15) << 12);
            responseScale += FIX_MUL(randomScale >> 5, -0x2000);
            randomAngle = (u32)RandomIntInternal(&data_0209d4b8);
            func_0203d388(&response,
                (s16)(((((randomAngle >> 16) & 0x7fff) << 12) >> 15) - 0x800));
        } else if (state.hitExit == 1) {
            u32 randomScale;
            responseScale = FIX_MUL(dot, 0x1a00);
            randomScale = ((((((u32)RandomIntInternal(&data_0209d4b8) >> 16) &
                0x7fff) * 0x10) >> 15) << 12);
            responseScale += FIX_MUL(responseScale, randomScale >> 6);
            if (self->mCurrent0 < 0x18000 && response.z > 0) {
                response.x = 0;
                response.z = 0x1000;
            }
        } else {
            responseScale = FIX_MUL(dot, 0x1200);
        }
        self->mVel0 -= FIX_MUL(responseScale, response.x);
        self->mVel1 -= FIX_MUL(responseScale, response.z);
    }

    /* Spring: snap onto it and take its downward speed. */
    if (state.hitSpring == 1) {
        if (self->mCurrent0 < 0xd0000)
            return;
        if (self->mCurrent0 < 0xd8000) {
            self->mCurrent0 = 0xd8000;
            return;
        }
        if (self->mCurrent0 < 0xe8000)
            self->mCurrent0 = 0xe8000;
        func_ov006_0211470c(&specialPos, self->mpManager->spring);
        self->mCurrent1 = specialPos.z - 0x20001 - self->mRadius;
        func_ov006_021146f4(&specialDelta0, self->mpManager->spring);
        if (specialDelta0.z >= -0x1000)
            return;
        func_ov006_021146f4(&specialDelta1, self->mpManager->spring);
        if (self->mVel1 < specialDelta1.z)
            return;
        self->mVel0 = 0;
        func_ov006_021146f4(&specialDelta2, self->mpManager->spring);
        self->mVel1 = specialDelta2.z;
        if (self->mVel1 < -0x6000)
            self->mVel1 = (self->mVel1 + 0x6000) / 4 - 0x6000;

        if (self->soundTimer == 0) {
            if (self->mVel1 < -0x6000) {
                func_02012718((void *)0x16d, self->mCurrent0);
                func_02012718((void *)0x16e, self->mCurrent0);
            } else {
                volume = (-self->mVel1 << 7) / 0x6000;
                if (volume > 0x7f)
                volume = 0x7f;
                if (volume < 0)
                volume = 0;
                func_020126ac(0x16d, 5, volume, 0,
                func_020126e8(self->mCurrent0));
                if (volume >= 0x40)
                func_020126ac(0x16e, 5, volume, 0,
                    func_020126e8(self->mCurrent0));
            }
            self->soundTimer = 5;
        }
        if (self->soundPlayed != 0)
            return;
        if (self->mVel1 < -0x6000)
            func_02012718((void *)0x16e, self->mCurrent0);
        self->soundPlayed = 1;
        return;
    }

    /* Collision sounds: tile, table-0x4674, exit pocket, wing. */
    if (state.hitTile == 1) {
        event0.x = savedVel.x;
        event0.z = savedVel.z;
        func_ov006_02111b90(self, 0x171, &event0);
    } else if (state.hitOther == 1) {
        event1.x = savedVel.x;
        event1.z = savedVel.z;
        func_ov006_02111b90(self, 0x172, &event1);
    } else if (state.hitExit == 1) {
        event2.x = savedVel.x;
        event2.z = savedVel.z;
        func_ov006_02111b90(self, 0x16f, &event2);
    } else if (state.hitWing == 1) {
        event3.x = savedVel.x;
        event3.z = savedVel.z;
        func_ov006_02111b90(self, 0x1a3, &event3);
    }

    /* Holes: swallow the ball, or nudge it away if it only grazed. */
    dot = self->mRadius + 0x10000;
    square = FIX_MUL(dot, dot);
    for (i = 0; i < self->mpManager->holeCount; i++) {
        other = GET_HOLE(self->mpManager, i);
        if (func_ov006_021115c4(other) == 0) {
            hazardDelta.x = self->mCurrent0;
            hazardDelta.z = self->mCurrent1;
            other = GET_HOLE(self->mpManager, i);
            func_ov006_0211470c(&hazardPos, other);
            hazardDelta.x -= hazardPos.x;
            hazardDelta.z -= hazardPos.z;
            hazardDelta.z *= 2;
            if (func_0203d5bc(&hazardDelta) < square) {
                dot = self->mRadius + 0x10000 - Vec2_Len(&hazardDelta);
                if (self->mRadius < dot) {
                other = GET_HOLE(self->mpManager, i);
                func_ov006_0211470c(&hazardPos2, other);
                self->hitX = hazardPos2.x;
                self->hitZ = hazardPos2.z;
                other = GET_HOLE(self->mpManager, i);
                func_ov006_021115cc(other);
                func_ov006_02111dcc(self, 0x80);
                self->state3a = 1;
                return;
                }
                correction = hazardDelta;
                func_0203d434(&correction);
                correction.x = FIX_MUL(correction.x, dot);
                correction.x /= 64;
                correction.z = FIX_MUL(correction.z, dot);
                correction.z /= 64;
                correctionX = correction.x;
                correctionZ = correction.z;
                self->mVel0 -= correctionX;
                self->mVel1 -= correction.z;
                return;
            }
        }
    }
}

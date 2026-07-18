/* The equal-arm owner selection below preserves mwccarm's address rematerialization. */
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned int u32;
typedef long long s64;
typedef unsigned long long u64;

typedef struct V2 {
    int x;
    int z;
} V2;

typedef struct Mgr {
    char pad00[8];
    int flags08;
    char pad0c[0x4668 - 0x0c];
    int entityCount;
    int hazardCount;
    int angleCount;
    char pad4674[0x4684 - 0x4674];
    void *special;
    void *entities[13];
    void *angles[25];
    void *hazards[1];
} Mgr;

typedef struct Obj {
    char pad00[4];
    Mgr *mgr;
    int pos[2];
    int oldPos[2];
    char pad18[8];
    int velX;
    int velZ;
    int radius;
    int index;
    char pad30[0x0a];
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
} Obj;

typedef struct State {
    int flags[7];
    int iteration;
    int depth;
    int zeroAngle;
    int zero;
    int entityIndex;
    void *null34;
    void *null38;
    int null3c;
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
extern int func_ov006_021126b4(Obj *self, V2 *p);
extern int func_ov006_02112504(Obj *self, V2 *p);
extern int func_ov006_021122e0(Obj *self, V2 *p);
extern int func_ov006_02112190(Obj *self, V2 *p);
extern int func_ov006_02112030(Obj *self, V2 *p);
extern int func_ov006_02111f8c(Obj *self, V2 *p);
extern int func_ov006_02111ee8(Obj *self, V2 *p);
extern int func_ov006_02111e90(Obj *self, V2 *p);
extern int func_ov006_0211248c(Obj *self, V2 *p);
extern int func_ov006_021120d4(Obj *self, V2 *p);
extern void func_0203d6d0(V2 *out, V2 *a, V2 *b);
extern int func_0203d434(V2 *p);
extern void func_0203d630(V2 *p, int scale);
extern int func_0203d614(V2 *p);
extern void func_0203d480(V2 *out, V2 *in);
extern int RandomIntInternal(int *seed);
extern void func_ov006_021146f4(V2 *out, void *obj);
extern void func_02012718(void *id, int x);
extern int func_020126e8(int x);
extern void func_020126ac(int id, int type, int volume, int arg3, int arg4);
extern void func_ov006_02111b90(Obj *self, int id, V2 *vel);
extern int func_ov006_021115c4(void *obj);
extern void func_ov006_021115cc(void *obj);
extern void func_ov006_02111dcc(Obj *self, int value);

extern int data_0209d4b8;

#define FIX_MUL(a, b) ((int)(((s64)(a) * (b) + 0x800) >> 12))
#define FIX_MUL_SU(a, b) \
    ((int)(((s64)((u64)(s64)(a) * (u64)(u32)(b)) + 0x800) >> 12))
#define FIX_MUL_US(a, b) \
    ((int)(((s64)((u64)(u32)(a) * (u64)(s64)(b)) + 0x800) >> 12))

static void *GetEntity(Mgr *mgr, int i)
{
    return i < 13 ? mgr->entities[i] : (void *)0;
}

static void *GetHazard(Mgr *mgr, int i)
{
    return i < mgr->hazardCount ? mgr->hazards[i] : (void *)0;
}

#pragma opt_strength_reduction off
void func_ov006_02112ad8(Obj *self)
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
    int entityLoopIndex;
    int angle;
    int responseScale;
    int dot;
    int volume;
    void *other;
    void *fallback;
    Mgr *mgr;
    s64 square;
    int correctionZ;
    int correctionX;
    int *velXPtr;
    int *velZPtr;

    savedVel.x = self->velX;
    state.flags[0] = 0;
    savedVel.z = self->velZ;
    self->targetIndex = -1;
    state.flags[1] = 0;
    state.flags[2] = 0;
    state.flags[3] = 0;
    state.flags[4] = 0;
    state.flags[5] = 0;
    state.iteration = 0;
    state.flags[6] = 0;
    i = 0;
    state.iteration = 0;
    do {
        self->hitA[i] = (u8)state.iteration;
        self->hitC[i] = (u8)state.iteration;
        self->hitB[i] = (u8)state.iteration;
        i++;
    } while (i < 0x20);
    self->anyHit = (u8)state.iteration;
    self->specialHit = (u8)state.iteration;
    state.iteration = 0;

    if (self->pos[1] >= 0x77000) {
        int clearIndex = state.iteration;
        do {
            self->nearby[clearIndex] = 0;
            clearIndex++;
        } while (clearIndex < 0x19);
    } else {
        int probeIndex = state.iteration;
        do {
            self->nearby[probeIndex] = 0;
            if (probeIndex < self->mgr->angleCount) {
                dist.x = self->pos[0];
                dist.z = self->pos[1];
                mgr = self->mgr;
                func_ov006_0211470c(&pos0,
                    probeIndex >= mgr->angleCount
                        ? (void *)0 : mgr->angles[probeIndex]);
                dist.x -= pos0.x;
                dist.z -= pos0.z;
                if (func_0203d5bc(&dist) <= 0x900000LL) {
                    self->nearby[probeIndex] = 1;
                    state.iteration = 1;
                }
            }
            probeIndex++;
        } while (probeIndex < 0x19);
    }

    state.depth = 0;
    state.zero = 0;
    state.null34 = (void *)0;
    state.null38 = (void *)0;
    state.null3c = 0;
    state.angle40 = 0;
    state.index44 = 0;
    state.zero4c = 0;
    state.baseZ = 0x800;
    state.zero48 = 0;
    state.zeroAngle = 0;

outer_loop:
        angle = state.zeroAngle;
        blocked = angle;
        i = angle;
        do {
            self->hit[i] = (u8)state.zero;
            work.x = state.zero;
            work.z = state.zero;
            work.z += self->radius;
            func_0203d388(&work, angle);
            dot = func_0203d570(&work, (V2 *)&self->velX);
            if (dot >= 0) {
                work.x += self->pos[0];
                work.z += self->pos[1];
                if ((u8)self->mgr->flags08 == 0) {
                    pos1 = work;
                    if (func_ov006_021126b4(self, &pos1) != 0) {
                        self->hit[i] = 1;
                        self->hitA[i] = 1;
                        self->anyHit = 1;
                        blocked = 1;
                        goto final_checks;
                    } else {
                        pos2 = work;
                        if (func_ov006_02112504(self, &pos2) != 0) {
                            self->hit[i] = 1;
                            self->hitA[i] = 1;
                            self->anyHit = 1;
                            state.flags[0] = 1;
                            blocked = 1;
                        } else {
                            pos3 = work;
                            if (func_ov006_021122e0(self, &pos3) != 0) {
                                self->hit[i] = 1;
                                self->hitA[i] = 1;
                                self->anyHit = 1;
                                self->hitB[i] = 1;
                                state.flags[2] = 1;
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
                            state.flags[2] = 1;
                            blocked = 1;
                            goto final_checks;
                        } else {
                            if (state.iteration == 1) {
                                pos6 = work;
                                if (func_ov006_02112190(self, &pos6) != 0) {
                                    self->hit[i] = 1;
                                    self->hitA[i] = 1;
                                    self->anyHit = 1;
                                    self->specialHit = 1;
                                    state.flags[3] = 1;
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
                                            state.flags[4] = 1;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            } else {
                work.x += self->pos[0];
                work.z += self->pos[1];
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
                        state.flags[5] = 1;
                } else {
                    pos12 = work;
                    if (func_ov006_021120d4(self, &pos12) != 0) {
                        self->hit[i] = 1;
                        self->hitA[i] = 1;
                        self->anyHit = 1;
                        blocked = 1;
                        state.flags[1] = 1;
                    }
                }
            }
            angle = (s16)(angle + 0x800);
            i++;
        } while (i < 0x20);

        if (state.flags[6] < 0x11 && self->state3a == 0) {
            entityLoopIndex = 0;
            if (self->mgr->entityCount > 0) {
                do {
                    if (entityLoopIndex != self->index) {
                        other = entityLoopIndex >= 0xd ? state.null34
                            : self->mgr->entities[entityLoopIndex];
                        if (((Obj *)other)->state3a == 0) {
                            other = entityLoopIndex >= 0xd ? state.null38
                                : self->mgr->entities[entityLoopIndex];
                            if (*(u8 *)((char *)other + 0x30) != 0) {
                                other = entityLoopIndex >= 0xd
                                    ? (void *)state.null3c
                                    : self->mgr->entities[entityLoopIndex];
                                func_ov006_0211470c(&entityPos, other);
                                func_0203d6d0(&entityDelta, (V2 *)self->pos,
                                    &entityPos);
                                delta = entityDelta;
                                if (func_0203d5bc(&delta) <= 0x100000LL) {
                                    angle = state.angle40;
                                    mgr = self->mgr;
                                    other = entityLoopIndex >= 0xd ? (void *)angle
                                        : mgr->entities[entityLoopIndex];
                                    func_ov006_0211470c(&entityPos2, other);
                                    otherPos = entityPos2;
                                    if (self->pos[1] < 0x90000 ||
                                        self->pos[0] <= otherPos.x) {
                                        i = state.index44;
                                        do {
                                            work.z = self->radius;
                                            work.x = 0;
                                            func_0203d388(&work, angle);
                                            work.x += self->pos[0];
                                            work.z += self->pos[1];
                                            func_0203d6d0(&radialDelta, &work, &otherPos);
                                            delta = radialDelta;
                                            if (func_0203d5bc(&delta) < 0x40000LL) {
                                                self->targetIndex = entityLoopIndex;
                                                self->hit[i] = 1;
                                                self->hitA[i] = 1;
                                                self->hitC[i] = 1;
                                                blocked = 1;
                                            }
                                            angle = (s16)(angle + 0x800);
                                            i++;
                                        } while (i < 0x20);
                                    }
                                }
                            }
                        }
                    }
                    entityLoopIndex++;
                } while (entityLoopIndex < self->mgr->entityCount);
            }
        }

        normal.x = state.zero48;
        normal.z = state.zero48;
        angle = state.zero48;
        i = state.zero48;
        do {
            if (self->hit[i] == 1) {
                unit.x = state.zero4c;
                unit.z = state.baseZ;
                func_0203d388(&unit, angle);
                normal.x += unit.x;
                normal.z += unit.z;
            }
            angle = (s16)(angle + 0x800);
            i++;
        } while (i < 0x20);
        func_0203d434(&normal);
        func_0203d630(&normal, state.baseZ);
        {
            V2 *position = (V2 *)self->pos;
            Obj *owner = position
                ? self : self;
            int *pz = (int *)(((int)owner + 0xc) &
                0xffffffffffffffffULL);
            *(int *)(((int)position) & 0xffffffffffffffffLL) -=
                normal.x;
            *pz -= normal.z;
            func_0203d6d0(&moveDelta, position,
                (V2 *)self->oldPos);
        }
        dot = func_0203d614(&moveDelta);
        state.depth += 0x80;
        state.flags[6]++;
    if (blocked == 1 && state.flags[6] < 0x21 && state.depth <= dot)
        goto outer_loop;

    i = 0;
    response.x = 0;
    response.z = 0;
    if (state.flags[2] == 1) {
        angle = 0;
        do {
            if (self->hitB[i] == 1) {
                radial.x = 0;
                radial.z = 0x1000;
                func_0203d388(&radial, (s16)(angle / 0x20));
                response.x -= radial.x;
                response.z -= radial.z;
            }
            i++;
            angle += 0x10000;
        } while (i < 0x20);
    } else {
        angle = 0;
        do {
            if (self->hitA[i] == 1 && self->hitC[i] == 0) {
                radial.x = 0;
                radial.z = 0x1000;
                func_0203d388(&radial, (s16)(angle / 0x20));
                response.x -= radial.x;
                response.z -= radial.z;
            }
            i++;
            angle += 0x10000;
        } while (i < 0x20);
    }

    if (response.x != 0 || response.z != 0) {
        func_0203d480(&normalized, &response);
        dot = FIX_MUL(self->velX, response.x) + FIX_MUL(self->velZ, response.z);
        if (state.flags[5] == 1) {
            responseScale = FIX_MUL(dot, 0x1800);
        } else if (state.flags[0] == 1) {
            u32 randomScale;
            u32 randomAngle;
            responseScale = FIX_MUL(dot, 0x1400);
            randomScale = ((((((u32)RandomIntInternal(&data_0209d4b8) >> 16) &
                0x7fff) * 0x10) >> 15) << 12);
            responseScale += FIX_MUL_SU(responseScale, randomScale >> 6);
            randomAngle = (u32)RandomIntInternal(&data_0209d4b8);
            func_0203d388(&response,
                (s16)((((((randomAngle >> 16) & 0x7fff) << 12) >> 15)) - 0x800));
        } else if (state.flags[3] == 1) {
            u32 randomScale;
            u32 randomAngle;
            self->velX = 0;
            self->velZ = 0;
            responseScale = -0x2000;
            randomScale = ((((((u32)RandomIntInternal(&data_0209d4b8) >> 16) &
                0x7fff) * 0x10) >> 15) << 12);
            responseScale += FIX_MUL_US(randomScale >> 5, -0x2000);
            randomAngle = (u32)RandomIntInternal(&data_0209d4b8);
            func_0203d388(&response,
                (s16)((((((randomAngle >> 16) & 0x7fff) << 12) >> 15)) - 0x800));
        } else if (state.flags[2] == 1) {
            u32 randomScale;
            responseScale = FIX_MUL(dot, 0x1a00);
            randomScale = ((((((u32)RandomIntInternal(&data_0209d4b8) >> 16) &
                0x7fff) * 0x10) >> 15) << 12);
            responseScale += FIX_MUL_SU(responseScale, randomScale >> 6);
            if (self->pos[0] < 0x18000 && response.z > 0) {
                response.x = 0;
                response.z = 0x1000;
            }
        } else {
            responseScale = FIX_MUL(dot, 0x1200);
        }
        velXPtr = (int *)(int)
            ((u64)(int)&self->velX & 0xffffffffffffffffULL);
        *velXPtr -= FIX_MUL(responseScale, response.x);
        velZPtr = (int *)(int)
            ((u64)(int)&self->velZ & 0xffffffffffffffffULL);
        *velZPtr -= FIX_MUL(responseScale, response.z);
    }

    if (state.flags[5] == 1) {
        if (self->pos[0] < 0xd0000)
            return;
        if (self->pos[0] < 0xd8000) {
            self->pos[0] = 0xd8000;
            return;
        }
        if (self->pos[0] < 0xe8000)
            self->pos[0] = 0xe8000;
        func_ov006_0211470c(&specialPos, self->mgr->special);
        self->pos[1] = specialPos.z - 0x20001 - self->radius;
        func_ov006_021146f4(&specialDelta0, self->mgr->special);
        if (specialDelta0.z >= -0x1000)
            return;
        func_ov006_021146f4(&specialDelta1, self->mgr->special);
        if (self->velZ < specialDelta1.z)
            return;
        self->velX = 0;
        func_ov006_021146f4(&specialDelta2, self->mgr->special);
        self->velZ = specialDelta2.z;
        if (self->velZ < -0x6000)
            self->velZ = (self->velZ + 0x6000) / 4 - 0x6000;

        if (self->soundTimer == 0) {
            if (self->velZ < -0x6000) {
                func_02012718((void *)0x16d, self->pos[0]);
                func_02012718((void *)0x16e, self->pos[0]);
            } else {
                volume = (-self->velZ << 7) / 0x6000;
                if (volume > 0x7f)
                    volume = 0x7f;
                if (volume < 0)
                    volume = 0;
                func_020126ac(0x16d, 5, volume, 0,
                    func_020126e8(self->pos[0]));
                if (volume >= 0x40)
                    func_020126ac(0x16e, 5, volume, 0,
                        func_020126e8(self->pos[0]));
            }
            self->soundTimer = 5;
        }
        if (self->soundPlayed != 0)
            return;
        if (self->velZ < -0x6000)
            func_02012718((void *)0x16e, self->pos[0]);
        self->soundPlayed = 1;
        return;
    }

    if (state.flags[0] == 1) {
        event0.x = savedVel.x;
        event0.z = savedVel.z;
        func_ov006_02111b90(self, 0x171, &event0);
    } else if (state.flags[1] == 1) {
        event1.x = savedVel.x;
        event1.z = savedVel.z;
        func_ov006_02111b90(self, 0x172, &event1);
    } else if (state.flags[2] == 1) {
        event2.x = savedVel.x;
        event2.z = savedVel.z;
        func_ov006_02111b90(self, 0x16f, &event2);
    } else if (state.flags[4] == 1) {
        event3.x = savedVel.x;
        event3.z = savedVel.z;
        func_ov006_02111b90(self, 0x1a3, &event3);
    }

    dot = self->radius + 0x10000;
    square = FIX_MUL(dot, dot);
    i = 0;
    if (self->mgr->hazardCount <= 0)
        return;
    do {
        other = i >= self->mgr->hazardCount ? (void *)0 : self->mgr->hazards[i];
        if (func_ov006_021115c4(other) == 0) {
            hazardDelta.x = self->pos[0];
            hazardDelta.z = self->pos[1];
            other = i >= self->mgr->hazardCount ? (void *)0 : self->mgr->hazards[i];
            func_ov006_0211470c(&hazardPos, other);
            hazardDelta.x -= hazardPos.x;
            hazardDelta.z -= hazardPos.z;
            hazardDelta.z *= 2;
            if (func_0203d5bc(&hazardDelta) < square) {
                dot = self->radius + 0x10000 - func_0203d614(&hazardDelta);
                if (self->radius < dot) {
                    other = i >= self->mgr->hazardCount ? (void *)0 : self->mgr->hazards[i];
                    func_ov006_0211470c(&hazardPos2, other);
                    self->hitX = hazardPos2.x;
                    self->hitZ = hazardPos2.z;
                    other = i >= self->mgr->hazardCount ? (void *)0 : self->mgr->hazards[i];
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
                velXPtr = (int *)(int)
                    ((u64)(int)&self->velX & 0xffffffffffffffffULL);
                *velXPtr -= correctionX;
                velZPtr = (int *)(int)
                    ((u64)(int)&self->velZ & 0xffffffffffffffffULL);
                *velZPtr -= correction.z;
                return;
            }
        }
        i++;
    } while (i < self->mgr->hazardCount);
}

//cpp
#include "types.h"
#include "nitro/fx/fx.h"
#include "dExtModel_c.h"
#include "cstd.h"
#include "Vector3s.h"

extern "C" {

/* Vector3 declares a constructor and destructor under C++, which makes its
 * assignment copy word by word. The ROM copies these with C's ldm/stm block
 * move, which C++ keeps for a struct with an array member. */
typedef struct Vec3Words {
    Fix12i w[3];
} Vec3Words;

/* The same for Vector3s's six-byte halfword copy. */
typedef struct Vec3sWords {
    s16 w[3];
} Vec3sWords;

extern int func_020538b8(int a, int b);
extern void *func_ov007_020b7690(int i);
extern int func_02053644(s16 *v);
extern void func_02053380(s16 *in, s16 *out);

/* The 0x1c-byte animation player; names as in func_ov007_020bc02c. */
typedef struct Ov007AnimPlayer {
    /* 0x00: 0 playing; 1 held; func_ov007_020c9110 sets 2 when a one-shot ends */
    s32 state;
} Ov007AnimPlayer;

/* The 0xa0-byte model record func_ov007_020bc6d4 allocates; names as in
 * func_ov007_020bc02c and func_ov007_020be0dc, only the slots read here. */
typedef struct Ov007Model {
    struct ModelComponents *model; /* 0x00 */
    char pad_04[0x14];
    Ov007AnimPlayer *boneAnim; /* 0x18 */
    char pad_1c[0x78];
    int visible; /* 0x94: func_ov007_020bbff0 renders only while it is nonzero */
    /* 0x98: 1 on the frames that step the animations (func_ov007_020bc02c) */
    int animTick;
} Ov007Model;

typedef char
    Ov007Model_animTick_must_be_at_0x98[(int)&((Ov007Model *)0)->animTick == 0x98 ? 1 : -1];

/* func_ov007_020beaa0's 0x1c-byte figure; func_ov007_020be9ac calls this
 * file with it (its +0x00/+0x01 bytes and +0x18 hook are not read here).
 * particleIndex and model as in func_ov007_020be0dc. The model's
 * animTick/visible gate the update below, and its bone 0 transform `P` is
 * what this follows: yaw takes func_020538b8 (atan2) of P's third rotation
 * row x/z; prevPos holds P's translation from the last stepped frame, so
 * `dd` is the motion since then. */
typedef struct Ov007Figure {
    u8 pad_00[2];
    u8 particleIndex; /* 0x02: picks the particle system func_ov007_020b7690 returns */
    u8 pad_03;
    Ov007Model *model; /* 0x04: func_ov007_020bc6d4 */
    int yaw;           /* 0x08 */
    Vector3 prevPos;   /* 0x0c */
} Ov007Figure;

/* The element func_ov007_020b7690 hands back; only the fields written here are named. */
typedef struct TrackElem {
    u8 pad_00[0x1c];
    int flags;   /* 0x1c */
    Vector3 pos; /* 0x20 */
    u8 pad_2c[0x10];
    Vector3s delta; /* 0x3c */
    u8 pad_42[0xa];
    int vel; /* 0x4c */
    u8 pad_50[8];
    u8 frames; /* 0x58 */
} TrackElem;

void func_ov007_020beb80(char *self) {
    Ov007Figure *f = (Ov007Figure *)self;
    struct ModelComponents *B = f->model->model;
    Matrix4x3 *P = B->transforms;
    int *q = (int *)(int)&P->r.m[6];
    int r;
    Vector3s dd;
    TrackElem *e;
    int frames;
    int vel;
    int sc;
    Vector3 *tp = (Vector3 *)(int)&P->t;
    r = func_020538b8(q[0], q[2]);
    dd.x = (short)(tp->x - f->prevPos.x);
    dd.y = (short)(tp->y - f->prevPos.y);
    dd.z = (short)(tp->z - f->prevPos.z);
    if (f->model->animTick != 0) {
        f->yaw = r;
        *(Vec3Words *)&f->prevPos = *(Vec3Words *)tp;
    }
    e = (TrackElem *)func_ov007_020b7690(f->particleIndex);
    {
        Ov007Model *A = f->model;
        if (A->visible != 0 && (unsigned)A->boneAnim->state <= 1) {
            vel = 0;
            frames = 0x1000;
            if (dd.x != 0 || dd.y != 0 || dd.z != 0) {
                int mag = (short)func_02053644((s16 *)&dd);
                int t;
                if (mag <= 0) sc = 0;
                else if (mag >= 0xfa0) sc = 0x1000;
                else
                    sc = cstd::fdiv(mag, 0xfa0);
                if (mag <= 0x1f4) t = 0;
                else if (mag >= 0x3e8) t = 0x1000;
                else
                    t = cstd::fdiv(mag - 0x1f4, 0x1f4);
                frames -= t;
                func_02053380((s16 *)&dd, (s16 *)&dd);
                *(Vec3sWords *)&e->delta = *(Vec3sWords *)&dd;
                vel = (short)FX_MUL(sc, 0x460);
            }
            e->frames = (u8)(((frames << 2) >> 12) + 1);
            e->vel = vel;
            e->flags &= ~2;
            *(Vec3Words *)&e->pos = *(Vec3Words *)tp;
        } else {
            e->flags |= 2;
        }
    }
}
}

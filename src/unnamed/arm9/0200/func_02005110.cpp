//cpp
#include "types.h"
#include "cstd.h"

extern "C" {

struct Vec3
{
  s32 x;
  s32 y;
  s32 z;
};
extern void Vec3_Add(struct Vec3 *out, struct Vec3 *a, struct Vec3 *b);
extern int *Vec3_AsrInPlace(int *v, int sh);
extern s32 Vec3_Dist(const struct Vec3 *a, const struct Vec3 *b);
extern void Vec3_Sub(struct Vec3 *out, struct Vec3 *a, struct Vec3 *b);
extern void Vec3_MulScalarInPlace(int *v, int s);
extern s16 Vec3_HorzAngle(const struct Vec3 *v0, const struct Vec3 *v1);
extern s16 Vec3_VertAngle(const struct Vec3 *v1, const struct Vec3 *v0);
extern int func_020091f8(void *a, void *b, int c, int d);
extern u32 func_020093f4(void *p, int x);
struct dCamera_c;
extern int func_02009138(struct dCamera_c *cam, int dist);
extern void func_020089d8(void *p);
/* File-local view of dCamera_c (include/dCamera_c.h is C++): the fields this
 * state touches, with the header's names. */
typedef struct {
    u8 pad_000[0x110];
    char *mTargetPlayer; /* 0x110 */
    u8 pad_114[0x8];
    /* LookAtExit points it at an actor's position. */
    struct Vec3 *mLookTarget; /* 0x11c */
    u8 pad_120[0x5c];
    s16 mAngleY; /* 0x17c */
} CameraView;

#ifndef SM64DS_PLATFORM_PC
typedef char CameraView_angle_y_must_be_at_0x17c[(((int)(&((CameraView *)0)->mAngleY)) == 0x17c)
                                                     ? (1)
                                                     : (-1)];
#endif

/* The target player's position and facing, as include/dActor_c.h's C view
 * places them. Its u8 at 0x724 is Player.h's mOwlFarCamera, read raw: this
 * is a C file and cannot include the C++-only Player.h. */
typedef struct {
    u8 pad_000[0x5c];
    struct Vec3 mPos; /* 0x05c */
    u8 pad_068[0x26];
    s16 mAngleY; /* 0x08e */
} ActorView;

int func_02005110(char *self) {
    CameraView *cam = (CameraView *)self;
    struct Vec3 eye;
    struct Vec3 aim;
    struct Vec3 *target;
    s32 horzAngle;
    s32 mode;
    {
        struct Vec3 *p = &((ActorView *)cam->mTargetPlayer)->mPos;
        eye.x = p->x;
        eye.y = p->y;
        eye.z = p->z;
        eye.y += 0x64000;
    }
    mode = 0x48000;
    target = cam->mLookTarget;
    if (target == 0) {
        aim.x = eye.x;
        aim.y = eye.y;
        aim.z = eye.z;
        if ((*((u8 *)(cam->mTargetPlayer + 0x724))) != 0) {
            mode = 0xd0000;
        }
        horzAngle = ((ActorView *)cam->mTargetPlayer)->mAngleY;
    } else {
        struct Vec3 diff;
        struct Vec3 sum;
        struct Vec3 newAim;
        s32 dist;
        s32 scale;
        s16 fall;
        Vec3_Add(&sum, target, &eye);
        aim.x = sum.x;
        aim.y = sum.y;
        aim.z = sum.z;
        Vec3_AsrInPlace((int *)(&aim), 1);
        dist = Vec3_Dist(&eye, &aim);
        if (dist > 0x30000) {
            Vec3_Sub(&diff, &aim, &eye);
            scale = cstd::fdiv(0x30000, dist);
            Vec3_MulScalarInPlace((int *)(&diff), scale);
            Vec3_Add(&newAim, &eye, &diff);
            aim.x = newAim.x;
            aim.y = newAim.y;
            aim.z = newAim.z;
        }
        fall = 0x2000;
        {
            struct Vec3 *playerPos = &((ActorView *)cam->mTargetPlayer)->mPos;
            s32 fallDist = Vec3_Dist(cam->mLookTarget, playerPos);
            s32 t = fallDist - 0x200000;
            if (t > 0) {
                s16 scaled = (s16)(((((s64)t) << 2) + 0x800) >> 12);
                if (scaled > fall) {
                    scaled = fall;
                }
                fall -= scaled;
            }
        }
        s16 diffAngle = Vec3_HorzAngle(cam->mLookTarget, &eye) - cam->mAngleY;
        fall = (diffAngle >= 0) ? (-fall) : (fall);
        if ((!newAim.x) && (!newAim.x)) {
        }
        horzAngle = (s16)(Vec3_HorzAngle(cam->mLookTarget, &eye) + fall);
    }
    {
        s32 vert = Vec3_VertAngle(&aim, &eye);
        if (vert < (-0x2800)) {
            vert = -0x2800;
        } else if (vert > 0x2800) {
            vert = 0x2800;
        }
        func_020091f8(self, &aim, horzAngle, vert);
    }
    {
        u32 r = func_020093f4((void *)self, mode);
        if (func_02009138((struct dCamera_c *)self, r) == 0) {
            func_020089d8((void *)self);
        }
    }
    return 1;
}
}

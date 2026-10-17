#include "PlayerInput.h"
#include "nitro/fx/fx.h"
typedef unsigned char u8;
typedef short s16;
typedef unsigned int u32;
typedef int s32;


extern void Vec3_RotateYAndTranslate(int *out, int *in, short angle, int *src);
extern void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern int func_020091f8(void *a, void *b, int c, int d);
extern unsigned int func_020093f4(void *p, int x);
extern int func_02009138(int *thiz, int arg);
extern void func_020089d8(void *p);

extern s32 data_02086e90[3];
extern short data_0209f4a2[];
extern short data_0209f4a4[];

/* File-local views (the class headers are C++): only the fields this state
 * touches. Camera names follow include/dCamera_c.h; the player's follow
 * include/dActor_c.h (mPos spans its mPosX..mPosZ). */
typedef struct {
    u8 pad_000[0x5c];
    Vector3 mPos; /* 0x05c */
    u8 pad_068[0x26];
    s16 mAngleY; /* 0x08e */
} PlayerView;

typedef struct {
    u8 pad_000[0x80];
    Vector3 lookAt; /* 0x080 */
    Vector3 pos;    /* 0x08c */
    /* func_020089f8 copies mTargetPlayer->mPos here every frame ... */
    Vector3 mTargetPos; /* 0x098 */
    /* ... and stores lookAt - mTargetPos here. */
    Vector3 mLookAtOffset; /* 0x0a4 */
    u8 pad_0b0[0x60];
    PlayerView *mTargetPlayer; /* 0x110 */
    u8 pad_114[0x40];
    u32 mFlags; /* 0x154 */
    u8 pad_158[0x24];
    s16 mAngleY; /* 0x17c */
    s16 mAngleX; /* 0x17e */
    u8 pad_180[0x6];
    /* The yaw func_020091f8 eases mAngleY toward; set from the player's
       facing + 0x8000, i.e. behind the player. */
    s16 mTargetAngleY; /* 0x186 */
} CameraView;

int func_02009aa8(char *self)
{
    CameraView *cam = (CameraView *)self;
    s32 tmp[3];
    Vector3 delta;
    Vector3 src;

    src.x = data_02086e90[0];
    src.y = data_02086e90[1];
    src.z = data_02086e90[2];

    Vec3_RotateYAndTranslate(tmp, (int *)&cam->mTargetPlayer->mPos, cam->mTargetPlayer->mAngleY,
                             (int *)&src);

    /* 0x1a6 (the header's unk_1a6) stays raw: set on entering a state
       (func_02009d30), cleared below once func_020091f8 reports arrival. */
    if (*(u8 *)(self + 0x1a6) != 0) {
        Vector3 *base;
        int r;

        Vec3_Sub(&delta, &cam->mTargetPlayer->mPos, &cam->mTargetPos);
        AddVec3(&cam->lookAt, &delta, &cam->lookAt);
        AddVec3(&cam->pos, &delta, &cam->pos);

        base = &cam->mTargetPlayer->mPos;
        cam->mTargetPos.x = base->x;
        cam->mTargetPos.y = base->y;
        cam->mTargetPos.z = base->z;

        r = func_020091f8(self, tmp, cam->mTargetAngleY, 0);
        if (r != 0) *(u8 *)(self + 0x1a6) = 0;

        cam->mLookAtOffset.x = 0;
        cam->mLookAtOffset.z = 0;
    } else {
        s16 newAngle;
        int idx;
        s16 t1, t2;
        s16 rem;
        int diff2;
        int clamped;
        s16 v17e;

        newAngle = cam->mTargetPlayer->mAngleY + 0x8000;
        cam->mAngleY += newAngle - cam->mTargetAngleY;
        cam->mTargetAngleY = newAngle;

        cam->lookAt.x = tmp[0];
        cam->lookAt.y = tmp[1];
        cam->lookAt.z = tmp[2];

        idx = gActivePlayerSlot * 0x18;
        t1 = *(s16 *)((char *)data_0209f4a2 + idx);
        cam->mAngleY -= FX_MUL(t1, 0x200);

        diff2 = (s16)(cam->mAngleY - newAngle);
        if (diff2 < -0x2000) {
            clamped = -0x2000;
        } else {
            if (diff2 > 0x2000) {
                clamped = 0x2000;
            } else {
                clamped = diff2;
            }
        }
        rem = diff2 - clamped;

        cam->mTargetPlayer->mAngleY += rem;
        cam->mTargetAngleY += rem;

        t2 = *(s16 *)((char *)data_0209f4a4 + idx);
        cam->mAngleX += FX_MUL(t2, 0x200);

        v17e = cam->mAngleX;
        if (v17e < -0x1c00) {
            v17e = -0x1c00;
        } else if (v17e > 0x1c00) {
            v17e = 0x1c00;
        }
        cam->mAngleX = v17e;
    }

    func_02009138((int *)self, func_020093f4(self, 0x48000));
    cam->mFlags |= 0x80000;
    func_020089d8(self);

    return 1;
}

#include "PlayerInput.h"
#include "nitro/fx/fx.h"
extern void FUN_02029ab0(void);
extern int func_02008b08(void *cam);
extern short func_02008abc(void *cam);
extern short func_02008a70(void *cam);
extern int Sound_PlayIfNotActive(int a, int b, int c, int d);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, short a);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, short a);
extern void MulVec3Mat4x3(void *v, void *m, void *dst);
extern short data_0209f4a0[];
extern unsigned char data_0209f4ae[];
extern int data_020a0e68[];

/* File-local views (the class headers are C++): only the fields this state
 * touches. Camera names follow include/dCamera_c.h; the player's follow
 * include/dActor_c.h. */
typedef struct {
    char pad_000[0x8e];
    short mAngleY; /* 0x08e */
} PlayerView;

typedef struct {
    int x, y, z;
} Vec3;

typedef struct {
    char pad_000[0x80];
    Vec3 lookAt; /* 0x080 */
    Vec3 pos;    /* 0x08c */
    char pad_098[0x78];
    PlayerView *mTargetPlayer; /* 0x110 */
    char pad_114[0x44];
    /* The handle Sound_PlayIfNotActive hands back for the turning sound. */
    int mTurnSound; /* 0x158 */
    char pad_15c[0x20];
    short mAngleY; /* 0x17c */
    short mAngleX; /* 0x17e */
    char pad_180[0x6];
    /* The yaw other states ease mAngleY toward; this state keeps it as an
       offset from the yaw at 0x19e (left raw: func_020097ec seeds it from
       the player's facing, nothing else shows its role). */
    short mTargetAngleY; /* 0x186 */
} CameraView;

int func_020095e4(void *cam)
{
    CameraView *view = (CameraView *)cam;
    int v[3];
    int off;
    int r7;
    short old186, old17e;
    char *c = (char *)cam;

    FUN_02029ab0();
    func_02008b08(cam);

    off = gActivePlayerSlot * 0x18;
    if (*(short *)((char *)data_0209f4a0 + off) != 0) {
        r7 = (data_0209f4ae[off] == 2) ? 0x240 : 0x200;
        old186 = view->mTargetAngleY;
        {
            int a = func_02008abc(cam);
            view->mTargetAngleY -= FX_MUL(r7, a);
        }
        {
            int t = view->mTargetAngleY;
            if (t < -0x4000) t = -0x4000;
            else if (t > 0x4000) t = 0x4000;
            view->mTargetAngleY = t;
        }
        view->mAngleY = *(short *)(c + 0x19e) + view->mTargetAngleY;
        old17e = view->mAngleX;
        view->mAngleX += FX_MUL(r7, (long long)func_02008a70(cam));
        {
            int t = view->mAngleX;
            if (t < 0) t = 0;
            else if (t > 0x38e3) t = 0x38e3;
            view->mAngleX = t;
        }
        if (view->mTargetAngleY != old186 || view->mAngleX != old17e) {
            view->mTurnSound = Sound_PlayIfNotActive(view->mTurnSound, 3, 0x181, 0);
        }
    }

    v[0] = 0;
    v[1] = 0;
    v[2] = -0x280000;
    Matrix4x3_FromTranslation(data_020a0e68, view->pos.x, view->pos.y, view->pos.z);
    Matrix4x3_ApplyInPlaceToRotationY(data_020a0e68, view->mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, view->mAngleX);
    MulVec3Mat4x3(v, data_020a0e68, &view->lookAt);
    view->mTargetPlayer->mAngleY = view->mAngleY - 0x8000;
    return 1;
}

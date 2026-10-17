#include "types.h"
#include "PlayerInput.h"
#include "nitro/fx/fx.h"

extern void Vec3_RotateYAndTranslate(int *out, int *in, short angle, int *src);
extern int func_020091f8(void *a, int b, int c, int d);
extern unsigned int func_020093f4(void *p, int x);
extern int func_02009138(int *thiz, int arg);
extern void func_020089d8(void *p);

extern int data_02086e9c[3];
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
    u8 pad_000[0x110];
    PlayerView *mTargetPlayer; /* 0x110 */
    u8 pad_114[0x68];
    s16 mAngleY; /* 0x17c */
    s16 mAngleX; /* 0x17e */
    u8 pad_180[0x6];
    /* The yaw func_020091f8 eases mAngleY toward. */
    s16 mTargetAngleY; /* 0x186 */
} CameraView;

int func_020098e0(char *self)
{
    CameraView *cam = (CameraView *)self;
    /* 0x1a6 (the header's unk_1a6) stays raw: set on entering a state
       (func_02009d30), cleared below once func_020091f8 reports arrival. */
    if (*(u8 *)(self + 0x1a6) != 0) {
        int pos[3];
        int src[3];
        int result;
        src[0] = data_02086e9c[0];
        src[1] = data_02086e9c[1];
        src[2] = data_02086e9c[2];

        {
            PlayerView *player = cam->mTargetPlayer;
            Vec3_RotateYAndTranslate(pos, (int *)&player->mPos, player->mAngleY, src);
        }

        result = func_020091f8(self, (int)pos, cam->mTargetAngleY, 0);
        if (result != 0) {
            *(u8 *)(self + 0x1a6) = 0;
        }
    } else {
        int idx = gActivePlayerSlot;
        int off = idx * 0x18;
        s16 t1 = *(short*)((char*)data_0209f4a2 + off);
        int scaled1 = FX_MUL(t1, 0x200);

        cam->mAngleY -= scaled1;

        {
            s16 a186 = cam->mTargetAngleY;
            s16 diff = (s16)(cam->mAngleY - a186);
            if (diff < -0x5000) diff = -0x5000;
            else if (diff > 0x5000) diff = 0x5000;
            cam->mAngleY = (s16)(a186 + diff);
        }

        {
            s16 t2 = *(short*)((char*)data_0209f4a4 + off);
            int scaled2 = FX_MUL(t2, 0x200);
            cam->mAngleX += scaled2;
        }

        {
            s16 clamped = cam->mAngleX;
            if (clamped < -0x1c00) clamped = -0x1c00;
            else if (clamped > 0x1c00) clamped = 0x1c00;
            cam->mAngleX = clamped;
        }
    }

    {
        unsigned int r = func_020093f4(self, 0x48000);
        func_02009138((int*)self, (int)r);
    }
    func_020089d8(self);
    return 1;
}

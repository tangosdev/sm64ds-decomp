#include "types.h"
#include "nitro/fx/fx.h"
typedef struct { int x, y, z; } Vec3i;
typedef struct { short x, y, z; } Vec3s;

extern int func_020538b8(int a, int b);
extern int func_ov007_020b7690(int i);
extern int func_02053644(void *v);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void func_02053380(void *a, void *b);

/* The fields this file reads; func_ov007_020be9ac calls it with the same
 * object (its b0/b1 bytes and +0x18 callback are not read here).
 * mOwner is the animated object it tracks: its +0x98/+0x94 words gate the
 * update below, and its first word leads (via +0xc) to the node `P` whose
 * +0x24 position this follows. mYaw takes func_020538b8 (atan2) of P's
 * +0x18 x/z; mPrevPos holds P's position from the last gated update, so
 * `dd` is the motion since then. mSlot picks the element
 * func_ov007_020b7690 returns. */
typedef struct BoneFollower {
    u8 pad_00[2];
    u8 mSlot; /* 0x02 */
    u8 pad_03;
    char *mOwner;   /* 0x04 */
    int mYaw;       /* 0x08 */
    Vec3i mPrevPos; /* 0x0c */
} BoneFollower;

void func_ov007_020beb80(char *self) {
    BoneFollower *f = (BoneFollower *)self;
    char *B = *(char **)f->mOwner;
    char *P = *(char **)(B + 0xc);
    int *q = (int *)(int)(P + 0x18);
    int r;
    Vec3s dd;
    char *e;
    int frames;
    int vel;
    int sc;
    Vec3i *tp = (Vec3i *)(int)(P + 0x24);
    r = func_020538b8(q[0], q[2]);
    dd.x = (short)(tp->x - f->mPrevPos.x);
    dd.y = (short)(tp->y - f->mPrevPos.y);
    dd.z = (short)(tp->z - f->mPrevPos.z);
    if (*(int *)(f->mOwner + 0x98) != 0) {
        f->mYaw = r;
        f->mPrevPos = *tp;
    }
    e = (char *)func_ov007_020b7690(f->mSlot);
    {
        char *A = f->mOwner;
        if (*(int *)(A + 0x94) != 0 && (unsigned)*(*(int **)(A + 0x18)) <= 1) {
            vel = 0;
            frames = 0x1000;
            if (dd.x != 0 || dd.y != 0 || dd.z != 0) {
                int mag = (short)func_02053644(&dd);
                int t;
                if (mag <= 0) sc = 0;
                else if (mag >= 0xfa0) sc = 0x1000;
                else sc = _ZN4cstd4fdivEii(mag, 0xfa0);
                if (mag <= 0x1f4) t = 0;
                else if (mag >= 0x3e8) t = 0x1000;
                else t = _ZN4cstd4fdivEii(mag - 0x1f4, 0x1f4);
                frames -= t;
                func_02053380(&dd, &dd);
                *(Vec3s *)(e + 0x3c) = dd;
                vel = (short)FX_MUL(sc, 0x460);
            }
            *(u8 *)(e + 0x58) = (u8)(((frames << 2) >> 12) + 1);
            *(int *)(e + 0x4c) = vel;
            *(int *)((int)e + 0x1c) &= ~2;
            *(Vec3i *)(e + 0x20) = *tp;
        } else {
            *(int *)((int)e + 0x1c) |= 2;
        }
    }
}

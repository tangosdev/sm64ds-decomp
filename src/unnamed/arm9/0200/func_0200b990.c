#include "types.h"
#include "nitro/fx/fx.h"
typedef struct V3 { int x, y, z; } V3;
typedef struct dBgCh_Lin { char pad[0x14]; char surf[0x64]; } dBgCh_Lin;

extern s16 Vec3_HorzAngle(const V3 *a, const V3 *b);
extern void *_ZN9dBgCh_LinC1Ev(dBgCh_Lin *rc);
extern void func_0200897c(char *self, dBgCh_Lin *rc);
extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(dBgCh_Lin *rc, V3 *a, V3 *b, void *actor);
extern int _ZN9dBgCh_Lin10DetectClsnEv(dBgCh_Lin *rc);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *surf, V3 *out);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
extern int AngleDiff(int a, int b);
extern void _ZN9dBgCh_LinD1Ev(dBgCh_Lin *rc);
extern int _Z14ApproachLinearRsss(short *value, short target, short step);
extern signed char data_0209f2f8;

/* File-local views (the class headers are C++): only the fields this helper
 * touches. Camera names follow include/dCamera_c.h; the player's follow
 * include/dActor_c.h. */
typedef struct {
    u8 pad_000[0x8e];
    s16 mAngleY; /* 0x08e */
} PlayerView;

typedef struct {
    u8 pad_000[0x80];
    V3 lookAt; /* 0x080 */
    u8 pad_08c[0x84];
    PlayerView *mTargetPlayer; /* 0x110 */
    u8 pad_114[0x68];
    s16 mAngleY; /* 0x17c */
} CameraView;

void func_0200b990(char *self, char *arg1, int arg2)
{
    CameraView *cam = (CameraView *)self;
    V3 v;
    s16 angle;
    u8 b0, b1;
    int step;
    int rate;

    {
        int sz = (int)(*(s16 *)(arg1 + 6)) << 12;
        int sy = (int)(*(s16 *)(arg1 + 4)) << 12;
        int sx = (int)(*(s16 *)(arg1 + 2)) << 12;
        v.x = sx;
        v.y = sy;
        v.z = sz;
    }
    angle = Vec3_HorzAngle(&v, &cam->lookAt);

    rate = 8;
    b0 = *(u8 *)(arg1 + 0);
    if (b0 == 0) {
        b1 = *(u8 *)(arg1 + 1);
        if (b1 == 0xff) {
            signed char zone = data_0209f2f8;
            if (zone != 0x16 && zone != 0xa && zone != 0x10) {
                dBgCh_Lin rc;
                V3 lineB;
                _ZN9dBgCh_LinC1Ev(&rc);
                func_0200897c(self, &rc);
                lineB.y = cam->lookAt.y;
                lineB.x = v.x;
                lineB.z = v.z;
                _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(&rc, &cam->lookAt, &lineB, 0);
                if (_ZN9dBgCh_Lin10DetectClsnEv(&rc) != 0) {
                    V3 normal;
                    s16 a2;
                    _ZNK11SurfaceInfo12CopyNormalToER7Vector3(rc.surf, &normal);
                    a2 = _ZN4cstd5atan2E5Fix12IiES1_(normal.x, normal.z);
                    if (AngleDiff(a2, angle) < 0x1800) angle = a2;
                }
                _ZN9dBgCh_LinD1Ev(&rc);
            }
            step = 0x2000;
        } else {
            step = 0;
            rate = (data_0209f2f8 == 0x2d) ? 0x12 : 0xf;
        }
    } else {
        step = 0x6800;
    }

    {
        PlayerView *ptr = cam->mTargetPlayer;
        s16 target = ptr->mAngleY;
        s16 diff = (s16)(angle - target);
        if (diff < 0) {
            angle -= step;
        } else {
            angle += step;
        }
    }

    {
        int fixmul = FX_MUL(arg2, rate);
        _Z14ApproachLinearRsss(&cam->mAngleY, angle, (s16)fixmul);
    }
}

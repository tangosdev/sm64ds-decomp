#include "nitro/fx/fx.h"
typedef struct { int x, y, z; } Vector3;

extern short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern void Vec3_RotateYAndTranslate(int *out, int *in, short angle, int *src);
extern int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
extern void func_0200897c(void *self, void *arg);
extern int func_020092c4(int arg0, Vector3 *out, Vector3 *target);

extern void *_ZN9dBgCh_GndC1Ev(void *self);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void *self, const Vector3 *pos, void *actor);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(void *self);
extern void _ZN9dBgCh_GndD1Ev(void *self);

extern Vector3 data_02086e84;

/* File-local view of dCamera_c (include/dCamera_c.h is C++): the fields this
 * helper touches. */
typedef struct {
    char pad_000[0x80];
    Vector3 lookAt; /* 0x080 */
    Vector3 pos;    /* 0x08c */
    /* func_020089f8 copies mTargetPlayer->mPos here every frame. */
    Vector3 mTargetPos; /* 0x098 */
    char pad_0a4[0xb0];
    int mFlags; /* 0x154 */
    char pad_158[0x24];
    short mAngleY; /* 0x17c */
} CameraView;

void func_0200b590(char *c, short *a1, int arg2)
{
    CameraView *cam = (CameraView *)c;
    Vector3 v0;
    Vector3 rotated;
    short angle1;
    short angle2;
    int diff;
    short newAngle;
    Vector3 pos;
    Vector3 base;
    char rg[0x50];
    int dx, dz;
    int hitY;
    int dist;
    int rem;
    int selfZ;
    int selfX;

    {
        int tz = a1[3] << 12;
        int ty = a1[2] << 12;
        int tx = a1[1] << 12;
        v0.x = tx;
        v0.y = ty;
        v0.z = tz;
    }

    angle1 = Vec3_HorzAngle(&cam->mTargetPos, &v0);
    angle2 = Vec3_HorzAngle(&cam->pos, &v0);
    diff = (short)(angle2 - angle1);
    if (diff < -0x4000)
        diff = -0x4000;
    else if (diff > 0x4000)
        diff = 0x4000;
    newAngle = (short)(angle1 + diff);

    base.x = data_02086e84.x;
    base.y = data_02086e84.y;
    base.z = data_02086e84.z;
    Vec3_RotateYAndTranslate((int *)&rotated, (int *)&v0, newAngle, (int *)&base);
    rotated.y = arg2;

    selfZ = cam->lookAt.z;
    dz = rotated.z - selfZ;
    selfX = cam->lookAt.x;
    dx = rotated.x - selfX;
    pos.y = arg2 + 0x100000;

    pos.x = selfX + FX_MUL(dx, 0xb33);
    pos.z = selfZ + FX_MUL(dz, 0xb33);

    _ZN9dBgCh_GndC1Ev(rg);
    func_0200897c(c, rg);
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(rg, &pos, 0);
    if (_ZN9dBgCh_Gnd10DetectClsnEv(rg)) {
        hitY = *(int *)(rg + 0x44);
        rotated.y = hitY;
        if (hitY < arg2)
            rotated.y = arg2;
    }
    rotated.y = rotated.y + 0x7d000;
    dist = Vec3_HorzDist(&cam->pos, &cam->mTargetPos);
    rem = 0x100000 - dist;
    if (rem > 0)
        rotated.y += rem;
    if (cam->mFlags & 4) {
        cam->pos.x = rotated.x;
        cam->pos.y = rotated.y;
        cam->pos.z = rotated.z;
    } else {
        func_020092c4((int)c, &cam->pos, &rotated);
    }

    cam->mAngleY = Vec3_HorzAngle(&cam->lookAt, &cam->pos);
    _ZN9dBgCh_GndD1Ev(rg);
}

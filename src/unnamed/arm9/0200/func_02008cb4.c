#include "types.h"
enum { false, true };

typedef struct Vec3 { int x, y, z; } Vec3;
typedef struct dBgCh_Lin { char pad[0x14]; char surf[0x64]; } dBgCh_Lin;

extern int func_ov002_020ca270(char* p);
extern void Vec3_Sub(Vec3* out, Vec3* a, Vec3* b);
extern int LenVec3(Vec3* v);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Vec3_MulScalarInPlace(Vec3* v, int s);
extern void Vec3_Add(Vec3* out, Vec3* a, Vec3* b);
extern void *_ZN9dBgCh_LinC1Ev(dBgCh_Lin* rc);
extern void func_0200897c(char* self, dBgCh_Lin* rc);
extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(dBgCh_Lin* rc, Vec3* a, Vec3* b, void* actor);
extern int _ZN9dBgCh_Lin10DetectClsnEv(dBgCh_Lin* rc);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* surf, Vec3* out);
extern int DotVec3(Vec3* a, Vec3* b);
extern void _ZN9dBgCh_LinD1Ev(dBgCh_Lin* rc);
/* local extern: C99 cannot name the canonical C++ member; actual Vector3 aggregate return preserves its hidden-result ABI. */
extern Vector3 _ZN9dBgCh_Lin10GetClsnPosEv(dBgCh_Lin* rc);
extern int Vec3_Dist(Vec3* a, Vec3* b);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
extern int AngleDiff(int a, int b);

/* File-local view of dCamera_c (include/dCamera_c.h is C++): the fields this
 * collision step touches. 0xe0, 0xec and 0xf4 stay raw: they sit in the
 * header's unnamed gap, and this function only reads them. 0x1a0 stays raw
 * too: cleared here beside 0x1a6, with no reader here to show its role.
 * 0x1a6 (the header's unk_1a6) stays raw: set on entering a state
 * (func_02009d30) and cleared once func_020091f8 reports arrival, which is
 * not yet enough to name it. */
typedef struct {
    u8 pad_000[0x80];
    Vec3 lookAt; /* 0x080 */
    Vec3 pos;    /* 0x08c */
    /* func_020089f8 copies mTargetPlayer->mPos here every frame. */
    Vec3 mTargetPos; /* 0x098 */
    u8 pad_0a4[0x30];
    /* Last frame's pos (Behavior copies it here before moving). */
    Vec3 mPrevPos; /* 0x0d4 */
    u8 pad_0e0[0x30];
    /* + 0xc is the player's actor ID (include/dActor_c.h). */
    char *mTargetPlayer; /* 0x110 */
    u8 pad_114[0x40];
    u32 mFlags; /* 0x154 */
    u8 pad_158[0x2e];
    /* The yaw func_020091f8 eases mAngleY toward. */
    s16 mTargetAngleY; /* 0x186 */
} CameraView;

int func_02008cb4(char* c)
{
    CameraView *cam = (CameraView *)c;
    Vec3 v0;
    Vec3 dir1;
    Vec3 end1;
    Vec3 n1;
    Vec3 dir2;
    Vec3 end2;
    Vec3 n2;
    Vec3 diff;
    Vector3 cp1;
    Vector3 cp2;
    Vec3 np;
    Vec3 cl;
    dBgCh_Lin rc1;
    dBgCh_Lin rc2;
    int vy;
    int vz;
    int vx;
    int scale;
    int len;
    int d;

    {
        char *p = cam->mTargetPlayer;
        int b = *(u16*)(p + 0xc);
        b = b == 0xbf;
        if (b != false) {
            if (func_ov002_020ca270(p) != 0)
                return 0;
        }
    }

    scale = 0x200;
    if (cam->mFlags & 0x40200) {
        vz = cam->mTargetPos.z;
        vy = cam->mTargetPos.y + 0x82500;
        vx = cam->mTargetPos.x;
        v0.x = vx;
        v0.y = vy;
        v0.z = vz;
        Vec3_Sub(&dir1, &cam->pos, &v0);
        len = LenVec3(&dir1);
        if (len != 0) {
            Vec3_MulScalarInPlace(&dir1, _ZN4cstd4fdivEii(0x100000, len));
            Vec3_Add(&end1, &cam->pos, &dir1);
            _ZN9dBgCh_LinC1Ev(&rc1);
            func_0200897c(c, &rc1);
            _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(&rc1, &end1, &v0, 0);
            if (_ZN9dBgCh_Lin10DetectClsnEv(&rc1) != 0) {
                _ZNK11SurfaceInfo12CopyNormalToER7Vector3(rc1.surf, &n1);
                if (DotVec3((Vec3*)(c + 0xec), &n1) > 0) {
                    cam->mFlags &= ~0x80000;
                    _ZN9dBgCh_LinD1Ev(&rc1);
                    return 0;
                }
                cp1 = _ZN9dBgCh_Lin10GetClsnPosEv(&rc1);
                if (Vec3_Dist((Vec3*)(c + 0xe0), (Vec3*)&cp1) < 0x80000) {
                    cam->mFlags &= ~0x80000;
                    _ZN9dBgCh_LinD1Ev(&rc1);
                    return 0;
                }
                {
                    u32 f = cam->mFlags;
                    if ((f & 0x40000) && *(u8 *)(c + 0x1a6) != 0 && !(f & 0x80100)) {
                        int a = _ZN4cstd5atan2E5Fix12IiES1_(n1.x, n1.z);
                        if (AngleDiff(a, _ZN4cstd5atan2E5Fix12IiES1_(*(int*)(c + 0xec), *(int*)(c + 0xf4))) > 0x6000) {
                            cam->mTargetAngleY += 0x8000;
                            cam->mFlags |= 0x80000;
                        }
                    }
                }
            }
            _ZN9dBgCh_LinD1Ev(&rc1);
        }
        scale = 0x1000;
    }

    {
        u32 f = cam->mFlags;
        if (f & 0x80100) {
            vz = cam->mTargetPos.z;
            vy = (f & 0x100) ? cam->pos.y : 0x82500 + cam->mTargetPos.y;
            vx = cam->mTargetPos.x;
            v0.x = vx;
            v0.y = vy;
            v0.z = vz;
        } else {
            scale = 0x1000;
            v0.x = cam->mPrevPos.x;
            v0.y = cam->mPrevPos.y;
            v0.z = cam->mPrevPos.z;
        }
    }

    Vec3_Sub(&dir2, &cam->pos, &v0);
    len = LenVec3(&dir2);
    if (len == 0) {
        cam->mFlags &= ~0x80000;
        return 0;
    }
    Vec3_MulScalarInPlace(&dir2, _ZN4cstd4fdivEii(0x60000, len));
    Vec3_Add(&end2, &cam->pos, &dir2);
    _ZN9dBgCh_LinC1Ev(&rc2);
    func_0200897c(c, &rc2);
    _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(&rc2, &v0, &end2, 0);
    if (_ZN9dBgCh_Lin10DetectClsnEv(&rc2) == 0) {
        cam->mFlags &= ~0x80000;
        _ZN9dBgCh_LinD1Ev(&rc2);
        return 0;
    }
    if (!(cam->mFlags & 0x80100)) {
        int t;
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3(rc2.surf, &n2);
        t = n2.y;
        if (t < 0) t = -t;
        if (t > 0xb50) {
            _ZN9dBgCh_LinD1Ev(&rc2);
            return 0;
        }
    }
    d = Vec3_Dist(&cam->pos, &cam->lookAt);
    Vec3_MulScalarInPlace(&dir2, scale);
    cp2 = _ZN9dBgCh_Lin10GetClsnPosEv(&rc2);
    Vec3_Sub(&np, (Vec3*)&cp2, &dir2);
    cam->pos.x = np.x;
    cam->pos.y = np.y;
    cam->pos.z = np.z;
    Vec3_Sub(&diff, &cam->pos, &cam->lookAt);
    if (LenVec3(&diff) >= d) {
        Vec3_MulScalarInPlace(&diff, _ZN4cstd4fdivEii(d, LenVec3(&diff)));
        Vec3_Add(&cl, &cam->lookAt, &diff);
        cam->pos.x = cl.x;
        cam->pos.y = cl.y;
        cam->pos.z = cl.z;
    }
    if (*(u8 *)(c + 0x1a6) != 0 || *(u16 *)(c + 0x1a0) != 0) {
        *(u8 *)(c + 0x1a6) = 0;
        *(u16*)(c + 0x1a0) = 0;
    }
    cam->mFlags &= ~0x80000;
    _ZN9dBgCh_LinD1Ev(&rc2);
    return 1;
}

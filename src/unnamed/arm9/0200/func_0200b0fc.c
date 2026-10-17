#include "types.h"
// @symbol func_0200b0fc
/* recovered: shared common types, declarations from a shared header */
#include "decl_PathPtr.h"
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
#include "nitro/fx/fx.h"
extern void *_ZN7PathPtrC1Ev(void *self);
extern void _ZN7PathPtr6FromIDEj(void *self, unsigned int id);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void *self, struct Vector3 *out, unsigned int idx);
extern void Vec3_Sub(struct Vector3 *out, struct Vector3 *a, struct Vector3 *b);
extern int LenVec3(struct Vector3 *v);
extern int NormalizeVec3IfNonZero(struct Vector3 *v);
extern int DotVec3(const struct Vector3 *a, const struct Vector3 *b);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Vec3_MulScalarInPlace(int *v, int s);
extern void Vec3_Add(struct Vector3 *out, struct Vector3 *a, struct Vector3 *b);
extern void Vec3_MulScalar(struct Vector3 *out, const struct Vector3 *in, int scale);
extern void AddVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern int func_020092c4(void *arg0, struct Vector3 *out, struct Vector3 *target);
extern short Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
extern signed char data_0209f2f8;

/* File-local view of dCamera_c (include/dCamera_c.h is C++): the fields this
 * helper touches. 0x12c stays raw: Behavior derives it from a distance, but
 * nothing here shows its role. */
typedef struct {
    u8 pad_000[0x80];
    struct Vector3 lookAt; /* 0x080 */
    struct Vector3 pos;    /* 0x08c */
    u8 pad_098[0xbc];
    int mFlags; /* 0x154 */
    u8 pad_158[0x24];
    s16 mAngleY; /* 0x17c */
    u8 pad_17e[0x29];
    /* The path node this camera is past: cleared while flag 4 is set, then
       stepped forward or back as lookAt nears the next or previous node. */
    u8 mPathNode; /* 0x1a7 */
} CameraView;

void func_0200b0fc(char *c, char *a)
{
    CameraView *cam = (CameraView *)c;
    char path[8];
    struct Vector3 node0;
    struct Vector3 node1;
    struct Vector3 dir;
    struct Vector3 toPos;
    struct Vector3 toTarget;
    struct Vector3 cross1;
    struct Vector3 side;
    int idx;
    int projSide;
    int projDir;
    int diff;
    int maxDelta;
    int negMax;
    int clamped;
    int segLen;
    struct Vector3 node2;
    struct Vector3 tmp;
    struct Vector3 diffBack;
    int lenFwd;
    int lenBack;
    struct Vector3 diffFwd2;
    struct Vector3 prevNode;
    struct Vector3 diffPrev;
    int lenA;
    int lenB;
    struct Vector3 targetPos;
    struct Vector3 farPoint;
    struct Vector3 diffFar;
    struct Vector3 back;
    struct Vector3 farBack;
    struct Vector3 vecFwd2;
    struct Vector3 farFwd2;
    struct Vector3 farPrev;
    struct Vector3 diffFarPrev;
    struct Vector3 offset;
    int v4;
    int sideScale;

    _ZN7PathPtrC1Ev(path);
    _ZN7PathPtr6FromIDEj(path, *(unsigned char*)(a + 1));

    if (cam->mFlags & 4) {
        cam->mPathNode = 0;
    }
    idx = cam->mPathNode + 1;

    _ZNK7PathPtr7GetNodeER7Vector3j(path, &node0, cam->mPathNode);
    _ZNK7PathPtr7GetNodeER7Vector3j(path, &node1, idx);
    Vec3_Sub(&dir, &node1, &node0);
    segLen = LenVec3(&dir);
    NormalizeVec3IfNonZero(&dir);

    Vec3_Sub(&toPos, &cam->lookAt, &node0);
    Vec3_Sub(&toTarget, &cam->pos, &node0);
    func_0203cf00(&cross1, &toPos, &dir);
    func_0203cf00(&side, &dir, &cross1);
    NormalizeVec3IfNonZero(&side);

    projSide = DotVec3(&toPos, &side);
    projDir = DotVec3(&toPos, &dir);
    diff = projDir - DotVec3(&toTarget, &dir);

    if (_ZNK7PathPtr9GetUnk004Ev(path) == 0xff) {
        maxDelta = 0;
    } else {
        maxDelta = _ZNK7PathPtr9GetUnk004Ev(path) << 12;
    }
    negMax = -maxDelta;
    if (diff < negMax) {
        clamped = negMax;
    } else {
        if (diff <= maxDelta) maxDelta = diff;
        clamped = maxDelta;
    }
    projDir -= clamped;

    if (idx + 1 < _ZNK7PathPtr8NumNodesEv(path)) {
        _ZNK7PathPtr7GetNodeER7Vector3j(path, &node2, idx + 1);
        Vec3_Sub(&tmp, &node2, &node1);
        Vec3_MulScalarInPlace((int*)&tmp, _ZN4cstd4fdivEii(0x190000, LenVec3(&tmp)));
        Vec3_Add(&farPoint, &node1, &tmp);
        Vec3_Sub(&diffFar, &farPoint, &cam->lookAt);
        tmp.x = diffFar.x;
        tmp.y = diffFar.y;
        tmp.z = diffFar.z;

        Vec3_MulScalar(&back, &dir, -0x190000);
        Vec3_Add(&farBack, &node1, &back);
        Vec3_Sub(&diffBack, &farBack, &cam->lookAt);

        lenBack = LenVec3(&diffBack);
        lenFwd = LenVec3(&tmp);
        if (lenFwd < lenBack) {
            cam->mPathNode++;
        }
    } else if (_ZNK7PathPtr5LoopsEv(path) != 0) {
        if (projDir > segLen) projDir = segLen;
    }

    if (cam->mPathNode != 0 && cam->mPathNode != idx) {
        Vec3_MulScalar(&vecFwd2, &dir, 0x2bc000);
        Vec3_Add(&farFwd2, &node0, &vecFwd2);
        Vec3_Sub(&diffFwd2, &farFwd2, &cam->lookAt);

        _ZNK7PathPtr7GetNodeER7Vector3j(path, &prevNode, cam->mPathNode - 1);
        Vec3_Sub(&diffPrev, &prevNode, &node0);
        Vec3_MulScalarInPlace((int*)&diffPrev, _ZN4cstd4fdivEii(0x190000, LenVec3(&diffPrev)));
        Vec3_Add(&farPrev, &node0, &diffPrev);
        Vec3_Sub(&diffFarPrev, &farPrev, &cam->lookAt);
        diffPrev.x = diffFarPrev.x;
        diffPrev.y = diffFarPrev.y;
        diffPrev.z = diffFarPrev.z;

        lenA = LenVec3(&diffFwd2);
        lenB = LenVec3(&diffPrev);
        if (lenB < lenA) {
            cam->mPathNode--;
        }
    } else if (_ZNK7PathPtr5LoopsEv(path) != 0) {
        if (projDir < 0) projDir = 0;
    }

    Vec3_MulScalar(&offset, &dir, projDir);
    Vec3_Add(&targetPos, &node0, &offset);

    if (_ZNK7PathPtr13GetPullFactorEv(path) == 0xff) {
        v4 = 0;
    } else {
        v4 = _ZNK7PathPtr13GetPullFactorEv(path) << 4;
    }

    if (data_0209f2f8 == 0x2f) {
        projSide = projSide - FX_MUL(*(int *)(c + 0x12c), 0x5000);
        if (projSide < 0) {
            targetPos.y -= FX_MUL(projSide, 0x400);
            projSide = 0;
        }
    } else {
        projSide = FX_MUL(projSide, v4);
    }

    Vec3_MulScalarInPlace((int*)&side, projSide);
    AddVec3(&targetPos, &side, &targetPos);

    if (cam->mFlags & 4) {
        cam->pos.x = targetPos.x;
        cam->pos.y = targetPos.y;
        cam->pos.z = targetPos.z;
    } else {
        func_020092c4(c, &cam->pos, &targetPos);
    }

    cam->mAngleY = Vec3_HorzAngle(&cam->lookAt, &cam->pos);
}

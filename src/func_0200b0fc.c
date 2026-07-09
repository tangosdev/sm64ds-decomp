typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef unsigned int u32;

struct Vector3 { int x, y, z; };

extern void _ZN7PathPtrC1Ev(void *self);
extern void _ZN7PathPtr6FromIDEj(void *self, unsigned int id);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void *self, struct Vector3 *out, unsigned int idx);
extern void Vec3_Sub(struct Vector3 *out, struct Vector3 *a, struct Vector3 *b);
extern int LenVec3(struct Vector3 *v);
extern int NormalizeVec3IfNonZero(struct Vector3 *v);
extern void func_0203cf00(struct Vector3 *out, struct Vector3 *a, struct Vector3 *b);
extern int DotVec3(const struct Vector3 *a, const struct Vector3 *b);
extern unsigned int func_0203ad54(void *c);
extern int _ZNK7PathPtr8NumNodesEv(void *self);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Vec3_MulScalarInPlace(int *v, int s);
extern void Vec3_Add(struct Vector3 *out, struct Vector3 *a, struct Vector3 *b);
extern void Vec3_MulScalar(struct Vector3 *out, const struct Vector3 *in, int scale);
extern int _ZNK7PathPtr5LoopsEv(void *self);
extern unsigned int func_0203ad44(void *c);
extern void AddVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern int func_020092c4(void *arg0, struct Vector3 *out, struct Vector3 *target);
extern short Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
extern signed char data_0209f2f8;

void func_0200b0fc(char *c, char *a)
{
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

    if (*(int*)(c + 0x154) & 4) {
        *(u8*)(c + 0x1a7) = 0;
    }
    idx = *(u8*)(c + 0x1a7) + 1;

    _ZNK7PathPtr7GetNodeER7Vector3j(path, &node0, *(u8*)(c + 0x1a7));
    _ZNK7PathPtr7GetNodeER7Vector3j(path, &node1, idx);
    Vec3_Sub(&dir, &node1, &node0);
    segLen = LenVec3(&dir);
    NormalizeVec3IfNonZero(&dir);

    Vec3_Sub(&toPos, (struct Vector3*)(c + 0x80), &node0);
    Vec3_Sub(&toTarget, (struct Vector3*)(c + 0x8c), &node0);
    func_0203cf00(&cross1, &toPos, &dir);
    func_0203cf00(&side, &dir, &cross1);
    NormalizeVec3IfNonZero(&side);

    projSide = DotVec3(&toPos, &side);
    projDir = DotVec3(&toPos, &dir);
    diff = projDir - DotVec3(&toTarget, &dir);

    if (func_0203ad54(path) == 0xff) {
        maxDelta = 0;
    } else {
        maxDelta = func_0203ad54(path) << 12;
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
        Vec3_Sub(&diffFar, &farPoint, (struct Vector3*)(c + 0x80));
        tmp.x = diffFar.x;
        tmp.y = diffFar.y;
        tmp.z = diffFar.z;

        Vec3_MulScalar(&back, &dir, -0x190000);
        Vec3_Add(&farBack, &node1, &back);
        Vec3_Sub(&diffBack, &farBack, (struct Vector3*)(c + 0x80));

        lenBack = LenVec3(&diffBack);
        lenFwd = LenVec3(&tmp);
        if (lenFwd < lenBack) {
            u8 *p = (u8*)((long long)(c + 0x1a7) & 0xFFFFFFFFFFFFFFFFLL);
            *p = *p + 1;
        }
    } else if (_ZNK7PathPtr5LoopsEv(path) != 0) {
        if (projDir > segLen) projDir = segLen;
    }

    if (*(u8*)(c + 0x1a7) != 0 && *(u8*)(c + 0x1a7) != idx) {
        Vec3_MulScalar(&vecFwd2, &dir, 0x2bc000);
        Vec3_Add(&farFwd2, &node0, &vecFwd2);
        Vec3_Sub(&diffFwd2, &farFwd2, (struct Vector3*)(c + 0x80));

        _ZNK7PathPtr7GetNodeER7Vector3j(path, &prevNode, *(u8*)(c + 0x1a7) - 1);
        Vec3_Sub(&diffPrev, &prevNode, &node0);
        Vec3_MulScalarInPlace((int*)&diffPrev, _ZN4cstd4fdivEii(0x190000, LenVec3(&diffPrev)));
        Vec3_Add(&farPrev, &node0, &diffPrev);
        Vec3_Sub(&diffFarPrev, &farPrev, (struct Vector3*)(c + 0x80));
        diffPrev.x = diffFarPrev.x;
        diffPrev.y = diffFarPrev.y;
        diffPrev.z = diffFarPrev.z;

        lenA = LenVec3(&diffFwd2);
        lenB = LenVec3(&diffPrev);
        if (lenB < lenA) {
            u8 *p = (u8*)((long long)(c + 0x1a7) & 0xFFFFFFFFFFFFFFFFLL);
            *p = *p - 1;
        }
    } else if (_ZNK7PathPtr5LoopsEv(path) != 0) {
        if (projDir < 0) projDir = 0;
    }

    Vec3_MulScalar(&offset, &dir, projDir);
    Vec3_Add(&targetPos, &node0, &offset);

    if (func_0203ad44(path) == 0xff) {
        v4 = 0;
    } else {
        v4 = func_0203ad44(path) << 4;
    }

    if (data_0209f2f8 == 0x2f) {
        projSide = projSide - (int)(((long long)*(int*)(c + 0x12c) * 0x5000 + 0x800) >> 12);
        if (projSide < 0) {
            targetPos.y -= (int)(((long long)projSide * 0x400 + 0x800) >> 12);
            projSide = 0;
        }
    } else {
        projSide = (int)(((long long)projSide * v4 + 0x800) >> 12);
    }

    Vec3_MulScalarInPlace((int*)&side, projSide);
    AddVec3(&targetPos, &side, &targetPos);

    if (*(int*)(c + 0x154) & 4) {
        *(int*)(c + 0x8c) = targetPos.x;
        *(int*)(c + 0x90) = targetPos.y;
        *(int*)(c + 0x94) = targetPos.z;
    } else {
        func_020092c4(c, (struct Vector3*)(c + 0x8c), &targetPos);
    }

    *(s16*)(c + 0x17c) = Vec3_HorzAngle((struct Vector3*)(c + 0x80), (struct Vector3*)(c + 0x8c));
}

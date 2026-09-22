/*
 * ov006 minigame: per-frame gate-crossing check over the five tracked
 * objects (data_ov006_0214097c).  For each live object, project its
 * position and target onto the gate frame (c+0x14 / c+0x20 basis, c+0x58
 * half-width).  A segment that crosses the gate records the hit
 * (c+0x38 position, c+0x44 direction, state 1 or 2 with a sound whose
 * pitch is lerped from the crossing point), decrements the remaining
 * count at c+0x324 and returns 1 when it reaches zero.  Segments that
 * miss just outside the gate edges are marked state 3.
 *
 * Shape notes: the +-1 clamps are a ternary macro so the constants rank
 * above a/i in the callee-saved band and still hoist in the compiler's
 * own order; arr is assigned after the null/active checks so its pool
 * load sits in the hoisted-constant sequence; the +0x320/+0x324 halfword
 * accesses go through c directly, which yields the ROM's shared
 * c+0x300 base and the copied read.
 */
#define CLAMP(v, lo, hi) ((v) < (lo) ? (lo) : (v) > (hi) ? (hi) : (v))
#pragma opt_common_subs off
#pragma opt_propagation off
#include "types.h"

struct V2 { int x, y; };

struct VT {
    Vector3 *(*GetPos)(void *);
    Vector3 *(*GetTargetPos)(void *);
    void (*Pad08)(void *);
    int (*IsActive)(void *);
};
struct Cannon {
    struct VT *vt;
    Vector3 v4;
    int f10;
    int f14;
    u16 f18;
};

extern void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
extern void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern int DotVec3(Vector3 *a, Vector3 *b);
extern void Vec2_Sub(int *o, int *a, int *b);
extern int func_0203d524(int *a, int *b);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void func_ov006_020e6db4(int a0, int a1, int a2);
extern void func_ov006_020cf040(char *sl, void *arg1, Vector3 *r2);
extern void func_ov006_020cfa28(char *p);
extern void Vec3_MulScalar(Vector3 *out, Vector3 *in, int scale);
extern void Vec3_Add(Vector3 *out, Vector3 *a, Vector3 *b);

extern struct Cannon *data_ov006_0214097c[];
extern int data_ov006_0213b30c;
extern int data_ov006_0213b310;
extern int data_ov006_0213b2f4;
extern int data_ov006_0213b308;

int func_ov006_020cfc74(char *c)
{
    int i;
    int a;
    struct Cannon **arr;

    i = 0;
    do {
        int b, cc, dd;
        Vector3 pos0, pos1, diff;
        struct V2 p1proj, p0proj;
        struct V2 negW, posW;
        struct V2 gateDir, aTest, bTest;
        int flag, s1, s2;
        Vector3 sumPt, diffPt, farFwd, farBack;
        Vector3 *p0;
        Vector3 *p1;

        if (data_ov006_0214097c[i] == 0)
            continue;
        if (data_ov006_0214097c[i]->vt->IsActive(data_ov006_0214097c[i]) == 0)
            continue;
        arr = data_ov006_0214097c;

        p0 = arr[i]->vt->GetPos(arr[i]);
        pos0.x = p0->x;
        pos0.y = p0->y;
        pos0.z = p0->z;

        p1 = arr[i]->vt->GetTargetPos(arr[i]);
        pos1.x = p1->x;
        pos1.y = p1->y;
        pos1.z = p1->z;

        Vec3_Sub(&diff, &pos0, &pos1);
        SubVec3(&pos0, (Vector3 *)(c + 8), &pos0);
        SubVec3(&pos1, (Vector3 *)(c + 8), &pos1);

        a = DotVec3((Vector3 *)(c + 0x20), &pos0);
        b = DotVec3((Vector3 *)(c + 0x14), &pos0);
        cc = DotVec3((Vector3 *)(c + 0x20), &pos1);
        dd = DotVec3((Vector3 *)(c + 0x14), &pos1);
        DotVec3((Vector3 *)(c + 0x14), &diff);
        DotVec3((Vector3 *)(c + 0x20), &diff);

        p0proj.y = b;
        p1proj.x = cc;
        p0proj.x = a;
        p1proj.y = dd;

        negW.x = -(*(int *)(c + 0x58));
        negW.y = 0;
        posW.x = *(int *)(c + 0x58);
        posW.y = 0;

        Vec2_Sub((int *)&gateDir, (int *)&negW, (int *)&posW);
        Vec2_Sub((int *)&aTest, (int *)&negW, (int *)&p1proj);
        Vec2_Sub((int *)&bTest, (int *)&negW, (int *)&p0proj);

        flag = 0;
        s1 = func_0203d524((int *)&gateDir, (int *)&aTest);
        s2 = func_0203d524((int *)&gateDir, (int *)&bTest);
        s1 = CLAMP(s1, -1, 1);
        s2 = CLAMP(s2, -1, 1);

        if (s1 * s2 <= 0 && s1 > s2) {
            struct V2 edge, e1, e2;
            int t1, t2;
            Vec2_Sub((int *)&edge, (int *)&p1proj, (int *)&p0proj);
            gateDir = edge;
            Vec2_Sub((int *)&e1, (int *)&p1proj, (int *)&negW);
            aTest = e1;
            Vec2_Sub((int *)&e2, (int *)&p1proj, (int *)&posW);
            bTest = e2;

            t1 = func_0203d524((int *)&gateDir, (int *)&aTest);
            t2 = func_0203d524((int *)&gateDir, (int *)&bTest);
            t1 = CLAMP(t1, -1, 1);
            t2 = CLAMP(t2, -1, 1);
            if (t1 * t2 <= 0) flag = 1;
        }

        if (flag != 0) {
            int mag, t;
            *(int *)(c + 0x38) = pos0.x;
            *(int *)(c + 0x3c) = pos0.y;
            *(int *)(c + 0x40) = pos0.z;
            *(int *)(c + 0x44) = diff.x;
            *(int *)(c + 0x48) = diff.y;
            *(int *)(c + 0x4c) = diff.z;

            t = _ZN4cstd4fdivEii((a < 0) ? -a : a, *(int *)(c + 0x58));
            {
                int w = *(int *)(c + 0x58);
                int av = (a < 0) ? -a : a;
                mag = (int)(((long long)av * w + 0x800) >> 12);
            }

            if (mag < 0x400000) {
                arr[i]->f18 = 2;
                func_ov006_020e6db4(0x1b1, *(int *)(c + 8),
                    (data_ov006_0213b310 * t + data_ov006_0213b30c * (0x1000 - t)) >> 12);
            } else {
                arr[i]->f18 = 1;
                func_ov006_020e6db4(0x1ae, *(int *)(c + 8),
                    (data_ov006_0213b308 * t + data_ov006_0213b2f4 * (0x1000 - t)) >> 12);
            }

            {
                int *dst = (int *)((char *)arr[i] + 4);
                dst[0] = *(int *)(c + 0x14);
                dst[1] = *(int *)(c + 0x18);
                dst[2] = *(int *)(c + 0x1c);
                arr[i]->f10 = a;
                arr[i]->f14 = *(int *)(c + 0x58);
                *(u16 *)(c + 0x320) = 0;
                func_ov006_020cf040(c, (void *)(c + 0x38), (Vector3 *)(c + 0x44));

                *(u16 *)(c + 0x324) -= 1;
                if (*(u16 *)(c + 0x324) == 0) {
                    *(u8 *)(c + 0x328) = 3;
                    func_ov006_020cfa28(c);
                    return 1;
                }
                *(u16 *)(c + 0x326) += 1;
            }
        } else {
            Vec3_MulScalar(&farFwd, (Vector3 *)(c + 0x20), *(int *)(c + 0x58));
            Vec3_Add(&sumPt, &pos0, &farFwd);
            Vec3_MulScalar(&farBack, (Vector3 *)(c + 0x20), *(int *)(c + 0x58));
            Vec3_Sub(&diffPt, &pos0, &farBack);

            if (sumPt.y < 0 && sumPt.y > -0x30000) {
                if (sumPt.x > -0x8000 && sumPt.x < 0) {
                    if (diff.x > 0) {
                        arr[i]->f18 = 3;
                    }
                }
            } else {
                if (diffPt.y < 0 && diffPt.y > -0x30000 && diffPt.x > 0 && diffPt.x < 0x8000 && diff.x < 0) {
                    arr[i]->f18 = 3;
                }
            }
        }
    } while (++i < 5);
    return 0;
}

/* func_ov007_020c19cc at 0x020c19cc (ov007, size 0x3ac)
 *
 * Title-screen bottom-screen doodle: emits the GX command stream for one
 * stylus stroke. num==1 draws a single quad billboard at the point; num>=2
 * walks the polyline, computing each segment's heading (func_020538b8),
 * smoothing each joint as prev + (s16)(ang-prev)/2, rotating a quad via the
 * sine table, and lerping the final point toward its predecessor by t/0x1000.
 *
 * Matched byte-for-byte with mwccarm 2004/b56.
 */
typedef short s16;
typedef unsigned short u16;
typedef long long s64;

struct V3 { int x, y, z; };
struct Mat3 { int m[9]; };

extern int func_020538b8(int x, int y);
extern void Matrix3x3_SetRotationZ(struct Mat3 *m, int s, int c);
extern void func_0205532c(int *a, int *v);
extern short data_02082214[];

static inline int lerp(int a, int b, int t)
{
    if (t >= 0x1000) return b;
    if (t <= 0) return a;
    return (int)((((s64)(0x1000 - t)) * a + ((s64)t) * b) >> 12);
}

void func_ov007_020c19cc(int num, struct V3 *pts, int sz0, int t)
{
    int szhi;
    int nszhi;
    int prev;
    struct V3 *q;
    struct V3 tmp;
    struct Mat3 mtx;
    int i;
    int ang;
    struct V3 *p;
    struct V3 *vp;
    int idx;
    u16 usz;

    usz = (u16)(s16)sz0;
    szhi = usz << 16;
    nszhi = (u16)(-(s16)sz0) << 16;
    ang = 0;

    if (num < 2 && num == 1) {
        u16 nn = (u16)(s16)(-(s16)sz0);
        *(volatile int *)0x04000500 = 3;
        *(volatile int *)0x04000444 = 0;
        {
            int z = pts[0].z;
            int y = pts[0].y;
            int x = pts[0].x;
            *(volatile int *)0x04000470 = x;
            *(volatile int *)0x04000470 = y;
            *(volatile int *)0x04000470 = z;
        }
        *(volatile int *)0x0400048C = szhi | nn;
        *(volatile int *)0x0400048C = 0;
        *(volatile int *)0x04000494 = nn | (nn << 16);
        *(volatile int *)0x0400048C = usz | szhi;
        *(volatile int *)0x0400048C = 0;
        *(volatile int *)0x04000494 = usz | (nn << 16);
        *(volatile int *)0x04000448 = 1;
        *(volatile int *)0x04000504 = 0;
        return;
    }

    *(volatile int *)0x04000500 = 3;
    i = 0;
    if (num > 0) {
        p = pts;
        q = pts;
        do {
            prev = ang;
            *(volatile int *)0x04000444 = 0;
            vp = p;
            if (i < num - 1) {
                ang = func_020538b8(pts[i + 1].y - q->y, pts[i + 1].x - q->x);
                if (i == 0) {
                    prev = ang;
                }
            } else if (t != 0x1000) {
                tmp.x = lerp(pts[i - 1].x, p->x, t);
                tmp.y = lerp(pts[i - 1].y, p->y, t);
                tmp.z = lerp(pts[i - 1].z, p->z, t);
                vp = &tmp;
            }
            idx = (u16)((s16)prev + (s16)((s16)(ang - prev) / 2)) >> 4;
            Matrix3x3_SetRotationZ(&mtx, data_02082214[idx * 2], data_02082214[idx * 2 + 1]);
            func_0205532c((int *)&mtx, (int *)vp);
            *(volatile int *)0x0400048C = szhi;
            *(volatile int *)0x0400048C = 0;
            *(volatile int *)0x04000494 = nszhi;
            *(volatile int *)0x04000448 = 1;
            p++;
            q++;
        } while (++i < num);
    }
    *(volatile int *)0x04000504 = 0;
}

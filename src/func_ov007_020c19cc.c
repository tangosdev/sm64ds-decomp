#include "common.h"
extern int func_020538b8(int y, int x);
extern void Matrix3x3_SetRotationZ(int *m, int s, int c);
extern void func_0205532c(int *a, int *v);
extern s16 data_02082214[];

void func_ov007_020c19cc(int count, Vector3 *pts, s16 half, int t)
{
    int z;
    int nh;
    u32 vy_pos;
    u32 vy_neg;
    int i;
    int ang;
    Vector3 *p;
    int prevAng;
    Vector3 *prev;
    int inv;
    int push0;
    int z0;
    int pop1;
    Vector3 tmp;
    int mtx[9];
    Vector3 *pos;

    z = (int)half << 16;
    z >>= 16;
    nh = -z;
    vy_pos = (u32)(u16)z << 16;
    vy_neg = (u32)(u16)nh << 16;
    ang = 0;

    if (count < 2) {
        if (count == 1) {
            int n;
            int nshl;
            int tz, ty, tx;
            *(vu32 *)0x4000500 = 3;
            *(vu32 *)0x4000444 = ang;
            n = (s16)nh;
            nshl = n << 16;
            tz = pts->z;
            ty = pts->y;
            tx = pts->x;
            *(vu32 *)0x4000470 = tx;
            *(vu32 *)0x4000470 = ty;
            *(vu32 *)0x4000470 = tz;
            *(vu32 *)0x400048C = vy_pos | (unsigned short)n;
            *(vu32 *)0x400048C = ang;
            *(vu32 *)0x4000494 = (unsigned short)n | ((unsigned)(unsigned short)n << 16);
            *(vu32 *)0x400048C = (u16)z | vy_pos;
            *(vu32 *)0x400048C = ang;
            *(vu32 *)0x4000494 = (u16)z | ((unsigned)(unsigned short)n << 16);
            *(vu32 *)0x4000448 = 1;
            *(vu32 *)0x4000504 = ang;
            return;
        }
    }

    *(vu32 *)0x4000500 = 3;
    i = 0;
    if (count > 0) {
        inv = 0x1000 - t;
        p = pts;
        prev = pts;
        z0 = i;
        pop1 = 1;
        push0 = i;
        do {
            prevAng = ang;
            *(vu32 *)0x4000444 = push0;
            pos = p;
            if (i < count - 1) {
                int nx = pts[i + 1].x;
                int ny = pts[i + 1].y;
                ang = func_020538b8(ny - prev->y, nx - prev->x);
                if (i == 0) {
                    prevAng = ang;
                }
            } else if (t != 0x1000) {
                int vx = p->x;
                int qi = i - 1;
                Vector3 *q = &pts[qi];
                int px = q->x;
                if (t < 0x1000) {
                    if (t <= 0) {
                        vx = px;
                    } else {
                        vx = (int)(((s64)inv * px + (s64)t * vx) >> 12);
                    }
                }
                tmp.x = vx;
                {
                    int vy = p->y;
                    int py = q->y;
                    if (t < 0x1000) {
                        if (t <= 0) {
                            vy = py;
                        } else {
                            vy = (int)(((s64)inv * py + (s64)t * vy) >> 12);
                        }
                    }
                    tmp.y = vy;
                }
                {
                    int qz = q->z;
                    int vz = p->z;
                    if (t < 0x1000) {
                        if (t <= 0) {
                            vz = qz;
                        } else {
                            vz = (int)(((s64)inv * qz + (s64)t * vz) >> 12);
                        }
                    }
                    tmp.z = vz;
                }
                pos = &tmp;
            }
            {
                s16 halfd = (s16)(ang - prevAng) / 2;
                int idx = (unsigned short)((s16)prevAng + halfd) >> 4;
                Matrix3x3_SetRotationZ(mtx, data_02082214[idx * 2], data_02082214[idx * 2 + 1]);
                func_0205532c(mtx, (int *)pos);
            }
            prev++;
            i++;
            *(vu32 *)0x400048C = vy_pos;
            p++;
            *(vu32 *)0x400048C = z0;
            *(vu32 *)0x4000494 = vy_neg;
            *(vu32 *)0x4000448 = pop1;
        } while (i < count);
    }
    *(vu32 *)0x4000504 = 0;
}

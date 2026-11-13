//cpp
#include "common.h"
#include "nitro/fx/fx_trig.h"
#include "nitro/gx/g3imm_cmd.h"

/* The VTX_XY geometry port: registers.h names the other vertex ports but not
 * this one, and g3imm_cmd.h has no G3_VtxXY. */
#define REG_VTX_XY (*(volatile u32 *)0x04000494)

extern "C" void MTX_RotZ33_(int *m, int s, int c);
extern "C" void G3_MultTransMtx33(int *a, int *v);

/* Draws a ribbon `half` units either side of the point path `pts` as one
 * quad strip: at each point it pushes a matrix, turns to the mean of the
 * incoming and outgoing headings and translates there, then emits the pair
 * (0, half) and (0, -half). The last point is placed `t` (Q12) of the way
 * from the point before it, 0x1000 being the point itself. A one-point path
 * draws a single square at that point. */
void func_ov007_020c19cc(int count, Vector3 *pts, int half, int t) {
    int z;
    int nh;
    u32 upperXY;
    u32 lowerXY;
    int i;
    int ang;
    Vector3 *p;
    int prevAng;
    Vector3 *prev;
    int inv;
    Vector3 tmp;
    int mtx[9];
    Vector3 *pos;

    z = (s16)half;
    nh = -z;
    upperXY = GX_FX16PAIR(0, z);
    lowerXY = GX_FX16PAIR(0, nh);
    ang = 0;

    if (count < 2) {
        if (count == 1) {
            int n;
            G3_Begin(GX_BEGIN_QUAD_STRIP);
            G3_PushMtx();
            n = (s16)nh;
            G3_Translate(pts->x, pts->y, pts->z);
            REG_VTX_16 = upperXY | (u16)n;
            REG_VTX_16 = 0;
            REG_VTX_XY = GX_FX16PAIR(n, n);
            REG_VTX_16 = (u16)z | upperXY;
            REG_VTX_16 = 0;
            REG_VTX_XY = GX_FX16PAIR(z, n);
            G3_PopMtx(1);
            G3_End();
            return;
        }
    }

    G3_Begin(GX_BEGIN_QUAD_STRIP);
    i = 0;
    if (count > 0) {
        inv = 0x1000 - t;
        p = pts;
        prev = pts;
        do {
            prevAng = ang;
            G3_PushMtx();
            pos = p;
            if (i < count - 1) {
                int nx = pts[i + 1].x;
                int ny = pts[i + 1].y;
                ang = FX_Atan2(ny - prev->y, nx - prev->x);
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
                MTX_RotZ33_(mtx, FX_SinIdx(idx), FX_CosIdx(idx));
                G3_MultTransMtx33(mtx, (int *)pos);
            }
            prev++;
            i++;
            REG_VTX_16 = upperXY;
            p++;
            REG_VTX_16 = 0;
            REG_VTX_XY = lowerXY;
            G3_PopMtx(1);
        } while (i < count);
    }
    G3_End();
}

//cpp
// @symbol func_ov006_020e1dc8
/* recovered: dScMgCurling_c stone separation, ov006 0x020e1dc8 (756 bytes).
 * Stone idx has just moved; any other active stone within 24 units of it is
 * pushed to 26 units away along the line between them (FX_Mul of the angle's
 * cos/sin by 0x1a, then back to integer units), and that pushed stone is in
 * turn checked once against the rest: the first stone it overlaps is pushed
 * the same way, the bump sound plays, and the routine returns. Only one push
 * per call in each direction; the outer scan stops after its first hit.
 *
 * What the match hinged on: the two FX_Mul results must be named (vx, vy) --
 * inlined into the stores they cost a reloaded 0x4660 pool constant and four
 * bytes -- and the inner scan must have its OWN dx/dy/dist/ang, declared in
 * the loop body. Sharing the outer loop's four variables recolours the outer
 * head (dy/dx/i-pointer rotate through r7/r8/sb). No pragma. */
#include "dScMgCurling_c.h"

extern "C" {
extern int _ZN4cstd4sqrtEy(u64 v);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern void func_02012718(int id, int v);
extern s16 data_02082214[];
}

extern "C" void func_ov006_020e1dc8(dScMgCurling_c *self, int idx)
{
    int i;
    int j;
    int dx;
    int dy;
    int dist;
    u16 ang;
    int k;

    for (i = 0; i < 5; i++) {
        if (self->mStone[i].active == 0) continue;
        if (idx == i) continue;
        dx = (self->mStone[i].x - self->mStone[idx].x) >> 12;
        dy = (self->mStone[i].y - self->mStone[idx].y) >> 12;
        dist = _ZN4cstd4sqrtEy((u64)(dx * dx + dy * dy));
        ang = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx);
        if (dist > 0x18) continue;
        {
            int cs;
            int sn;
            int vx;
            int vy;

            k = (ang >> 4) * 2;
            cs = data_02082214[k + 1];
            vx = (int)(((long long)cs * 0x1a + 0x800) >> 12);
            self->mStone[i].x = self->mStone[idx].x + (vx << 12);
            sn = data_02082214[k];
            vy = (int)(((long long)sn * 0x1a + 0x800) >> 12);
            self->mStone[i].y = self->mStone[idx].y + (vy << 12);
            for (j = 0; j < 5; j++) {
                int dx2;
                int dy2;
                int dist2;
                u16 ang2;

                if (self->mStone[j].active == 0) continue;
                if (i == j) continue;
                dx2 = (self->mStone[j].x - self->mStone[i].x) >> 12;
                dy2 = (self->mStone[j].y - self->mStone[i].y) >> 12;
                dist2 = _ZN4cstd4sqrtEy((u64)(dx2 * dx2 + dy2 * dy2));
                ang2 = _ZN4cstd5atan2E5Fix12IiES1_(dy2, dx2);
                if (dist2 > 0x18) continue;
                {
                    int cs2;
                    int sn2;
                    int vx2;
                    int vy2;

                    k = (ang2 >> 4) * 2;
                    cs2 = data_02082214[k + 1];
                    vx2 = (int)(((long long)cs2 * 0x1a + 0x800) >> 12);
                    self->mStone[j].x = self->mStone[i].x + (vx2 << 12);
                    sn2 = data_02082214[k];
                    vy2 = (int)(((long long)sn2 * 0x1a + 0x800) >> 12);
                    self->mStone[j].y = self->mStone[i].y + (vy2 << 12);
                    func_02012718(0xe8, self->mStone[idx].x);
                    return;
                }
            }
            return;
        }
    }
}

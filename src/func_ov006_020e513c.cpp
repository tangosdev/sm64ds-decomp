//cpp
// @symbol func_ov006_020e513c
/* recovered: dScMgCurling2_c stone separation, ov006 0x020e513c (788 bytes).
 * The two-player rink's twin of func_ov006_020e1dc8: eleven stones at a
 * 0x30 stride instead of five at 0x2c, and after the bump sound the pushed
 * pair is handed to func_ov006_020e39e0. Everything else, including the
 * two levers that closed the Curling version (named vx/vy, the inner scan's
 * own dx2/dy2/dist2/ang2 declared in the loop body), carries over verbatim.
 * The extra live idx/i for the trailing call is what moves k out of fp and
 * spills the 0x800 constant; the compiler does that on its own. */
#include "dScMgCurling2_c.h"

extern "C" {
extern int _ZN4cstd4sqrtEy(u64 v);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern void func_02012718(int id, int v);
extern void func_ov006_020e39e0(dScMgCurling2_c *self, int a, int b);
extern s16 data_02082214[];
}

extern "C" void func_ov006_020e513c(dScMgCurling2_c *self, int idx)
{
    int i;
    int j;
    int dx;
    int dy;
    int dist;
    u16 ang;
    int k;

    for (i = 0; i < 11; i++) {
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
            for (j = 0; j < 11; j++) {
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
                    func_ov006_020e39e0(self, idx, i);
                    return;
                }
            }
            return;
        }
    }
}

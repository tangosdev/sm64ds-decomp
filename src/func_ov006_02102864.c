extern unsigned func_02054de8(void);
extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern void func_ov006_021027e4(char *c, int x, int y, int layer);

#define LA(p) (*(int *)((((int)(p)) + 0x5664) & 0xFFFFFFFFFFFFFFFF))
#define LB(p) (*(int *)((((unsigned int)(p)) + 0x5664) & 0xFFFFFFFFFFFFFFFF))
#define LC(p) (*(int *)(((int)((p) + 0x5664)) & 0xFFFFFFFFFFFFFFFF))
#define LD(p) (*(int *)(((unsigned int)((p) + 0x5664)) & 0xFFFFFFFFFFFFFFFF))
#define LE(p) (*(int *)(((long long)((int)(p) + 0x5664)) & 0xFFFFFFFFFFFFFFFFLL))
#define LF(p) (*(int *)(((long long)((unsigned int)(p) + 0x5664)) & 0xFFFFFFFFFFFFFFFFLL))
#define LG(p) (*(int *)(((unsigned long long)((int)(p) + 0x5664)) & 0xFFFFFFFFFFFFFFFFLL))
#define LH(p) (*(int *)(((unsigned long long)((unsigned int)(p) + 0x5664)) & 0xFFFFFFFFFFFFFFFFLL))

#pragma opt_loop_invariants off
#pragma opt_dead_assignments off

void func_ov006_02102864(char *c)
{
    int k;                       /* sp+0x00 */
    int tx;                      /* sp+0x04 */
    int ty;                      /* sp+0x08 */
    int *p1;                     /* sp+0x0c */
    int *p2;                     /* sp+0x10 */
    int idx;                     /* sp+0x14 */
    int v2[2];                   /* sp+0x18..0x1c */
    int x1;                      /* sp+0x20 */
    volatile unsigned short tmp; /* sp+0x24 */
    int cx;                      /* sb */
    int cy;                      /* r8 */
    int dx;                      /* r7 */
    int dy;                      /* fp */
    int adx;                     /* r6 */
    int ady;                     /* r5 */
    int layer;                   /* r4 */
    int n;

    n = *(unsigned char *)(c + 0x5676);
    if (n == 0) return;
    idx = n - 1;
    if (*(int *)(c + idx * 0x40 + 0x4000 + 0x660) != *(int *)(c + idx * 0x40 + 0x4000 + 0x678)
     || *(int *)(c + idx * 0x40 + 0x4000 + 0x664) != *(int *)(c + idx * 0x40 + 0x4000 + 0x67c)) {
        char *dst = (char *)func_02054de8();
        int i2;
        tmp = 0;
        MultiStore16(tmp, dst, 0x6000);

        i2 = (int)((long long)idx & 0xFFFFFFFFFFFFFFFFLL);
        p1 = (int *)(c + i2 * 0x40 + 0x4660);
        p2 = (int *)(c + i2 * 0x40 + 0x4664);
        k = 0;
        v2[0] = 0x6c;
        v2[1] = 0x22;
        layer = 2;
        x1 = 0x94;
        do {
            if (k == 0) {
                int v;
                cx = v2[0];
                v = *p2 >> 12;
                tx = *p1 >> 12;
                if (v >= 0x22) ty = v + 4; else ty = v - 4;
                cy = v2[1];
                tx = tx - 4;
            } else {
                int v;
                cx = x1;
                v = *p2 >> 12;
                tx = *p1 >> 12;
                if (v >= 0x22) ty = v + 4; else ty = v - 4;
                cy = v2[1];
                tx = tx + 4;
            }
            dx = tx - cx;
            adx = dx;
            if (dx < 0) adx = -dx;
            dy = ty - cy;
            ady = dy;
            if (dy < 0) ady = -dy;

            if (adx >= ady) {
                *(int *)(c + 0x5664) = adx / 2;
            LX:
                if (dx == 0) {
                    func_ov006_021027e4(c, cx, cy, layer);
                } else if (dx > 0) {
                    cx++;
                    LA(c) += ady;
                    if (*(int *)(c + 0x5664) > adx) {
                        if (dy >= 0) cy++; else cy--;
                        LB(c) -= adx;
                    }
                    func_ov006_021027e4(c, cx, cy, layer);
                    if (cx == tx) goto DONEK;
                    goto LX;
                } else {
                    cx--;
                    LC(c) += ady;
                    if (*(int *)(c + 0x5664) > adx) {
                        if (dy >= 0) cy++; else cy--;
                        LD(c) -= adx;
                    }
                    func_ov006_021027e4(c, cx, cy, layer);
                    if (cx == tx) goto DONEK;
                    goto LX;
                }
            } else {
                *(int *)(c + 0x5664) = ady / 2;
            LY:
                if (dy == 0) {
                    func_ov006_021027e4(c, cx, cy, layer);
                } else if (dy > 0) {
                    cy++;
                    LE(c) += adx;
                    if (*(int *)(c + 0x5664) > ady) {
                        if (dx >= 0) cx++; else cx--;
                        LF(c) -= ady;
                    }
                    func_ov006_021027e4(c, cx, cy, layer);
                    if (cy == ty) goto DONEK;
                    goto LY;
                } else {
                    cy--;
                    LG(c) += adx;
                    if (*(int *)(c + 0x5664) > ady) {
                        if (dx >= 0) cx++; else cx--;
                        LH(c) -= ady;
                    }
                    func_ov006_021027e4(c, cx, cy, layer);
                    if (cy != ty) goto LY;
                }
            }
        DONEK:
            k++;
        } while (k < 2);
    }
    {
        int i3 = (int)((unsigned long long)idx & 0xFFFFFFFFFFFFFFFFLL);
        *(int *)(c + i3 * 0x40 + 0x4000 + 0x678) = *(int *)(c + i3 * 0x40 + 0x4000 + 0x660);
        *(int *)(c + i3 * 0x40 + 0x4000 + 0x67c) = *(int *)(c + i3 * 0x40 + 0x4000 + 0x664);
    }
}

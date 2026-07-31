typedef unsigned char u8;
typedef unsigned short u16;

extern char *_ZN3G2S13GetBG2CharPtrEv(void);
extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern void func_ov006_020fbcb8(void *self, int x, int y, int k);

#pragma opt_common_subs off
#pragma opt_loop_invariants off
#define AC (*(int *)(((long long)((int)a + 0x5c14))))

void func_ov006_020fbd38(void *arg0)
{
    char *a = (char *)arg0;
    int i;
    int tx;
    int ty;
    int *sx;
    int *sy;
    int *pf00;
    int *pf18;
    int idx;
    int sX0;
    int sX1;
    int sY;
    volatile u16 fill;
    int off;
    int py;
    int cx;
    int cy;
    int dx;
    int adx;
    int dy;
    int ady;
    char *buf;
    int n;

    n = *(u8 *)(a + 0x5c2f);
    if (n == 0)
        return;
    idx = n - 1;
    off = idx * 0x38;
    pf18 = (int *)(a + 0x4ef0 + off);
    pf00 = (int *)(a + 0x4ed8 + off);
    if (*pf00 != *pf18 || *(int *)(a + 0x4edc + off) != *(int *)(a + 0x4ef4 + off)) {
        buf = _ZN3G2S13GetBG2CharPtrEv();
        fill = 0;
        MultiStore16(fill, buf, 0x6000);
        sx = (int *)(a + off + 0x4ed8);
        sy = (int *)(a + off + 0x4edc);
        sX0 = 0x6c;
        sX1 = 0x94;
        sY = 0x22;
        for (i = 0; i < 2; i++) {
            if (i == 0) {
                cx = sX0;
                tx = *sx >> 12;
                cy = sY;
                py = *sy >> 12;
                if (py >= 0x22) ty = py + 4; else ty = py - 4;
                tx -= 4;
            } else {
                cx = sX1;
                tx = *sx >> 12;
                cy = sY;
                py = *sy >> 12;
                if (py >= 0x22) ty = py + 4; else ty = py - 4;
                tx += 4;
            }
            dx = tx - cx;
            adx = dx;
            if (dx < 0) adx = -dx;
            dy = ty - cy;
            ady = dy;
            if (dy < 0) ady = -dy;
            if (adx >= ady) {
                *(int *)(a + 0x5c14) = adx / 2;
                for (;;) {
                    if (dx == 0) {
                        func_ov006_020fbcb8(a, cx, cy, 2);
                        break;
                    } else if (dx > 0) {
                        cx++;
                        AC += ady;
                        if (*(int *)(a + 0x5c14) > adx) {
                            if (dy >= 0) cy++; else cy--;
                            AC -= adx;
                        }
                        func_ov006_020fbcb8(a, cx, cy, 2);
                        if (cx == tx)
                            break;
                    } else {
                        cx--;
                        AC += ady;
                        if (*(int *)(a + 0x5c14) > adx) {
                            if (dy >= 0) cy++; else cy--;
                            AC -= adx;
                        }
                        func_ov006_020fbcb8(a, cx, cy, 2);
                        if (cx == tx)
                            break;
                    }
                }
            } else {
                *(int *)(a + 0x5c14) = ady / 2;
                for (;;) {
                    if (dy == 0) {
                        func_ov006_020fbcb8(a, cx, cy, 2);
                        break;
                    } else if (dy > 0) {
                        cy++;
                        AC += adx;
                        if (*(int *)(a + 0x5c14) > ady) {
                            if (dx >= 0) cx++; else cx--;
                            AC -= ady;
                        }
                        func_ov006_020fbcb8(a, cx, cy, 2);
                        if (cy == ty)
                            break;
                    } else {
                        cy--;
                        AC += adx;
                        if (*(int *)(a + 0x5c14) > ady) {
                            if (dx >= 0) cx++; else cx--;
                            AC -= ady;
                        }
                        func_ov006_020fbcb8(a, cx, cy, 2);
                        if (cy == ty)
                            break;
                    }
                }
            }
        }
    }
    *pf18 = *pf00;
    *(int *)(a + 0x4ef4 + idx * 0x38) = *(int *)(a + 0x4edc + idx * 0x38);
}

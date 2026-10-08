typedef short s16;

void func_ov007_020cb4b0(s16* p, s16* nrm, int w1, int h1, int x, int y)
{
    int even = (y & 1) == 0;
    int k = y * w1 + x;
    s16 (*t)[3] = (s16 (*)[3])nrm;
    s16* cell = t[2 * k];
    s16 (*tc)[3] = (s16 (*)[3])cell;

    p[2] = 0;
    p[1] = p[2];
    p[0] = p[1];

    if (x > 0) {
        if (y > 0) {
            s16* a = tc[(-1 - w1) * 2];
            if (even) {
                p[0] += a[0] + a[3];
                p[1] += a[1] + a[4];
                p[2] += a[2] + a[5];
            } else {
                p[0] += a[3];
                p[1] += a[4];
                p[2] += a[5];
            }
        }
        if (y < h1) {
            s16* b = tc[-2];
            if (even) {
                p[0] += b[0] + b[3];
                p[1] += b[1] + b[4];
                p[2] += b[2] + b[5];
            } else {
                p[0] += b[3];
                p[1] += b[4];
                p[2] += b[5];
            }
        }
    }

    if (x < w1) {
        if (y > 0) {
            s16* c = tc[(0 - w1) * 2];
            if (!even) {
                p[0] += c[3] + c[0];
                p[1] += c[4] + c[1];
                p[2] += c[5] + c[2];
            } else {
                p[0] += c[0];
                p[1] += c[1];
                p[2] += c[2];
            }
        }
        if (y < h1) {
            if (!even) {
                p[0] += cell[3] + cell[0];
                p[1] += cell[4] + cell[1];
                p[2] += cell[5] + cell[2];
            } else {
                p[0] += cell[0];
                p[1] += cell[1];
                p[2] += cell[2];
            }
        }
    }
}

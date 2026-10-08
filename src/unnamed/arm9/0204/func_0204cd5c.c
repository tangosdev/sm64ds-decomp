typedef short s16;
typedef long long s64;

void func_0204cd5c(int *out, int *in, int sel, s16 *m)
{
    switch (sel) {
    case 2:
        out[0] = 0;
        out[1] = in[0];
        out[2] = in[1];
        break;
    case 1:
        out[0] = in[1];
        out[1] = 0;
        out[2] = in[0];
        break;
    case 0:
        out[0] = in[0];
        out[1] = in[1];
        out[2] = 0;
        break;
    case 3:
        out[0] = (int)(((s64)in[0] * m[0x2e] + 0x800) >> 12) + (int)(((s64)in[1] * m[0x31] + 0x800) >> 12);
        out[1] = (int)(((s64)in[0] * m[0x2f] + 0x800) >> 12) + (int)(((s64)in[1] * m[0x32] + 0x800) >> 12);
        out[2] = (int)(((s64)in[0] * m[0x30] + 0x800) >> 12) + (int)(((s64)in[1] * m[0x33] + 0x800) >> 12);
        break;
    }
}

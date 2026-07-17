typedef unsigned char u8;
typedef long long s64;

#pragma opt_strength_reduction off

int func_ov006_02125bbc(char *o, int *p)
{
    int i;
    for (i = 0; i < 0x80; i++) {
        if (*(u8 *)(o + i + 0xac58) == 1) {
            char *e = o + i * 8;
            int dx, dy;
            if (p[0] >= *(int *)(e + 0xacd8) + 0x10000)
                continue;
            if (p[0] < *(int *)(e + 0xacd8) - 0x10000)
                continue;
            if (p[1] >= *(int *)(e + 0xacdc) + 0x8000)
                continue;
            if (p[1] < *(int *)(e + 0xacdc) - 0x8000)
                continue;
            dy = *(int *)(e + 0xacdc) - p[1];
            dx = *(int *)(e + 0xacd8) - p[0];
            if ((int)(((s64)dx * dx + 0x800) >> 12) + (int)(((s64)dy * dy + 0x800) >> 12) / 4 > 0x100000)
                continue;
            *(u8 *)(o + i + 0xb2d8) = 1;
            if (*(int *)(o + i * 4 + 0xb0d8) == 1) {
                *(u8 *)(o + 0xb9e6) = 1;
                *(int *)(o + 0xb9e8) = *(int *)(e + 0xacd8);
                *(int *)(o + 0xb9ec) = *(int *)(e + 0xacdc);
            }
            return 1;
        }
    }
    return 0;
}

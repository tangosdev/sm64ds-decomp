extern void func_ov004_020afdd0(int tex, int x, int y, int a, int b);
extern int data_ov006_02136e0c[];

void func_ov006_020fae20(char *base)
{
    int i;
    char *o = base;
    for (i = 0; i < 4; i++) {
        if (*(unsigned char *)(o + 0x5000 + 0xbd5) != 0) {
            func_ov004_020afdd0(
                data_ov006_02136e0c[*(unsigned char *)(o + 0x5000 + 0xbd6)],
                *(int *)(o + 0x5000 + 0xbcc) >> 0xc,
                *(int *)(o + 0x5000 + 0xbd0) >> 0xc,
                -1, 1);
        }
        o += 0xc;
    }
}

extern void func_ov004_020afdd0(int tex, int x, int y, int a, int b);
extern int data_ov006_02136b80[];

void func_ov006_020fc144(char *base)
{
    int i;
    char *o = base;
    for (i = 0; i < 2; i++) {
        if (*(unsigned char *)(o + 0x4000 + 0xeb8) != 0) {
            func_ov004_020afdd0(
                data_ov006_02136b80[*(unsigned char *)(o + 0x4000 + 0xeb5)],
                *(int *)(o + 0x4000 + 0xea0) >> 0xc,
                *(int *)(o + 0x4000 + 0xea4) >> 0xc,
                -1, 2);
        }
        o += 0x1c;
    }
}

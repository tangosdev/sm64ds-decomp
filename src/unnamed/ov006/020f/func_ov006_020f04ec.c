extern char *func_ov004_020afb20(int a0, int a1, int a2, int a3, int a4, int a5, unsigned short a6);
extern char *data_ov006_021350d8;

void func_ov006_020f04ec(char *base)
{
    int j;
    for (j = 0; j < 2; j++) {
        if (*(unsigned char *)(base + 0x47b5) != 0) {
            int x = *(int *)(base + 0x47a0) >> 0xc;
            int y = *(int *)(base + 0x47a4) >> 0xc;
            char *e = data_ov006_021350d8;
            for (;;) {
                char *r = func_ov004_020afb20((int)e, x, y, -1, 1, 0x1000, 0);
                if (r != 0) {
                    int v = (int)((unsigned int)(*(int *)(r + 4) << 0x10) >> 0x1c);
                    *(int *)r = (*(int *)r & ~0xc00) | 0x800;
                    *(unsigned short *)(r + 4) = (unsigned short)((*(unsigned short *)(r + 4) & ~0xf000) | (v << 12));
                }
                if (*(unsigned short *)(e + 6) == 0xffff) break;
                e += 8;
            }
        }
        base += 0x18;
    }
}

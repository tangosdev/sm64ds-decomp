extern int Sound_PlayIfNotActive(int a, int b, int c, int d);

void func_ov006_020f0a6c(char *c, int i)
{
    int n = i * 0x18;
    int *pf8 = (int *)(c + 0x47a8 + n);
    int *pf0 = (int *)(c + 0x47a0 + n);
    int v;
    *pf0 = *pf0 + *pf8;
    v = *pf0 >> 12;
    if (i == 0) {
        *pf8 -= 0x100;
        if (v >= 0x140) {
            *pf0 = 0x140000;
            ((unsigned char *)(c + 0x47b7))[n]++;
            *(short *)(c + n + 0x47b0) = 0x10;
            *pf8 = -0x6000;
        }
    } else {
        *pf8 += 0x100;
        if (v <= -0x40) {
            *pf0 = -0x40000;
            ((unsigned char *)(c + 0x47b7))[n]++;
            *(short *)(c + n + 0x47b0) = 0x10;
            *pf8 = 0x6000;
        }
    }
    if (i == 0) {
        *(int *)(c + 0x47ac + n) = Sound_PlayIfNotActive(*(int *)(c + 0x47ac + n), 2, 0x1b8, 0);
    }
}

typedef short s16;

extern void _ZN5Sound12PlayBank2_2DEj(unsigned int a);
extern int Sound_PlayIfNotActive(int a, int b, int c, int d);

void func_ov006_020f06fc(char *c, int i)
{
    int n = i * 0x18;
    int *pf8 = (int *)(c + 0x47a8 + n);
    int *pf0 = (int *)(c + 0x47a0 + n);
    int v;

    *pf0 = *pf0 + *pf8;
    v = *pf0 >> 12;

    if (i == 0) {
        *pf8 += 0x140;
        if (v >= 0x80 && *pf8 > 0) {
            *pf0 = 0x80000;
            *(unsigned char *)(c + n + 0x47b4) = 0;
            *(unsigned char *)(c + n + 0x47b7) = 0;
            if (i == 0)
                _ZN5Sound12PlayBank2_2DEj(0x1b9);
            return;
        }
        if (i != 0)
            return;
        *(int *)(c + 0x47ac + n) = Sound_PlayIfNotActive(*(int *)(c + 0x47ac + n), 2, 0x1b8, 0);
    } else {
        *pf8 -= 0x140;
        if (v <= 0x80 && *pf8 < 0) {
            *pf0 = 0x80000;
            *(unsigned char *)(c + n + 0x47b4) = 0;
            *(unsigned char *)(c + n + 0x47b7) = 0;
            if (i != 0)
                return;
            _ZN5Sound12PlayBank2_2DEj(0x1b9);
            return;
        }
        if (i != 0)
            return;
        *(int *)(c + 0x47ac + n) = Sound_PlayIfNotActive(*(int *)(c + 0x47ac + n), 2, 0x1b8, 0);
    }
}

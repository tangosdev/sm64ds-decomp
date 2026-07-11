#define AT(p, off) ((void *)(int)(((long long)(int)((char *)(p) + (off))) & 0xFFFFFFFFFFFFFFFFLL))

void func_ov007_020c80c8(char *c, int m, int sh)
{
    if ((*(int *)(c + 0x20) & 1) == 0) {
        if (*(short *)(c + 0x1c) == 0x1000) {
            int *p = (int *)AT(c, 0xc);
            int prod = (int)(((long long)m * *(int *)(c + 0xc) + 0x800) >> 12);
            *p = *p + ((*(int *)(c + 0x18) - prod) >> sh);
        } else {
            int *q = (int *)AT(c, 0x18);
            int prod = (int)(((long long)m * *(int *)(c + 0xc) + 0x800) >> 12);
            int *p;
            int prod2;
            *q = *q - prod;
            p = (int *)AT(c, 0xc);
            prod2 = (int)(((long long)*(int *)(c + 0x18) * (*(short *)(c + 0x1c) >> sh) + 0x800) >> 12);
            *p = *p + prod2;
        }
        {
            int *t = (int *)AT(*(char **)c, 8);
            *t = *t + (*(int *)(c + 0xc) >> sh);
        }
    }
    *(int *)(c + 0x18) = 0;
}

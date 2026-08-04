void func_ov006_02100278(char *c, int r1, int r2, int r3)
{
    int i;
    char *p = c;
    for (i = 0; i < 0x10; i++, p += 0x18) {
        if (*(unsigned char *)(p + 0x54b4) == 0) {
            int off = i * 0x18;
            char *q = (char *)(((int)c + off) & 0xFFFFFFFFFFFFFFFFull);
            *(unsigned char *)(q + 0x54b4) = 1;
            *(unsigned char *)(q + 0x54b5) = 1;
            if (r1 >= 0x80000)
                *(int *)(q + 0x54a0) = 0xa4000;
            else
                *(int *)(q + 0x54a0) = 0x5c000;
            {
                char *w = (char *)(int)((long long)(int)(c + off) & 0xFFFFFFFFFFFFFFFFLL);
                *(int *)(w + 0x54a4) = r2 + 0x48000;
                *(int *)(w + 0x54ac) = 0;
                *(int *)(w + 0x54a8) = 0;
                *(unsigned char *)(w + 0x54b6) = (unsigned char)r3;
                *(unsigned char *)(w + 0x54b7) = 0;
                *(short *)(w + 0x54b0) = 0x40;
            }
            return;
        }
    }
}

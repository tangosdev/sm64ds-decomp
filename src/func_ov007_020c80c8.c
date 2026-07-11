void func_ov007_020c80c8(char *o, int a, int s)
{
    if (!(*(int *)(o + 0x20) & 1)) {
        if (*(short *)(o + 0x1c) == 0x1000) {
            int m = (int)(((long long)a * *(int *)(o + 0xc) + 0x800) >> 12);
            int *p = (int *)(int)(((long long)(int)(o + 0xc)) & 0xFFFFFFFFFFFFFFFFLL);
            *p += (*(int *)(o + 0x18) - m) >> s;
        } else {
            int m = (int)(((long long)a * *(int *)(o + 0xc) + 0x800) >> 12);
            int *p18 = (int *)(int)(((long long)(int)(o + 0x18)) & 0xFFFFFFFFFFFFFFFFLL);
            int m2;
            int *p0c;
            *p18 -= m;
            m2 = (int)(((long long)*(int *)(o + 0x18) * (*(short *)(o + 0x1c) >> s) + 0x800) >> 12);
            p0c = (int *)(int)(((long long)(int)((char *)o + 0xc)) & 0xFFFFFFFFFFFFFFFFLL);
            *p0c += m2;
        }
        {
            int *base = *(int **)o;
            int *q = (int *)(int)(((long long)(int)((char *)base + 8)) & 0xFFFFFFFFFFFFFFFFLL);
            *q += *(int *)(o + 0xc) >> s;
        }
    }
    *(int *)(o + 0x18) = 0;
}

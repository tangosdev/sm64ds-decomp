extern void MultiCopyHalf(void *dst, void *src, int n);

void func_0206853c(char *self, void *src, unsigned int flags, int s)
{
    int i;
    int cnt;
    cnt = 1;
    MultiCopyHalf(src, self + 0x35e, 0x14a);
    for (i = 0; i < 0xf; i++) {
        if (flags & (2 << i)) cnt = (cnt + 1) & 0xff;
    }
    *(unsigned char *)(self + 0x358) = (unsigned char)cnt;
    *(short *)(self + 0x35a) = (short)(flags | 1);
    *(short *)(self + 0x35c) = (short)s;
    *(unsigned char *)(((long long)(int)(self + 0x4ac)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
}

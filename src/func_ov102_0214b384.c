void func_ov102_0214b384(void *arg0, unsigned int arg1) {
    char *p = (char *)arg0;
    unsigned int cur = *(unsigned short *)(p + 0x3ea);
    if (cur == 0 || cur > arg1) {
        *(unsigned short *)(p + 0x3ea) = (unsigned short)arg1;
    }
    *(unsigned int *)(((long long)(int)(p + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) &= ~1u;
}

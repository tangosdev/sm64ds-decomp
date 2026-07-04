void func_ov006_02102ef4(unsigned char *r0) {
    if (r0[0x5678] != 0) return;
    *(unsigned short *)(r0 + 0x5674) = 0x1e;
    r0[0x5679] = 0;
    r0[0x567a] = 1;
    {
        unsigned char *p = (unsigned char *)(((long long)(int)(r0 + 0x5678)) & 0xFFFFFFFFFFFFFFFFLL);
        *p += 1;
    }
}

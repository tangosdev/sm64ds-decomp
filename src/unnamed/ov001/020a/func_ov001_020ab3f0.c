void func_ov001_020ab3f0(void *r0) {
    unsigned char *ptr = (unsigned char *)r0;
    if (ptr[0x10] != 0) {
        ptr[0x10] = 0;
    } else {
        ptr[0x10] = 1;
    }
    ptr[0x11] = 0;
    *(int *)(ptr + 0xc) = *(int *)(ptr + 0x18);
}

void func_ov060_02115018(void *c) {
    int r1 = *(int *)((char *)c + 0xa8);
    if (r1 >= 0) return;
    int thresh = *(int *)((char *)c + 0x3b4) - 0x12c000;
    int cur = *(int *)((char *)c + 0x60);
    if (cur >= thresh) return;
    *(int *)((char *)c + 0x64) = 0;
    *(int *)((char *)c + 0x5c) = *(int *)((char *)c + 0x64);
    *(int *)((char *)c + 0x60) = *(int *)((char *)c + 0x3b4) + 0x7d0000;
    *(int *)((char *)c + 0xa8) = 0;
    *(int *)((char *)c + 0x98) = 0;
}

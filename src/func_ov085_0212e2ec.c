extern int SAVE_DATA[];
int func_ov085_0212e2ec(void *r0) {
    int *p = SAVE_DATA;
    p[2] |= 0x20000;
    *(int*)((char*)r0+0x2d8) = 0;
    return 1;
}

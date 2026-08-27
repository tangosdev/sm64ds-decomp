void func_ov004_020b1b78(void* c, int val) {
    int r2 = *(int*)((char*)c + 0xa8);
    r2 -= val;
    if (r2 < 0) r2 = 0;
    else if (r2 > 0x270f) r2 = 0x270f;
    *(int*)((char*)c + 0xa8) = r2;
}

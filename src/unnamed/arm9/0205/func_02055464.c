int func_02055464(int *out) {
    if (*(volatile unsigned int *)0x4000600 & 0x4000)
        return -1;
    *out = (*(volatile unsigned int *)0x4000600 & 0x2000) >> 13;
    return 0;
}

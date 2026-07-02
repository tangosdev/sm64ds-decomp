extern signed char data_ov063_0211e26c[];
int func_ov063_0211c770(unsigned char* c, int idx) {
    if ((unsigned int)idx > 3 || (short)idx < 0)
        return 1;
    (*(int *)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFF)) += data_ov063_0211e26c[idx] << 12;
    return 0;
}

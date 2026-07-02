void func_ov060_021123a0(unsigned char* c, int f) {
    if (f)
        (*(int *)(((int)c + 0x378) & 0xFFFFFFFFFFFFFFFF)) &= ~1;
    else
        (*(int *)(((int)c + 0x378) & 0xFFFFFFFFFFFFFFFF)) |= 1;
}

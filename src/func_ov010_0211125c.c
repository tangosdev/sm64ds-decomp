void func_ov010_0211125c(unsigned char* c) {
    (*(short *)(((int)c + 0x90) & 0xFFFFFFFFFFFFFFFF)) += 0x400;
    if (*(short*)(c + 0x90) > 0) {
        *(short*)(c + 0x90) = 0;
        *(int*)(c + 0x3a0) = 0;
    }
}

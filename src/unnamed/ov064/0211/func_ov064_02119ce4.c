int func_ov064_02119ce4(char *c) {
    *(unsigned char*)(c + 0x380) = 0x1f;
    *(short*)(c + 0x100) = 0x64;
    int r2 = *(int*)(c + 0x37c);
    if ((unsigned)(r2 - 1) <= 1) {
        char *r1 = *(char**)(c + 0x38c);
        if (r1 != 0) {
            if (r2 == 1) {
                *(char**)(r1 + 0x3a8) = c;
            } else {
                *(char**)(r1 + 0x31c) = c;
            }
        }
    }
    return 1;
}

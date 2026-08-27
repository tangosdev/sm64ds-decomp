void func_ov007_020c10ec(char* c) {
    int i;
    *(int*)(c + 4) = 0;
    *(int*)c = *(int*)(c + 4);
    *(int*)(c + 8) = 1;
    *(int*)(c + 0xc) = 0;
    *(unsigned char*)(c + 0x14) = 0;
    *(int*)(c + 0x10) = 0;
    *(char*)(c + 0x15) = -1;
    *(unsigned char*)(c + 0x1c) = 0;
    *(unsigned char*)(c + 0x1d) = 0;
    *(int*)(c + 0x20) = 0;
    *(int*)(c + 0x24) = 0;
    *(int*)(c + 0x28) = 0;
    *(short*)(c + 0x2c) = 0;
    *(short*)(c + 0x2e) = 0;
    *(int*)(c + 0x30) = 0;
    *(int*)(c + 0x40) = 0;
    *(int*)(c + 0x3c) = *(int*)(c + 0x40);
    *(int*)(c + 0x38) = *(int*)(c + 0x3c);
    *(int*)(c + 0x34) = *(int*)(c + 0x38);
    for (i = 0; i < *(unsigned char*)(c + 0x1c); i++) {
        ((int*)*(char**)(c + 0x18))[i] = 0;
    }
}

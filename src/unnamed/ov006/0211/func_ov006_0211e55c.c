void func_ov006_0211e55c(char* c, int idx) {
    int i;
    char* slot = c;
    for (i = 0; i < 0x10; i++) {
        if (*(unsigned char*)(slot + 0x4000 + 0x8a9) == 0) {
            *(unsigned char*)(c + i * 0xc + 0x4000 + 0x8a9) = 1;
            *(unsigned char*)(c + i * 0xc + 0x4000 + 0x8aa) = 1;
            *(int*)(c + i * 0xc + 0x4000 + 0x8a0) = *(int*)(c + idx * 0x24 + 0x4000 + 0x660);
            *(int*)(c + i * 0xc + 0x4000 + 0x8a4) = *(int*)(c + idx * 0x24 + 0x4000 + 0x664) - 0x18000;
            *(unsigned char*)(c + i * 0xc + 0x4000 + 0x8a8) = 0x60;
            return;
        }
        slot += 0xc;
    }
}

void func_ov006_0211f51c(char *c) {
    int i;
    for (i = 0; i < 0x10; i++) {
        if (*(unsigned char*)(c + 0x4677) != 0 && *(unsigned char*)(c + 0x4678) == 1) {
            *(unsigned char*)(c + 0x4678) = 2;
        }
        c += 0x24;
    }
}

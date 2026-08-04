void func_ov006_020f0dd8(char *c) {
    int i;
    for (i = 0; i < 2; i++) {
        *(unsigned char*)(c + 0x47b4) = 1;
        *(unsigned char*)(c + 0x47b6) = 0;
        *(short*)(c + 0x47b0) = 0;
        *(short*)(c + 0x47b2) = 0;
        *(unsigned char*)(c + 0x47b5) = 0;
        *(unsigned char*)(c + 0x47b7) = 0;
        c += 0x18;
    }
}

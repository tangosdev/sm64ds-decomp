void func_ov006_020dec3c(char *p) {
    int i;
    for (i = 0; i < 0x20; i++) {
        *(unsigned char *)(p + 0x15) = 0;
        p += 0x18;
    }
}

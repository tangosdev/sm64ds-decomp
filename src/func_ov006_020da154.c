void func_ov006_020da154(char *p) {
    int i;
    for (i = 0; i < 5; i++) {
        *(unsigned char *)(p + 0x2b) = 9;
        p += 0x30;
    }
}

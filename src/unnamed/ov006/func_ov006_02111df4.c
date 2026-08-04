int func_ov006_02111df4(char* c) {
    if ((*(int*)(*(int*)(c + 4) + 8) & 0xff) != 0
        && *(int*)(c + 8) < 0xe0000
        && *(int*)(c + 0xc) >= 0x50000
        && *(int*)(c + 0xc) < 0x88000) {
        if (*(int*)(c + 0x11c) >= 0x78) return 1;
    }
    if (*(unsigned char*)(c + 0x121) == 0) return 1;
    return *(unsigned char*)(c + 0x3b);
}

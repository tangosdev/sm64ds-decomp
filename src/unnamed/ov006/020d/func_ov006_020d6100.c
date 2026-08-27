void func_ov006_020d6100(unsigned char* base, int idx) {
    unsigned char* p = base + (idx << 4);
    int o = idx << 4;
    if (*(unsigned char*)(p + 0x628f) != 0) {
        *(unsigned short*)(base + 0x6288 + o) =
            *(unsigned short*)(base + 0x6288 + o) + 1;
        if (*(unsigned short*)(p + 0x6288) < 4) return;
        *(unsigned short*)(p + 0x6288) = 0;
        *(unsigned char*)(base + 0x628f + o) =
            *(unsigned char*)(base + 0x628f + o) - 1;
    } else {
        *(unsigned char*)(p + 0x628c) = 0;
    }
}

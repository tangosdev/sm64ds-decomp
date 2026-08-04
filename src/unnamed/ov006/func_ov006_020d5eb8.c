void func_ov006_020d5eb8(unsigned char* base, int idx) {
    unsigned char* p = base + (idx << 4);
    int o = idx << 4;
    if (*(unsigned char*)(p + 0x62ae) != 0) {
        *(unsigned short*)(base + 0x62a8 + o) =
            *(unsigned short*)(base + 0x62a8 + o) + 1;
        if (*(unsigned short*)(p + 0x62a8) < 3) return;
        *(unsigned short*)(p + 0x62a8) = 0;
        *(unsigned char*)(base + 0x62ae + o) =
            *(unsigned char*)(base + 0x62ae + o) - 1;
    } else {
        *(unsigned char*)(p + 0x62ac) = 0;
    }
}

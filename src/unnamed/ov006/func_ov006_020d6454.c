void func_ov006_020d6454(unsigned char* base, int idx) {
    unsigned char* p = base + (idx << 4);
    if (*(unsigned char*)(p + 0x626f) != 0) {
        unsigned short* c = (unsigned short*)(base + 0x6268 + (idx << 4));
        *c = *c + 1;
        if (*(unsigned short*)(p + 0x6268) < 3) return;
        *(unsigned short*)(p + 0x6268) = 0;
        *(unsigned char*)(base + 0x626f + (idx << 4)) =
            *(unsigned char*)(base + 0x626f + (idx << 4)) - 1;
    } else {
        *(unsigned char*)(p + 0x626c) = 0;
    }
}

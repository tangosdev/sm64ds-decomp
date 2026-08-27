void func_ov006_020e9318(char* c, int i) {
    unsigned char st = *(unsigned char*)(c + 0x554e);
    if (st == 1) {
        unsigned char* p = (unsigned char*)(c + 0x521c + i * 0x18);
        if (*p == 1) *p = 0;
    }
    if (*(unsigned char*)(c + 0x554e) >= 0x10) {
        *(unsigned char*)(c + i * 0x18 + 0x5219) = 1;
    }
}

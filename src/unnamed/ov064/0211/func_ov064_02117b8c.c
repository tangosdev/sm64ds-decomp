extern int func_ov064_02117a44(void *c);

void func_ov064_02117b8c(void *c) {
    unsigned short v = *(unsigned short*)((char*)c + 0x338);
    if (v < 0x14) return;
    int r = func_ov064_02117a44(c);
    if (r != -1) return;
    unsigned char b = *(unsigned char*)((char*)c + 0x33c);
    if (b == 0) {
        *(unsigned char*)((char*)c + 0x33b) = 0;
    } else {
        *(unsigned char*)((char*)c + 0x33b) = 1;
    }
}

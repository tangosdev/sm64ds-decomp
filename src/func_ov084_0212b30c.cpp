//cpp
extern "C" int func_ov084_0212b30c(char *c) {
    unsigned short v = *(unsigned short*)(c + 0xc);
    int b = (v == (unsigned short)0xc8) ? 1 : 0;
    if (b) return 0x41000;
    int b2 = (v == (unsigned short)0xca) ? 1 : 0;
    if (b2) return 0x96000;
    return 0x14000;
}

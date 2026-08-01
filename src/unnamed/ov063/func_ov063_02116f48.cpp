//cpp
extern int data_ov066_0211ad00(char *c);
extern "C" int func_ov063_02116f48(char *c) {
    *(int*)(c+0x490) = 0;
    *(unsigned char*)(c+0x5c9) = 0xff;
    *(int*)(c+0x584) = 0x2000;
    *(int*)(c+0x80) = 0x2000;
    *(int*)(c+0x84) = 0x2000;
    *(int*)(c+0x88) = 0x2000;
    *(int*)(c+0x188) = *(int*)(c+0x590) * *(int*)(c+0x80);
    *(int*)(c+0x18c) = *(int*)(c+0x594) * *(int*)(c+0x80);
    int r = data_ov066_0211ad00(c);
    if (r) { *(unsigned char*)(c+0x5cc) = 1; r = 1; }
    return r;
}

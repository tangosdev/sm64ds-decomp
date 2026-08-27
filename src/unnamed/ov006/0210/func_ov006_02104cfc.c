void func_ov006_02104cfc(char *c, int i) {
    char *e = c + (i << 2);
    *(unsigned char*)(c + 0x4684) = 1;
    *(unsigned char*)(c + 0x4685) = 1;
    *(int*)(c + 0x4678) = *(int*)(e + 0x4cc4);
    *(int*)(c + 0x467c) = *(int*)(e + 0x4d54) + 0x8000;
    *(short*)(c + 0x4680) = 0x30;
    *(short*)(c + 0x4682) = 0;
    *(unsigned char*)(c + 0x4686) = 0;
}

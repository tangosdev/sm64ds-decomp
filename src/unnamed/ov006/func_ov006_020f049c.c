void func_ov006_020f049c(char *c, int i) {
    char *s = c + i*4;
    *(unsigned char*)(c + 0x47e0) = 1;
    *(int*)(c + 0x47d0) = *(int*)(s + 0x47f8);
    *(int*)(c + 0x47d4) = *(int*)(s + 0x49d8);
    *(unsigned char*)(c + 0x47df) = 16;
    *(unsigned char*)(c + 0x47e1) = 0;
    *(unsigned char*)(c + 0x47e3) = 1;
    *(unsigned char*)(c + 0x47e2) = 5;
    *(int*)(c + 0x47d8) = -3840;
}

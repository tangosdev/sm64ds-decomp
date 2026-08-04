extern int data_ov006_0212e2c0[];
void func_ov006_020d5d90(char *base, int idx) {
    char *ip = base + idx * 16;
    *(unsigned char*)(ip + 0x6000 + 0x2bc) = 1;
    *(unsigned char*)(ip + 0x6000 + 0x2bd) = 1;
    *(int*)(ip + 0x6000 + 0x2b0) = 0x80000;
    *(int*)(ip + 0x6000 + 0x2b4) = data_ov006_0212e2c0[idx] << 12;
    *(short*)(ip + 0x6200 + 0xb8) = 0;
    *(unsigned char*)(ip + 0x6000 + 0x2be) = 0;
}

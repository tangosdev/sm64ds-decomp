extern int data_ov006_0212e2c8[];
extern int data_ov006_0212e2e0[];
#pragma opt_strength_reduction off
void func_ov006_020d62e0(char *c) {
    int i;
    for (i = 0; i < 2; i++) {
        char *p = c + i * 0x10;
        *(unsigned char *)(p + 0x628d) = 1;
        *(unsigned char *)(p + 0x628c) = 0;
        *(unsigned char *)(p + 0x628e) = 1;
        *(int *)(p + 0x6280) = data_ov006_0212e2c8[i] << 0xc;
        *(int *)(p + 0x6284) = data_ov006_0212e2e0[i] << 0xc;
        *(unsigned char *)(p + 0x628f) = 0;
        *(short *)(p + 0x6288) = 0;
    }
}

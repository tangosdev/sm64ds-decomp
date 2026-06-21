//cpp
extern unsigned short data_ov002_021000d8[];

extern "C" void func_ov002_020f55b4(char *base, int idx) {
    char *e = base + idx * 0x4c;
    if (*(unsigned short *)(e + 0x3a) != 0x10) return;
    *(unsigned char *)(base + 0x47 + idx * 0x4c) = *(unsigned char *)(base + 0x47 + idx * 0x4c) + 1;
    *(int *)(e + 0x10) = 0x38000;
    *(unsigned short *)(e + 0x2e) = data_ov002_021000d8[idx];
    *(unsigned char *)(e + 0x48) = 0;
    *(unsigned short *)(e + 0x3c) = 0;
    *(int *)(e + 0x24) = 0;
    *(unsigned short *)(e + 0x42) = 0x200;
    *(int *)(e + 0x28) = 0x1000;
    *(unsigned short *)(e + 0x2c) = 0x200;
}

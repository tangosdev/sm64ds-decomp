extern int data_ov006_02137614;
void func_ov004_020aff38(int a, int b, int c, int d, int e, int f, int g);

void func_ov006_020fe1d0(char *this) {
    int i;
    for (i = 0; i < 0x30; i++, this += 0x38) {
        if (*(unsigned char*)(this + 0x4f0e) == 0) continue;
        func_ov004_020aff38(data_ov006_02137614,
                            *(int*)(this + 0x4ed8) >> 0xc,
                            *(int*)(this + 0x4edc) >> 0xc,
                            -1, -1, 0x1000, 0);
    }
}

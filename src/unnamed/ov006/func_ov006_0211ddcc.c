extern void func_ov004_020af948(int a, int b, int c, int d);
extern int data_ov006_02137cd8[];
void func_ov006_0211ddcc(char *c);
void func_ov006_0211ddcc(char *c){
    int i;
    for (i = 0; i < 0x10; i++) {
        if (*(unsigned char*)(c + 0x4a71)) {
            int b = (*(int*)(c + 0x4a64)) >> 12;
            int a = (*(int*)(c + 0x4a60)) >> 12;
            if (b <= 8) b = 8;
            if (b >= 0xb8) b = 0xb8;
            func_ov004_020af948(data_ov006_02137cd8[*(unsigned char*)(c + 0x4a72) + 1], a, b, 0);
        }
        c += 0x14;
    }
}

extern void func_ov004_020b0104(void* a0, int a1, int a2, int a3, int a4, void* a5);
extern int data_ov006_0213c264;
extern int data_ov006_0213c2ac;
extern int data_ov006_0212e468[];

void func_ov006_020e1c68(char* a0) {
    int x, y;
    int i;
    char* p;
    int n;
    int j;
    p = a0;
    for (i = 0; i < 5; i++) {
        if (*(unsigned char*)(p + 0x4689) == 0) goto next;
        if (*(unsigned char*)(p + 0x468a) == 0) goto next;
        x = *(int*)(p + 0x4660) >> 12;
        y = *(int*)(p + 0x4664) >> 12;
        func_ov004_020b0104(&data_ov006_0213c264, x, y, -1, 1, 0);
        func_ov004_020b0104(&data_ov006_0213c2ac, x, y + 8, -1, 2, 0);
    next:
        p += 0x2c;
    }
    n = 5 - *(unsigned char*)(a0 + 0x4ee6);
    if (n < 0) n = 0;
    j = 0;
    if (n > 0) {
        for (; j < n; j++) {
            int v = data_ov006_0212e468[j];
            func_ov004_020b0104(&data_ov006_0213c264, v, 0xb0, -1, 1, 0);
            func_ov004_020b0104(&data_ov006_0213c2ac, v, 0xb8, -1, 2, 0);
        }
    }
}

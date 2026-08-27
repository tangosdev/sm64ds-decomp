extern void func_ov004_020afdd0(void* a0, int a1, int a2, int a3, int a4);
extern int data_ov006_02133e7c[];
extern int data_ov006_02133e10[];

void func_ov006_020fb74c(void* base) {
    int i;
    char* p = (char*)base;
    for (i = 0; i < 30; i++, p += 0x14) {
        if (*(unsigned char*)(p + 0x5966)) {
            int type = *(unsigned char*)(p + 0x5967);
            int x = *(int*)(p + 0x5958) >> 12;
            int y = *(int*)(p + 0x595c) >> 12;
            int idx = *(unsigned char*)(p + 0x5965);
            if (type != 2) {
                func_ov004_020afdd0((void*)data_ov006_02133e10[idx], x, y, -1, -1);
            } else {
                func_ov004_020afdd0((void*)data_ov006_02133e7c[idx], x, y, -1, -1);
            }
        }
    }
}

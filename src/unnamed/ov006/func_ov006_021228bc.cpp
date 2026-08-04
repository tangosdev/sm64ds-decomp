//cpp
extern "C" {
struct Tbl { int a, b, c; };
extern int data_0209e650[];
extern struct Tbl data_ov006_0213fbf8;
extern unsigned short data_ov006_0212f144[];
extern int RandomIntInternal(int* seed);
extern void func_020169f4(char* self, int kind);
extern void func_02016a14(char* self, int arg);
extern void func_02016a04(char* self, int arg);

void func_ov006_021228bc(char* r0, int r1) {
    char* r5 = r0;
    int r4 = r1;
    struct Tbl t = data_ov006_0213fbf8;
    unsigned int rnd = ((unsigned int)RandomIntInternal(data_0209e650) & 0x7fffffff) >> 0x13;
    int idx;
    if ((int)rnd < t.a) {
        idx = 0;
    } else if ((int)rnd < t.b) {
        idx = 1;
    } else if ((int)rnd < t.c) {
        idx = 2;
    } else {
        idx = 3;
    }
    func_020169f4(r5, data_ov006_0212f144[idx]);
    int r1_4 = r4 * 0xA00;
    *(short*)(r5 + 0x76) = 0xF0;
    if (r1_4 > 0x4000) {
        r1_4 = 0x4000;
    }
    int r0_5 = r1_4 + 0x4000;
    *(int*)(r5 + 0x50) = r0_5;
    *(int*)(r5 + 0x54) = r0_5;
    *(int*)(r5 + 0x58) = r0_5;
    *(int*)(r5 + 0x5c) = 0x60000;
    *(int*)(r5 + 0x60) = 0x100000;
    *(int*)(r5 + 0x64) = 0x18000;
    int r1_9 = -0x1C00;
    *(int*)(r5 + 0x68) = r1_9;
    *(int*)(r5 + 0x6c) = r1_9;
    *(int*)(r5 + 0x70) = 0;
    unsigned int rnd2 = (unsigned int)RandomIntInternal(data_0209e650);
    int* p1 = (int*)(((unsigned long long)(unsigned int)(r5 + 0x5c)) & 0xffffffffffffffffULL);
    int v1 = *p1 + ((((rnd2 & 0x7fffffff) >> 0x13) - 0x800) * 0x40);
    *p1 = v1;
    int rnd3 = RandomIntInternal(data_0209e650);
    int* p2 = (int*)(((unsigned long long)(unsigned int)(r5 + 0x68)) & 0xffffffffffffffffULL);
    int adj3 = (int)(((unsigned int)rnd3 & 0x7fffffff) >> 0x13);
    *p2 -= adj3 >> 1;
    int rnd4 = RandomIntInternal(data_0209e650);
    int* p3 = (int*)(((unsigned long long)(unsigned int)(r5 + 0x6c)) & 0xffffffffffffffffULL);
    int adj4 = (int)(((unsigned int)rnd4 & 0x7fffffff) >> 0x13);
    *p3 -= adj4 >> 1;
    func_02016a14(r5, 0);
    func_02016a04(r5, 0);
}
}

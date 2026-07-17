typedef unsigned short u16;

extern void func_0203cd80(int* m, short angle);
extern void func_ov006_02120c08(void);
extern void func_ov006_020eef58(void);
extern int GetOwnerLanguage(void);
extern int func_ov004_020ad674(void);
extern void func_ov004_020afcf8(void* a0, void* a1, int a2, void* a3);
extern void func_ov004_020afa20(int a0, int a1, int a2, int a3, int a4);
extern void func_ov004_020b1a5c(int a0, int a1);
extern void func_ov006_020caadc(void);
extern void func_ov006_020d09e0(void);
extern void func_ov006_020ced84(void);
extern void func_ov006_02122a4c(void);

extern int data_ov006_0213b0f0;
extern int data_ov006_02134ecc;
extern int* data_ov006_0213fc48[];
extern int data_ov006_02140830;

int func_ov006_021231ac(char* self)
{
    int m[3];
    int count;
    int a1v;
    int i;

    m[0] = 0;
    m[1] = 0;
    m[2] = 0xfffff008;
    func_0203cd80(m, -0x4000);

    *(volatile int*)0x40004c8 =
        (((short)m[0] >> 3) & 0x3ff) |
        ((((short)m[1] >> 3) & 0x3ff) << 10) |
        ((((short)m[2] >> 3) & 0x3ff) << 20);
    *(volatile int*)0x40004cc = 0x7fff;
    *(volatile int*)0x40004cc = 0x40007fff;

    func_ov006_02120c08();
    func_ov006_020eef58();

    if (*(u16*)(self + 0x4664) == 1) {
        count = data_ov006_0213b0f0;
        a1v = 0x6e;
        if (GetOwnerLanguage() == 5 || GetOwnerLanguage() == 4)
            a1v -= 4;
        for (i = 0; i < 3; i++) {
            if (i >= count) {
                int idx = func_ov004_020ad674();
                func_ov004_020afcf8((void*)data_ov006_0213fc48[idx][1], (void*)a1v, 0xc, (void*)0);
            } else {
                func_ov004_020afa20(data_ov006_02134ecc, a1v, 0xc, -1, -1);
            }
            a1v += 0x12;
        }
    }

    func_ov004_020b1a5c(data_ov006_02140830, 6);

    if (*(short*)(self + 0x7ba8) == 0) {
        func_ov006_020caadc();
        func_ov006_020d09e0();
        func_ov006_020ced84();
    }
    func_ov006_02122a4c();
    return 1;
}

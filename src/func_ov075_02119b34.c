extern int func_0203da9c(void);
extern int func_02033464(unsigned char a, signed char b, int c);
extern void func_ov075_02115134(char* c);
extern int func_020124c4(int a, int b, int c, int d);
extern int func_ov001_020ab450(char* c, int a);
extern int func_ov075_02116d9c(int unused, int id);
extern void func_ov001_020ab3f0(void *r0);
extern void func_ov075_0211a148(char* c, int a, int b);
extern void func_02030a6c(void);
extern void func_02012790(int n);

extern unsigned char data_0209fc50;
extern unsigned char data_0209fc5c[];
extern int data_ov075_0211d8a0;
extern int data_ov075_0211d8d0;

void func_ov075_02119b34(char* c) {
    int r5 = func_0203da9c();
    int r4 = data_0209fc50;
    for (int i = 1; i < 4; i++) {
        func_02033464((unsigned char)i, (signed char)i, 0);
    }
    func_ov075_02115134(*(char**)(c + 0x50));
    *(int*)(c + 0x278) = func_020124c4(*(int*)(c + 0x278), 2, 0x11c, 0);
    if (data_0209fc5c[r5] == 0)
        return;
    if (r4 < 4) {
        if (func_ov001_020ab450(c + 0x70, 0) == 0) {
            if (func_ov075_02116d9c((int)c, 0) == 0)
                return;
        }
    }
    func_ov001_020ab3f0(c + 0x70);
    if (r4 <= 1) {
        func_ov075_0211a148(c, (int)&data_ov075_0211d8a0, 0x14);
    } else {
        func_02030a6c();
        func_ov075_0211a148(c, (int)&data_ov075_0211d8d0, 0x14);
    }
    func_02012790(0x117);
}

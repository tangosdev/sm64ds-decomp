struct V3 { int z, y, x; };
extern void func_020553a4(void* p);
extern void func_0203cd80(struct V3* out, int a, int b);
extern void func_ov006_020cf2fc(char* p);
extern char data_0209b3ec[];
extern char data_ov006_02140990[];
void func_ov006_020d09e0(void) {
    struct V3 s;
    *(volatile int*)0x4000440 = 2;
    func_020553a4(data_0209b3ec);
    s.z = 0;
    s.y = 0;
    s.x = 0xfffff008;
    func_0203cd80(&s, -0x2000, 0xfffff008);
    *(volatile int*)0x40004c8 = (((short)s.z >> 3) & 0x3ff)
        | ((((short)s.y >> 3) & 0x3ff) << 10)
        | ((((short)s.x >> 3) & 0x3ff) << 20)
        | 0x40000000;
    *(volatile int*)0x40004cc = 0x40007fff;
    int i;
    char* p = data_ov006_02140990;
    for (i = 0; i < 4; i++) {
        if (*(unsigned char*)(p + 0x328) != 0) {
            func_ov006_020cf2fc(p);
        }
        p += 0x32c;
    }
}

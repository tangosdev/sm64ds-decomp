//cpp
struct Obj {
    char pad0[0x4f38];
    int field_4f38;
    char pad1[0x5000 - 0x4f3c];
    int idx;
    char pad2[0x5018 - 0x5004];
    unsigned short h5018;
    unsigned short h501a;
    char pad3[0x503f - 0x501c];
    unsigned char c503f;
};

typedef void (Obj::*ObjFn)();

extern ObjFn data_ov006_02142bdc[];
extern int data_0209e650;
extern unsigned char data_0209d45c;

extern "C" void func_ov006_020c2144(void *a);
extern "C" int RandomIntInternal(int *seed);

extern "C" int func_ov006_0210bcb0(Obj *self) {
    int i;
    unsigned char t;

    unsigned char *pc;
    unsigned short *ph1;
    unsigned short *ph2;

    (self->*data_ov006_02142bdc[self->idx])();

    pc = (unsigned char *)(((long long)(int)((char *)self + 0x503f)) & 0xFFFFFFFFFFFFFFFFLL);
    *pc = *pc + 1;
    func_ov006_020c2144((char *)self + 0x4f38);

    for (i = 0; i < 3; i++) {
        if (self->idx == 1) {
            ph1 = (unsigned short *)(((long long)(int)((char *)self + 0x5018)) & 0xFFFFFFFFFFFFFFFFLL);
            ph2 = (unsigned short *)(((long long)(int)((char *)self + 0x501a)) & 0xFFFFFFFFFFFFFFFFLL);
            *ph1 = *ph1 - 0x200;
            *ph2 = *ph2 - 0x400;
            break;
        }
    }

    t = ((unsigned int)RandomIntInternal(&data_0209e650) >> 16) & 1;
    if (t)
        data_0209d45c |= 2;
    else
        data_0209d45c &= ~2;

    return 1;
}

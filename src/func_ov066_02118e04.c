typedef unsigned char u8;
typedef unsigned short u16;

extern void* _ZN5Actor13ClosestPlayerEv(void* self);
extern void func_ov066_02119454(void* c, void* p);
extern int RandomIntInternal(int* seed);
extern int func_ov066_02116a68(void* self);

extern u8 data_ov066_0211abe0;
extern int data_ov066_0211abe4;
extern u8 data_ov066_0211ae04;
extern u8 data_ov066_0211ae08;
extern u8 data_ov066_0211ae0c;
extern int data_0209e650;
extern char data_ov066_0211b0ac[];
extern char data_ov066_0211b0dc[];
extern char data_ov066_0211b00c[];
extern char data_ov066_0211b02c[];
extern char data_ov066_0211afcc[];
extern char data_ov066_0211affc[];
extern char data_ov066_0211afdc[];

int func_ov066_02118e04(void* self)
{
    u8* c = (u8*)self;
    void* p = _ZN5Actor13ClosestPlayerEv(self);
    int r4;
    int v;

    if (data_ov066_0211abe0 == 0) {
        *(u16*)(c + 0x4d2) = 0x64;
        func_ov066_02119454(c, data_ov066_0211b0ac);
        return 1;
    }

    if (p == 0 || *(u16*)(c + 0x4d2) != 0)
        return 1;

    r4 = ((unsigned int)RandomIntInternal(&data_0209e650) >> 31) & 1;

    data_ov066_0211ae08 = 0;
    data_ov066_0211ae0c = 0;
    v = func_ov066_02116a68(c);
    if (v == (int)0xff3ae000) {
        data_ov066_0211ae04 = 4;
        func_ov066_02119454(c, data_ov066_0211b0dc);
        return 1;
    }

    if ((int)*(u8*)(c + 0x4d9) > data_ov066_0211abe4 + 3) {
        if (data_ov066_0211abe0 == 3) {
            data_ov066_0211abe4++;
            data_ov066_0211abe4 &= 1;
            data_ov066_0211ae04 = 8;
            func_ov066_02119454(c, data_ov066_0211b00c);
        } else {
            data_ov066_0211abe4 = -3;
            data_ov066_0211ae04 = 9;
            func_ov066_02119454(c, data_ov066_0211b02c);
        }
        return 1;
    }

    v = func_ov066_02116a68(c);
    if (v == -0xb50000) {
        data_ov066_0211ae04 = 5;
        func_ov066_02119454(c, data_ov066_0211afcc);
        return 1;
    }

    if (r4 == 0) {
        data_ov066_0211ae04 = 7;
        func_ov066_02119454(c, data_ov066_0211affc);
    } else {
        data_ov066_0211ae04 = 6;
        func_ov066_02119454(c, data_ov066_0211afdc);
    }
    return 1;
}

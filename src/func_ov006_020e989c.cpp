//cpp
typedef unsigned short u16;
typedef unsigned char u8;
typedef short s16;
typedef unsigned int u32;

struct C70 {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18(int);
};

extern "C" {
extern char *func_020beb68;
extern unsigned int data_ov006_0212e594[];
extern unsigned char data_020a0e40;
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];

extern void func_ov006_020e8d08(char *c);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);
extern unsigned int func_02012790(unsigned int);
extern void func_ov004_020adb1c(int);
extern void func_ov004_020b67e8(int);
extern void func_ov004_020b0a54(int);
extern void func_ov004_020b0cac(int, int, int, int, int, short);
extern void func_ov006_020e95a4(char *);
}

extern "C" void func_ov006_020e989c(C70 *cc)
{
    char *c = (char *)cc;

    func_ov006_020e8d08(c);
    if (*(u16 *)(c + 0x5548) != 0) {
        (*(u16 *)(int)(((long long)(int)(c + 0x5548))))--;
        if (*(u16 *)(c + 0x5548) == 0x40) {
            if (*(u8 *)(c + 0x5550) != 0) {
                int idx = func_020beb68 != 0 ? *(int *)(func_020beb68 + 0xb4) : 0;
                while (idx >= 5)
                    idx -= 5;
                if (func_020beb68 != 0) {
                    char *g = func_020beb68;
                    if (*(int *)(g + 0xb4) < 9999)
                        (*(int *)(int)(((long long)(int)(g + 0xb4))))++;
                    if (*(int *)(g + 0xb4) > *(int *)(g + 0xb8))
                        *(int *)(g + 0xb8) = *(int *)(g + 0xb4);
                }
                _ZN5Sound12PlayBank2_2DEj(data_ov006_0212e594[idx]);
            } else {
                func_02012790(0xe);
            }
        }
        if (*(s16 *)(c + 0x5548) > 0)
            return;
        if (*(u8 *)(c + 0x5550) != 0) {
            int rem, quot;
            func_ov004_020adb1c(func_020beb68 != 0 ? *(int *)(func_020beb68 + 0xb4) : 0);
            rem = func_020beb68 != 0 ? *(int *)(func_020beb68 + 0xb4) : 0;
            quot = 0;
            while (rem >= 5) {
                rem -= 5;
                quot++;
            }
            if (rem == 0 && quot != 0) {
                if (quot <= 2)
                    func_ov004_020b67e8(0x14);
                else
                    func_ov004_020b67e8(0x15);
                func_ov004_020b0a54(0);
            } else {
                (*(int *)(int)(((long long)(int)(c + 0xbc))))++;
                if (*(u32 *)(c + 0xbc) > 0x270e)
                    *(u32 *)(c + 0xbc) = 0x270e;
                cc->v18(-1);
            }
        } else {
            *(u16 *)(c + 0x554c) = 0;
            *(u8 *)(c + 0x5555) = 0;
            func_ov004_020b0cac(8, 0x80, 0xa0, 1, -1, 0xd);
            func_ov004_020b0a54(0x12);
            *(int *)(c + 0x51e4) = 1;
        }
        *(u8 *)(c + 0xc3) = 0;
        *(u16 *)(c + 0x5548) = 0;
    } else {
        int b;
        u8 i;
        if (*(u8 *)(c + 0x5550) != 0)
            return;
        b = 0;
        i = data_020a0e40;
        if (data_020a0de8[i * 4] != 0) {
            if (data_020a0de9[i * 4] != 0)
                b = 1;
        }
        if (b != 0)
            func_ov006_020e95a4(c);
        (*(u16 *)(int)(((long long)(int)(c + 0x554c))))++;
        if (*(u16 *)(c + 0x554c) == 0xb4)
            func_ov006_020e95a4(c);
        if (*(u16 *)(c + 0x554c) >= 0xb5)
            *(u16 *)(c + 0x554c) = 0xb5;
    }
}

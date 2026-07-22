//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern "C" {
    void func_ov006_020d0ac0(void);
    int func_02054d88(void);
    void MultiStore16(u16 val, char *dst, int nbytes);
    void func_ov006_020d0bd8(void);
    void func_02012718(void *a, int b);
    void func_ov006_02121cf4(char *c);
    int _ZN4cstd4fdivEii(int a, int b);
    int RandomIntInternal(int *seed);
    void func_ov004_020ae5c4(void *a, int b, int c, int d, int e, int f, int g);
    int func_020126e8(int a);
    int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
}

extern s16 data_ov006_0212e04c;
extern s16 data_ov006_0212e044;
extern s16 data_ov006_0212e048;
extern int data_0209e650;

extern "C" void func_ov006_02121d64(char *c)
{
    int counter;

    func_ov006_020d0ac0();
    *(int *)(((long long)(int)(c + 0x5d90)) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
    counter = *(int *)(c + 0x5d90);

    if (counter == 0) {
        if (*(u8 *)(c + 0xc4) == 0) {
            *(u8 *)(c + 0xc3) = 1;
            *(u8 *)(c + 0xc4) = 1;
            *(s16 *)(c + 0xc0) = 0;
        }

        {
            int dst = func_02054d88();
            volatile u16 tmp = 0;
            MultiStore16(tmp, (char *)dst, 0x6000);
        }

        func_ov006_020d0bd8();
        func_02012718((void *)0x1af, 0x8000);
        func_ov006_02121cf4(c);
        return;
    }

    {
        int t = _ZN4cstd4fdivEii(counter << 12, 0x5a000);
        volatile s16 oldY = *(s16 *)(c + 0x5db0);
        volatile s16 oldZ = *(s16 *)(c + 0x5db2);
        int mixRaw = data_ov006_0212e044 * t +
                     data_ov006_0212e04c * (0x1000 - t);

        *(s16 *)(c + 0x5db0) = (s16)(mixRaw >> 12);
        *(s16 *)(c + 0x5db2) = data_ov006_0212e048;

        *(s16 *)(((long long)(int)(c + 0x5db2)) & 0xFFFFFFFFFFFFFFFFLL) +=
            ((((int)((unsigned int)(RandomIntInternal(&data_0209e650) &
                              ~0x80000000) >> 19) -
               0x800) << 2) >> 12);

        func_ov004_020ae5c4(c, oldY, oldZ,
                            *(s16 *)(c + 0x5db0),
                            *(s16 *)(c + 0x5db2), 2, 0xc);

        *(int *)(c + 0x5dac) =
            func_02012468(*(int *)(c + 0x5dac), 2, 0x1b0, 2, 0,
                           func_020126e8(mixRaw), 0, 0);
    }
}

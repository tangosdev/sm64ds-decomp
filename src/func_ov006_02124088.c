typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern void func_ov006_020d0ac0(void);
extern int func_02054d88(void);
extern void MultiStore16(u16 val, char *dst, int nbytes);
extern void func_ov006_020d0b78(void);
extern void func_02012718(void *a, int b);
extern void func_ov006_02124040(char *c);
extern int _ZN4cstd4fdivEii(int a, int b);
extern int RandomIntInternal(int *seed);
extern int func_020126e8(int a);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, int h);
extern int func_ov004_020ae5c4(void *a, int b, int c, int d, int e, int f, int g);

extern s16 data_ov006_0212e058;
extern s16 data_ov006_0212e050;
extern s16 data_ov006_0212e048;
extern int data_0209e650;

void func_ov006_02124088(char *c)
{
    int counter;

    func_ov006_020d0ac0();
    *(int *)(((long long)(int)(c + 0x7b84)) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
    counter = *(int *)(c + 0x7b84);

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

        func_ov006_020d0b78();
        func_02012718((void *)0x1af, 0x8000);
        func_ov006_02124040(c);
        return;
    }

    {
        int t = _ZN4cstd4fdivEii(counter << 12, 0x5a000);
        volatile s16 old9c = *(s16 *)(c + 0x7b9c);
        volatile s16 old9e = *(s16 *)(c + 0x7b9e);
        int mixRaw = data_ov006_0212e050 * t + data_ov006_0212e058 * (0x1000 - t);
        *(s16 *)(c + 0x7b9c) = (s16)(mixRaw >> 12);
        *(s16 *)(c + 0x7b9e) = data_ov006_0212e048;

        *(s16 *)(((long long)(int)(c + 0x7b9e)) & 0xFFFFFFFFFFFFFFFFLL) +=
            ((((int)((unsigned int)(RandomIntInternal(&data_0209e650) & ~0x80000000) >> 19) - 0x800) << 2) >> 12);

        func_ov004_020ae5c4(c, old9c, old9e, *(s16 *)(c + 0x7b9c), *(s16 *)(c + 0x7b9e), 2, 0xc);

        *(int *)(c + 0x7b8c) = func_02012468(*(int *)(c + 0x7b8c), 2, 0x1b0, 2, 0,
                                              func_020126e8(mixRaw), 0, 0);
    }
}

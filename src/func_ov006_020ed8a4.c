typedef int s32;
typedef short s16;
typedef unsigned char u8;

typedef struct { int a, b; } Pair;

extern int func_ov006_020ebe6c(void);
extern int RandomIntInternal(int *seed);
extern void func_ov006_020ebd7c(int count);
extern void func_ov006_020ecdb8(char *self, int arg1, int arg2);
extern void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);

extern s16 data_ov006_0212e80c[];
extern volatile int data_ov006_0212e820[];
extern volatile s16 data_ov006_02141fd8;
extern int data_ov006_0213c958;
extern int data_0209e650;
extern Pair data_ov006_0213c964;

void func_ov006_020ed8a4(char *r4) {
    if (*(s32 *)(r4 + 0xbc) < 0xa) {
        int bc = *(s32 *)(r4 + 0xbc);
        data_ov006_02141fd8 = data_ov006_0212e80c[bc];
        data_ov006_0213c958 = data_ov006_0212e820[bc];
    } else {
        int ret = func_ov006_020ebe6c();
        data_ov006_02141fd8 = ret;
        if (ret == 3) {
            data_ov006_0213c958 = 0xf;
        } else {
            unsigned int rnd = RandomIntInternal(&data_0209e650) & 0x7fffffff;
            data_ov006_0213c958 = ((int)(rnd >> 19) * 3 >> 12) + 3;
        }
    }

    if (*(unsigned int *)(r4 + 0xbc) < 0xa)
        func_ov006_020ebd7c(2);
    else
        func_ov006_020ebd7c(3);

    {
        int i = 0;
        if (data_ov006_0213c958 > 0) {
            char *p = (char *)(r4 + 0x4678);
            do {
                func_ov006_020ecdb8(p, i, *(s32 *)(r4 + 0xbc));
                i++;
                p += 0x98;
            } while (i < data_ov006_0213c958);
        }
    }

    {
        unsigned int rnd = RandomIntInternal(&data_0209e650) & 0x7fffffff;
        int k = (int)(rnd >> 19) * data_ov006_0213c958 >> 12;
        *(char **)(r4 + 0x4f60) = (char *)(r4 + 0x4678) + k * 0x98;
        *(s32 *)(*(char **)(r4 + 0x4f60) + 0x70) = 0;
        *(s32 *)(r4 + 0x4668) = 0x3c;
        func_ov004_020b0cac(0xd, 0x80, 0x50, 1, -1, 0xd);
        *(s32 *)(r4 + 0x4674) = 0x3c;
        *(u8 *)(r4 + 0x4f64) = 0;
        *(Pair *)(r4 + 0x4660) = data_ov006_0213c964;
    }
}

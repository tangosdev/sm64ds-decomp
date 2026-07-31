typedef short s16;
typedef int s32;
typedef unsigned int u32;

extern void func_020731dc(int a, int b, void **node);
extern void func_0203d704(int* o, int* a, int* b);
extern void func_0203d388(int *p, int angle);
extern int RandomIntInternal(int* seed);
extern void func_ov004_020adfc4(int c, short a1, int* a2, int* r3, int* sp0);
extern void NullDestructor_0203d47c(void);

struct Pair { int a, b; };
extern struct Pair data_ov004_020bc17c;
extern struct Pair data_ov004_020bc1ec;
extern int data_ov004_020bf3ec;
extern int data_ov004_020bf3f4[2];
extern void* data_ov004_020bf410[3];
extern int data_0209e650;

void func_ov004_020b39a4(char* c) {
    int i;
    int angle;
    int boxA[2], boxB[2], boxC[2];
    int posA[2];
    int outA[2], outB[2];
    int flags;

    int shifted;

    *(int*)(c + 0x24) = 0;
    angle = *(short*)(c + 0x2c);
    angle = angle + 1;
    shifted = angle << 14;
    *(int*)(c + 0x28) = shifted;

    *(struct Pair*)(c + 0)   = data_ov004_020bc17c;
    *(struct Pair*)(c + 8)   = data_ov004_020bc1ec;

    boxA[0] = 0; boxA[1] = -0x2000;
    boxB[0] = 0; boxB[1] = -0x1800;
    boxC[0] = 0; boxC[1] = -0x4000;

    flags = data_ov004_020bf3ec;

    {
        int y12 = *(short*)(c + 0x12);
        int y10 = *(short*)(c + 0x10);
        int sY12, sY10;
        flags = flags & 1;
        sY12 = y12 << 12;
        sY10 = y10 << 12;
        posA[0] = sY10;
        posA[1] = sY12;
    }

    if (flags == 0) {
        data_ov004_020bf3f4[0] = 0;
        data_ov004_020bf3f4[1] = 0xc0;
        func_020731dc((int)data_ov004_020bf3f4, (int)NullDestructor_0203d47c, data_ov004_020bf410);
        data_ov004_020bf3ec = data_ov004_020bf3ec | 1;
    }

    for (i = 0; i < 0x20; i++) {
        int r;
        int ang;
        s16 spawnAngle;

        func_0203d704(outA, posA, boxC);
        func_0203d388(boxA, 0x800);
        func_0203d388(boxC, 0x800);
        func_0203d704(outB, boxB, boxA);

        r = RandomIntInternal(&data_0209e650);
        r &= 0x7fffffff;
        ang = (int)((u32)r >> 0x13) * 0x3c >> 0xc;
        ang = ang + 0x5a;
        spawnAngle = (s16)ang;

        func_ov004_020adfc4(0, spawnAngle, outA, outB, data_ov004_020bf3f4);
    }
}

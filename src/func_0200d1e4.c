typedef unsigned int u32;
typedef int s32;

extern unsigned int func_020093f4(void *p, int x);
extern unsigned int func_020093d4(void *p, int a);
extern void Vec3_RotateYAndTranslate(void *dst, void *src, short angle, void *unk);
extern void ChangeArea(int areaID);

extern s32 data_02086f2c[3];
extern s32 data_02086f14[3];

void func_0200d1e4(char *self)
{
    char *info = *(char **)(self + 0x110);
    char *base = (char *)(((long long)(int)(info + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);

    *(s32 *)(self + 0x98) = *(s32 *)(base + 0);
    *(s32 *)(self + 0x9c) = *(s32 *)(base + 4);
    *(s32 *)(self + 0xa0) = *(s32 *)(base + 8);

    int v1 = *(s32 *)(*(char **)(self + 0x13c) + 0x14);
    unsigned int r0 = func_020093f4(self, v1);

    s32 temp1[3];
    temp1[0] = data_02086f2c[0];
    temp1[1] = data_02086f2c[1];
    temp1[2] = r0;
    data_02086f2c[2] = r0;

    short angle = *(short *)(*(char **)(self + 0x110) + 0x8e);
    Vec3_RotateYAndTranslate(self + 0x80, self + 0x98, angle, temp1);

    int v2 = *(s32 *)(*(char **)(self + 0x13c) + 0x10);
    unsigned int dResult = func_020093d4(self, v2);
    *(s32 *)(((long long)(int)(self + 0x84)) & 0xFFFFFFFFFFFFFFFFLL) += dResult;

    int v3 = *(s32 *)(*(char **)(self + 0x13c) + 0x20);
    unsigned int r0b = func_020093f4(self, v3);

    s32 temp2[3];
    data_02086f14[2] = r0b;
    temp2[0] = data_02086f14[0];
    temp2[1] = data_02086f14[1];
    temp2[2] = r0b;

    angle = *(short *)(*(char **)(self + 0x110) + 0x8e);
    Vec3_RotateYAndTranslate(self + 0x8c, self + 0x80, angle, temp2);

    *(s32 *)(((long long)(int)(self + 0x90)) & 0xFFFFFFFFFFFFFFFFLL) += (int)(dResult - 0x8b000);

    *(s32 *)(self + 0xa4) = 0;
    *(s32 *)(self + 0xa8) = 0xc3f9d;
    *(s32 *)(self + 0xac) = 0;

    ChangeArea(*(signed char *)(*(char **)(self + 0x110) + 0xcc));
}

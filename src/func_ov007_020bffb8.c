typedef unsigned short u16;
typedef short s16;
typedef int s32;

extern s16 data_02082214[];

void func_ov007_020bffb8(char *c)
{
    s32 *out = (s32 *)(((int)c + 8));
    s32 *base = (s32 *)(((int)c + 0x14));
    s32 angleA = *(u16 *)(c + 0x30);
    s32 angleB = *(u16 *)(c + 0x32);
    s32 indexA;
    s32 indexB;
    s32 trigAPlus;

    angleA >>= 4;
    angleB >>= 4;
    indexA = angleA << 1;
    indexB = angleB << 1;
    trigAPlus = data_02082214[indexA + 1];

    {
        s32 initialX = (trigAPlus * *(s32 *)(c + 0x2c)) >> 12;
        s32 trigB = data_02082214[indexB];
        s32 storedX;
        s32 product;

        out[0] = initialX;
        storedX = out[0];
        product = storedX * trigB;
        out[0] = base[0] + (product >> 12);
    }
    {
        s32 trigA = data_02082214[indexA];
        s32 product = trigA * *(s32 *)(c + 0x2c);
        out[1] = base[1] + (product >> 12);
    }
    {
        s32 initialZ = (trigAPlus * *(s32 *)(c + 0x2c)) >> 12;
        s32 trigB = data_02082214[indexB + 1];
        s32 storedZ;
        s32 product;

        out[2] = initialZ;
        storedZ = out[2];
        product = storedZ * trigB;
        out[2] = base[2] + (product >> 12);
    }
}

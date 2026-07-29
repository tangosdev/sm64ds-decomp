#pragma opt_common_subs off
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef long long s64;

extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern int data_ov006_0212e8b8[];
extern s16 data_02082214[];

void func_ov006_020f13cc(char *c, int i)
{
    u8 *counterAi = (u8 *)(c + 0x5275 + i);

    if (*counterAi == 0) {
        u32 r = ((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        u16 val = (u16)((((r << 4) >> 15) << 12));

        *(u16 *)(c + i * 2 + 0x4f7c) = val;
        *counterAi += 1;
        return;
    }

    {
        u8 *counterC = (u8 *)(c + 0x5365);
        int *accB = (int *)(c + 0x47f8);
        int *accD = (int *)(c + 0x49d8);
        u16 valB;
        int shifted;
        int tblOdd, tblEven;
        int weight;
        s64 prod;
        int add12 = 0x800;

        valB = *(u16 *)(c + i * 2 + 0x4f00 + 0x7c);
        shifted = valB >> 4;
        tblOdd = data_02082214[shifted * 2 + 1];
        weight = data_ov006_0212e8b8[counterC[i]];
        prod = (s64)tblOdd * weight;
        accB[i] += (int)((prod + add12) >> 12);

        valB = *(u16 *)(c + i * 2 + 0x4f00 + 0x7c);
        shifted = valB >> 4;
        tblEven = data_02082214[shifted * 2];
        weight = data_ov006_0212e8b8[counterC[i]];
        prod = (s64)tblEven * weight;
        accD[i] += (int)((prod + add12) >> 12);

        {
            int x = *(int *)(c + i * 4 + 0x4000 + 0x7f8) >> 0xc;
            int y = *(int *)(c + i * 4 + 0x4000 + 0x9d8) >> 0xc;

            if (x > 0xf8) {
                *(u16 *)(c + i * 2 + 0x4f00 + 0x7c) = 0x8000 - *(u16 *)(c + i * 2 + 0x4f00 + 0x7c);
                *(int *)(c + i * 4 + 0x4000 + 0x7f8) = 0xf8000;
            } else if (x < 8) {
                *(u16 *)(c + i * 2 + 0x4f00 + 0x7c) = 0x8000 - *(u16 *)(c + i * 2 + 0x4f00 + 0x7c);
                *(int *)(c + i * 4 + 0x4000 + 0x7f8) = 0x8000;
            }

            if (y > 0xb8) {
                *(u16 *)(c + i * 2 + 0x4f00 + 0x7c) = -*(u16 *)(c + i * 2 + 0x4f00 + 0x7c);
                *(int *)(c + i * 4 + 0x4000 + 0x9d8) = 0xb8000;
                return;
            }

            if (y < 8) {
                *(u16 *)(c + i * 2 + 0x4f00 + 0x7c) = -*(u16 *)(c + i * 2 + 0x4f00 + 0x7c);
                *(int *)(c + i * 4 + 0x4000 + 0x9d8) = 0x8000;
                return;
            }

            return;
        }
    }
}

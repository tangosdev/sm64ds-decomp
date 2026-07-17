typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef long long s64;

#pragma opt_strength_reduction off
#pragma opt_common_subs off

extern int data_ov006_0212e430[];
extern u16 data_ov006_0212e334[];
extern s16 data_02082214[];

void func_ov006_020dcb1c(char *o, int a1)
{
    int i, j, k;
    char *b = o + a1 * 0x18;
    int *px = (int *)(((int)b + 0x4ac0) & 0xFFFFFFFFFFFFFFFF);
    int *pz = (int *)(((int)b + 0x4ac4) & 0xFFFFFFFFFFFFFFFF);
    for (i = 0, j = 0, k = 1; i < 4; i++, o += 0x18, j += 2, k += 2) {
        if (*(u8 *)(o + 0x4d28) == 0) {
            *(u8 *)(o + 0x4d28) = 1;
            *(u8 *)(o + 0x4d29) = 1;
            *(int *)(o + 0x4d14) = *px + (data_ov006_0212e430[j] << 12);
            *(int *)(o + 0x4d18) = *pz + (data_ov006_0212e430[k] << 12);
            {
                u16 ang = data_ov006_0212e334[i];
                int idx = (ang >> 4) * 2;
                *(int *)(o + 0x4d1c) = (int)(((s64)data_02082214[idx + 1] * 0x2000 + 0x800) >> 12);
                *(int *)(o + 0x4d20) = (int)(((s64)data_02082214[idx] * 0x2000 + 0x800) >> 12);
            }
            *(u16 *)(o + 0x4d24) = 0x18;
        }
    }
}

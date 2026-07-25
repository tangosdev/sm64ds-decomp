typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;

extern void func_ov006_020f1dbc(char *self, int idx);
extern int data_ov006_0212e8d8[];
extern short data_02082214[];

void func_ov006_020f1b98(char *base, int idx)
{
    u8 *flag = (u8 *)(base + 0x5275 + idx);
    if (*flag == 0) {
        u8 t = *(u8 *)(base + 0x5365 + idx);
        *(u16 *)(base + idx * 2 + 0x4f7c) = *(u16 *)(base + t * 2 + 0x515c);
        (*flag)++;
    } else {
        s32 a0 = ((s32)*(u16 *)(base + idx * 2 + 0x4f7c)) >> 4;
        s32 idx0 = data_ov006_0212e8d8[*(u8 *)(base + 0x5365 + idx)];
        s32 v0 = data_02082214[a0 * 2 + 1];
        *(s32 *)(base + 0x47f8 + idx * 4) += (s32)(((s64)v0 * idx0 + 0x800) >> 12);

        {
            s32 a1 = ((s32)*(u16 *)(base + idx * 2 + 0x4f7c)) >> 4;
            s32 idx1 = data_ov006_0212e8d8[*(u8 *)(base + 0x5365 + idx)];
            s32 v1 = data_02082214[a1 * 2];
            *(s32 *)(base + 0x49d8 + idx * 4) += (s32)(((s64)v1 * idx1 + 0x800) >> 12);
        }
        func_ov006_020f1dbc(base, idx);
    }
}

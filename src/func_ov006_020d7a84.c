typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef long long s64;

#pragma opt_strength_reduction off
#pragma opt_common_subs off

extern s16 _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern void func_ov006_020d634c(char *c, int i);
extern s16 data_02082214[];

void func_ov006_020d7a84(char *c, int i)
{
    int b, x, z, tx, ty;

    tx = (*(u8 *)(c + i * 0x40 + 0x4696) != 0) ? 0x100 : 0;
    ty = 0x60;
    tx -= *(int *)(c + i * 0x40 + 0x4660) >> 12;
    ty -= *(int *)(c + i * 0x40 + 0x4664) >> 12;

    *(s16 *)(c + i * 0x40 + 0x468c) =
        _ZN4cstd5atan2E5Fix12IiES1_(ty, tx);

    {
        s16 tv = data_02082214[
            ((*(u16 *)(c + i * 0x40 + 0x468c) >> 4) * 2) + 1];

        *(int *)((char *)(((int)c + 0x4660) & 0xFFFFFFFFFFFFFFFF) +
                  i * 0x40) +=
            (int)(((s64)tv *
                   *(int *)(c + i * 0x40 + 0x4670) + 0x800) >> 12);
    }

    {
        s16 tv = data_02082214[
            (*(u16 *)(c + i * 0x40 + 0x468c) >> 4) * 2];

        *(int *)((char *)(((int)c + 0x4664) & 0xFFFFFFFFFFFFFFFF) +
                  i * 0x40) +=
            (int)(((s64)tv *
                   *(int *)(c + i * 0x40 + 0x4670) + 0x800) >> 12);
    }

    b = *(u8 *)(c + i * 0x40 + 0x4696);
    x = *(int *)(c + i * 0x40 + 0x4660) >> 12;
    z = *(int *)(c + i * 0x40 + 0x4664) >> 12;
    tx = b ? 0x100 : 0;

    if (x > tx + 2)
        goto done;
    if (x < tx - 2)
        goto done;
    if (z > 0x62)
        goto done;
    if (z < 0x5e)
        goto done;

    *(u8 *)(c + i * 0x40 + 0x469b) = 1;
    if (*(u8 *)(c + i * 0x40 + 0x4696) != 0)
        *(s16 *)(c + i * 0x40 + 0x468c) = 0;
    else
        *(u16 *)(c + i * 0x40 + 0x468c) = 0x8000;

done:
    func_ov006_020d634c(c, *(u8 *)(c + i * 0x40 + 0x4696));
}

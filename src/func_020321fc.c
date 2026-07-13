typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed short s16;

extern u8 data_0209fcb8;
extern u8 data_0209fc6c;
extern u8 data_0209fc8c;
extern u8 data_0209fce0;
extern u8 data_0209fc74;
extern u8 data_0209fcac;
extern u8 data_0209fce4;
extern u8 data_0209fcc0;
extern u8 data_0209fcb4;
extern u8 data_0209fcc8;
extern u8 data_0209fca8;
extern u8 data_0209fc9c;
extern u8 data_0209d454;
extern u8 data_0209d45c;

int func_020321fc(void)
{
    if (data_0209fcb8 == 0)
    {
        if (data_0209fc8c > data_0209fc6c && data_0209fc74 > data_0209fce0)
        {
            int t;
            s16 v;
            int mask;

            data_0209fcac++;
            t = data_0209fcac * (data_0209fce4 - data_0209fc6c + 1);
            t = data_0209fce4 - t / 8;
            v = t;
            if (v < 0)
                data_0209fc8c = 0;
            else
                data_0209fc8c = t;
            t = data_0209fcac * (data_0209fcc0 - data_0209fce0 + 1);
            data_0209fc74 = data_0209fcc0 - t / 8;
            mask = 0;
            if (data_0209fc8c <= data_0209fc6c)
            {
                data_0209fc8c = data_0209fc6c;
                mask = 1;
            }
            if (data_0209fc74 <= data_0209fce0)
            {
                mask |= 2;
                data_0209fc74 = data_0209fce0;
            }
            if (mask == 3)
                data_0209fcc8 = data_0209fcb4;

            if (data_0209fca8 != 0)
            {
                *(volatile u16 *)0x4001040 = ((data_0209fc8c << 8) & 0xff00) | ((data_0209fce4 * 2 - data_0209fc8c + 1) & 0xff);
                *(volatile u16 *)0x4001044 = ((data_0209fc74 << 8) & 0xff00) | ((data_0209fcc0 * 2 - data_0209fc74 + 1) & 0xff);
            }
            else
            {
                *(volatile u16 *)0x4000040 = ((data_0209fc8c << 8) & 0xff00) | ((data_0209fce4 * 2 - data_0209fc8c + 1) & 0xff);
                *(volatile u16 *)0x4000044 = ((data_0209fc74 << 8) & 0xff00) | ((data_0209fcc0 * 2 - data_0209fc74 + 1) & 0xff);
            }
            return 0;
        }
        return 1;
    }

    if (data_0209fc8c < data_0209fce4 && data_0209fc74 < data_0209fcc0)
    {
        int t;
        int t2;

        data_0209fcac--;
        t = data_0209fcac * (data_0209fce4 - data_0209fc6c + 1);
        t2 = data_0209fcac * (data_0209fcc0 - data_0209fce0 + 1);
        data_0209fc8c = data_0209fce4 - t / 8;
        data_0209fc74 = data_0209fcc0 - t2 / 8;
        data_0209fc8c += 7;
        data_0209fc74 += 5;
        if (data_0209fc8c >= data_0209fce4)
            data_0209fc8c = data_0209fce4;
        if (data_0209fc74 >= data_0209fcc0)
            data_0209fc74 = data_0209fcc0;

        if (data_0209fca8 != 0)
        {
            *(volatile u16 *)0x4001040 = ((data_0209fc8c << 8) & 0xff00) | ((data_0209fce4 * 2 - data_0209fc8c + 1) & 0xff);
            *(volatile u16 *)0x4001044 = ((data_0209fc74 << 8) & 0xff00) | ((data_0209fcc0 * 2 - data_0209fc74 + 1) & 0xff);
        }
        else
        {
            *(volatile u16 *)0x4000040 = ((data_0209fc8c << 8) & 0xff00) | ((data_0209fce4 * 2 - data_0209fc8c + 1) & 0xff);
            *(volatile u16 *)0x4000044 = ((data_0209fc74 << 8) & 0xff00) | ((data_0209fcc0 * 2 - data_0209fc74 + 1) & 0xff);
        }
        return 0;
    }

    data_0209fc9c = 0;
    data_0209fcb8 = 0;
    if (data_0209fca8 != 0)
    {
        data_0209d454 &= ~2;
        *(volatile u32 *)0x4001000 = (*(volatile u32 *)0x4001000 & ~0x1f00) | (data_0209d454 << 8);
        *(volatile u16 *)0x4001050 = 0;
        *(volatile u16 *)0x4001048 &= ~0x3f;
        *(volatile u16 *)0x400104a &= ~0x3f;
        *(volatile u32 *)0x4001000 &= ~0xe000;
    }
    else
    {
        data_0209d45c &= ~8;
        *(volatile u32 *)0x4000000 = (*(volatile u32 *)0x4000000 & ~0x1f00) | (data_0209d45c << 8);
        *(volatile u16 *)0x4000050 = 0;
        *(volatile u16 *)0x4000048 &= ~0x3f;
        *(volatile u16 *)0x400004a &= ~0x3f;
        *(volatile u32 *)0x4000000 &= ~0xe000;
    }
    return 0;
}

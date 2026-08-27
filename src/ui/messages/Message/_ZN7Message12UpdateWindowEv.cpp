//cpp
#include "types.h"
extern "C" {
    void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int vol, int t);
    int func_0201fb4c(void);
    extern u32 data_0209b454;
    extern u8 data_0209d454;
    extern u8 data_0209d45c;
    extern u8 data_0209d64c;
    extern u8 data_0209d650;
    extern u8 data_0209d658;
    extern u8 data_0209d660;
    extern u8 data_0209d670;
    extern u8 data_0209d680;
    extern u8 data_0209d698;
    extern u8 data_0209d6ac;
    extern u8 data_0209d6bc;
    extern u8 data_0209d6c8;
    extern u8 data_0209d6cc;
    extern u8 data_0209d6d0;
}

struct Message {
    static int UpdateWindow();
};

int Message::UpdateWindow()
{
    if (data_0209d670 == 0)
    {
        if (data_0209d658 > data_0209d650 || data_0209d64c > data_0209d6cc)
        {
            int t;
            s16 v;
            int mask;

            data_0209d680++;
            t = data_0209d680 * (data_0209d6d0 - data_0209d650 + 1);
            t = data_0209d6d0 - t / 8;
            v = t;
            if (v < 0)
                data_0209d658 = 0;
            else
                data_0209d658 = t;
            t = data_0209d680 * (data_0209d6c8 - data_0209d6cc + 1);
            data_0209d64c = data_0209d6c8 - t / 8;
            mask = 0;
            if (data_0209d658 <= data_0209d650)
            {
                data_0209d658 = data_0209d650;
                mask = 1;
            }
            if (data_0209d64c <= data_0209d6cc)
            {
                mask |= 2;
                data_0209d64c = data_0209d6cc;
            }
            if (mask == 3)
                data_0209d6bc = data_0209d6ac;

            if (data_0209d698 != 0)
            {
                *(volatile u16 *)0x4001040 = ((data_0209d658 << 8) & 0xff00) | ((data_0209d6d0 * 2 - data_0209d658 + 1) & 0xff);
                *(volatile u16 *)0x4001044 = ((data_0209d64c << 8) & 0xff00) | ((data_0209d6c8 * 2 - data_0209d64c + 1) & 0xff);
            }
            else
            {
                *(volatile u16 *)0x4000040 = ((data_0209d658 << 8) & 0xff00) | ((data_0209d6d0 * 2 - data_0209d658 + 1) & 0xff);
                *(volatile u16 *)0x4000044 = ((data_0209d64c << 8) & 0xff00) | ((data_0209d6c8 * 2 - data_0209d64c + 1) & 0xff);
            }
            return 0;
        }
        return 1;
    }

    if (data_0209d658 < data_0209d6d0 && data_0209d64c < data_0209d6c8)
    {
        int t;
        int t2;

        data_0209d680--;
        t = data_0209d680 * (data_0209d6d0 - data_0209d650 + 1);
        t2 = data_0209d680 * (data_0209d6c8 - data_0209d6cc + 1);
        data_0209d658 = data_0209d6d0 - t / 8;
        data_0209d64c = data_0209d6c8 - t2 / 8;
        data_0209d658 += 7;
        data_0209d64c += 5;
        if (data_0209d658 >= data_0209d6d0)
            data_0209d658 = data_0209d6d0;
        if (data_0209d64c >= data_0209d6c8)
            data_0209d64c = data_0209d6c8;

        if (data_0209d698 != 0)
        {
            *(volatile u16 *)0x4001040 = ((data_0209d658 << 8) & 0xff00) | ((data_0209d6d0 * 2 - data_0209d658 + 1) & 0xff);
            *(volatile u16 *)0x4001044 = ((data_0209d64c << 8) & 0xff00) | ((data_0209d6c8 * 2 - data_0209d64c + 1) & 0xff);
        }
        else
        {
            *(volatile u16 *)0x4000040 = ((data_0209d658 << 8) & 0xff00) | ((data_0209d6d0 * 2 - data_0209d658 + 1) & 0xff);
            *(volatile u16 *)0x4000044 = ((data_0209d64c << 8) & 0xff00) | ((data_0209d6c8 * 2 - data_0209d64c + 1) & 0xff);
        }
        return 0;
    }

    data_0209b454 &= ~0x80000000;
    if (func_0201fb4c() != 0)
    {
        _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x64cc);
    }
    data_0209d660 = 0;
    data_0209d670 = 0;
    if (data_0209d698 != 0)
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

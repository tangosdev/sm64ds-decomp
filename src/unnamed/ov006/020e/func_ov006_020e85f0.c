extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;

#pragma opt_common_subs off
void func_ov006_020e85f0(char *c, int i)
{
    *(int*)(c + 0x52bc + i * 0x20) += *(int*)(c + i * 0x20 + 0x5000 + 0x2c4);
    *(int*)(c + 0x52c0 + i * 0x20) += *(int*)(c + i * 0x20 + 0x5000 + 0x2c8);

    if (*(unsigned short*)(c + i * 0x20 + 0x5200 + 0xce) != 0)
    {
        *(unsigned short*)(c + 0x52ce + i * 0x20) -= 1;
        {
            short s = *(short*)(c + i * 0x20 + 0x5200 + 0xce);
            if (s < 0)
            {
                s = 0;
                *(short*)(c + i * 0x20 + 0x5200 + 0xce) = s;
            }
        }
        return;
    }

    {
        unsigned char b = *(unsigned char*)(c + i * 0x20 + 0x5000 + 0x2d5);
        *(int*)(c + 0x52c4 + i * 0x20) += (b << 4) + 0x10;
    }

    if (*(unsigned short*)(c + i * 0x20 + 0x5200 + 0xd0) != 0)
    {
        *(unsigned short*)(c + 0x52d0 + i * 0x20) -= 1;
        {
            short s = *(short*)(c + i * 0x20 + 0x5200 + 0xd0);
            if (s < 0)
            {
                s = 0;
                *(short*)(c + i * 0x20 + 0x5200 + 0xd0) = s;
            }
        }
        return;
    }

    {
        unsigned r = (unsigned)RandomIntInternal(&data_0209d4b8);
        unsigned t = (r >> 16) & 0x7fff;
        t = (t << 4) >> 15;
        *(unsigned short*)(c + i * 0x20 + 0x5200 + 0xce) = (unsigned char)(t + 0x10);
        *(unsigned char*)(c + i * 0x20 + 0x5000 + 0x2da) = 3;
    }
}

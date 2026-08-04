#pragma opt_common_subs off
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;

void func_ov006_020ff690(char *c, int n)
{
    unsigned int r;
    int flag;

    flag = 0;
    r = (unsigned int)RandomIntInternal(&data_0209d4b8);
    if ((((r >> 16) & 0x7fff) << 1) >> 0xf)
    {
        flag = 1;
        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        if ((((r >> 16) & 0x7fff) << 2) >> 0xf)
        {
            *(char *)(c + (n << 5) + 0x5000 + 0x635) = 3;
            *(int *)(c + (n << 5) + 0x5000 + 0x628) = 0x1800;
        }
        else
        {
            *(char *)(c + (n << 5) + 0x5000 + 0x635) = 2;
            *(int *)(c + (n << 5) + 0x5000 + 0x628) = -0x1800;
        }
        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        *(char *)(c + (n << 5) + 0x5000 + 0x63c) = (char)((((r >> 16) & 0x7fff) * 6 >> 0xf) + 1);
    }
    else
    {
        *(char *)(c + (n << 5) + 0x5000 + 0x635) = 1;
        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        *(char *)(c + (n << 5) + 0x5000 + 0x63c) = (char)((((r >> 16) & 0x7fff) << 1) >> 0xf);
    }

    {
        unsigned int state = *(unsigned char *)(c + (n << 5) + 0x5000 + 0x63d);
        if (state != 0xff)
        {
            if (flag == state)
            {
                *(unsigned char *)(c + 0x563e + (n << 5)) += 1;
                if (*(unsigned char *)(c + (n << 5) + 0x5000 + 0x63e) >= 2)
                {
                    if (flag != 0)
                    {
                        *(char *)(c + (n << 5) + 0x5000 + 0x635) = 1;
                        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
                        *(char *)(c + (n << 5) + 0x5000 + 0x63c) = (char)((((r >> 16) & 0x7fff) << 1) >> 0xf);
                        flag = 0;
                    }
                    else
                    {
                        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
                        if ((((r >> 16) & 0x7fff) << 1) >> 0xf)
                        {
                            *(char *)(c + (n << 5) + 0x5000 + 0x635) = 3;
                            *(int *)(c + (n << 5) + 0x5000 + 0x628) = 0x1800;
                        }
                        else
                        {
                            *(char *)(c + (n << 5) + 0x5000 + 0x635) = 2;
                            *(int *)(c + (n << 5) + 0x5000 + 0x628) = -0x1800;
                        }
                        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
                        *(char *)(c + (n << 5) + 0x5000 + 0x63c) = (char)((((r >> 16) & 0x7fff) * 6 >> 0xf) + 1);
                        flag = 1;
                    }
                }
            }
            else
            {
                *(char *)(c + (n << 5) + 0x5000 + 0x63e) = 0;
            }
        }
    }

    *(char *)(c + (n << 5) + 0x5000 + 0x63d) = (char)flag;
    *(char *)(c + (n << 5) + 0x5000 + 0x636) = 0;
    *(char *)(c + (n << 5) + 0x5000 + 0x637) = 0x10;
    *(char *)(c + (n << 5) + 0x5000 + 0x638) = 0;
    *(short *)(c + (n << 5) + 0x5600 + 0x30) = 0;
    *(char *)(c + (n << 5) + 0x5000 + 0x63a) = 1;
}

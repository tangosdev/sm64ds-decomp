extern int data_020a0f5c;
extern int data_020a0f94;
extern unsigned short data_020a0f24;
extern int Wireless_Reset(void);
extern int func_0203faa8(void);

void func_0203fec4(unsigned short *p)
{
    int g;
    if (p[1] == 0)
    {
        g = data_020a0f5c;
        switch (g)
        {
        case 0:
        case 3:
            Wireless_Reset();
            return;
        case 4:
            switch (p[4])
            {
            case 7:
                data_020a0f24 = p[5];
                if (data_020a0f94 == 4)
                    return;
                if (func_0203faa8() != 0)
                    return;
                data_020a0f5c = 0;
                Wireless_Reset();
                return;
            case 9:
                data_020a0f5c = 0;
                if (data_020a0f94 == 2)
                    return;
                Wireless_Reset();
                return;
            case 6:
            case 8:
                break;
            default:
                data_020a0f5c = 0;
                Wireless_Reset();
                return;
            }
            break;
        default:
            return;
        }
    }
    else
    {
        data_020a0f5c = 0;
        Wireless_Reset();
    }
}

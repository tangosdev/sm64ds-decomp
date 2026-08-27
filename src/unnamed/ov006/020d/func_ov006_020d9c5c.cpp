//cpp
extern "C" {
    int _Z14ApproachLinearRiii(int* a, int b, int c);
    int _Z15ApproachLinear2Rsss(short* a, short b, short c);
    void _ZN5Sound12PlayBank2_2DEj(unsigned int n);
    int func_ov006_020da8e4(void);
    unsigned int func_02012790(unsigned int a);
    extern int data_ov006_0214176c;
    extern int data_ov006_02141768;
    extern unsigned char data_020a0e40;
    extern unsigned char data_020a0de8[];
    extern unsigned char data_020a0de9[];
    extern unsigned char data_020a0dea[][4];
    extern unsigned char data_020a0deb[][4];
}

struct VObj { virtual int f0(); virtual int f1(); virtual int f2(); };

extern "C" void func_ov006_020d9c5c(void* self)
{
    char* c = (char*)self;
    int r5;

    if (*(unsigned char*)(c + 0x2b) == 0)
        return;

    if (*(unsigned char*)(c + 0x2b) == 1) {
        if (*(short*)(c + 0x28) != 0) {
            _Z15ApproachLinear2Rsss((short*)(c + 0x28), 0, 1);
            return;
        }
        r5 = _Z14ApproachLinearRiii((int*)(c + 8), *(int*)(c + 0x10), *(int*)(c + 0x18));
        if (_Z14ApproachLinearRiii((int*)(c + 4), *(int*)(c + 0xc), *(int*)(c + 0x14)) != 0 && r5 != 0) {
            _ZN5Sound12PlayBank2_2DEj(0x144);
            *(unsigned char*)(c + 0x2b) = 2;
            *(unsigned char*)(c + 0x2c) = 1;
        }
    }

    if (*(unsigned char*)(c + 0x2c) != 0)
        _Z14ApproachLinearRiii((int*)(c + 0x24), 0x4000, 0x200);
    else
        _Z14ApproachLinearRiii((int*)(c + 0x24), 0, 0x200);

    {
        int st = *(unsigned char*)(c + 0x2b);
        if (st == 3) {
            _Z14ApproachLinearRiii((int*)(c + 8), *(int*)(c + 0x10) - 0x10000, 0x2000);
        } else if (st == 5) {
            r5 = _Z14ApproachLinearRiii((int*)(c + 8), *(int*)(c + 0x10), *(int*)(c + 0x18));
            if (_Z14ApproachLinearRiii((int*)(c + 4), *(int*)(c + 0xc), *(int*)(c + 0x14)) != 0 && r5 != 0) {
                _ZN5Sound12PlayBank2_2DEj(0x144);
                *(unsigned char*)(c + 0x2b) = 2;
                *(unsigned char*)(c + 0x2c) = 1;
            }
        } else if (st == 2) {
            _Z14ApproachLinearRiii((int*)(c + 8), *(int*)(c + 0x10), 0x4000);
        } else if (st == 4 && *(int*)(c + 0x24) == 0) {
            int result = ((VObj*)c)->f2();
            if (_Z14ApproachLinearRiii((int*)(c + 8), result, 0x8000) != 0) {
                *(int*)(c + 4) = 0x80000;
                *(unsigned char*)(c + 0x2b) = 6;
                *(unsigned char*)(c + 0x2a) = (unsigned char)func_ov006_020da8e4();
            }
        } else if (st == 7) {
            if (_Z14ApproachLinearRiii((int*)(c + 4), *(int*)(c + 0x1c), 0x6000) != 0)
                *(unsigned char*)(c + 0x2b) = 8;
        } else if (st == 9) {
            _Z14ApproachLinearRiii((int*)(c + 8), 0x80000, 0x6000);
        } else if (st == 0xa) {
            if (_Z15ApproachLinear2Rsss((short*)(c + 0x28), 0, 1) != 0)
                _Z14ApproachLinearRiii((int*)(c + 4), *(int*)(c + 0x1c), *(int*)(c + 0x14));
        }
    }

    if (data_ov006_0214176c != 0) {
        if (*(unsigned char*)(c + 0x2b) == 3) {
            *(unsigned char*)(c + 0x2b) = 4;
            *(unsigned char*)(c + 0x2c) = 0;
        }
        return;
    }
    if (data_ov006_02141768 != 0)
        return;

    {
        int idx = data_020a0e40;
        unsigned char e8;
        int flag = 0;
        e8 = data_020a0de8[idx * 4];
        if (e8 != 0 && data_020a0de9[idx * 4] != 0)
            flag = 1;
        if (flag == 0)
            return;
        {
            unsigned char st = *(unsigned char*)(c + 0x2b);
            if ((unsigned char)(st + 0xfe) > 1)
                return;
            if (*(int*)(c + 0x24) != 0x4000)
                return;
            {
                int a = (int)data_020a0dea[idx][0] - (*(int*)(c + 4) >> 12);
                int b = (int)data_020a0deb[idx][0] - (*(int*)(c + 8) >> 12);
                if (a <= 7)
                    return;
                if (a >= 0x29)
                    return;
                if (b <= -1)
                    return;
                if (b >= 0x31)
                    return;
                if (st == 2) {
                    func_02012790(0x14f);
                    *(unsigned char*)(c + 0x2b) = 3;
                } else {
                    func_02012790(0x150);
                    *(unsigned char*)(c + 0x2b) = 2;
                }
            }
        }
    }
}

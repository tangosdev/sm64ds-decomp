    extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);
    extern int RandomIntInternal(int* seed);
    extern int data_0209d4b8;

    struct E { char pad[0x1c]; };

    #pragma opt_common_subs off
    void func_ov006_020fb4e0(char* c, int idx)
    {
        int off;
        short* p;
        unsigned int v;
        off = idx * 0x1c;
        p = (short*)((char*)&((struct E*)(c + 0x5bc4))[idx]);
        if (*(unsigned short*)p != 0) {
            *p = *(unsigned short*)p - 1;
            if (*p < 0) *p = 0;
            return;
        }
        _ZN5Sound12PlayBank2_2DEj(0x188);
        *(char*)(c + off + 0x5000 + 0xbc8) = 1;
        v = (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 1;
        v = v >> 0xf;
        *(char*)(c + off + 0x5000 + 0xbc9) = v;
        if (v != 0) {
            *(char*)(c + off + 0x5000 + 0xbc7) = 1;
            *(int*)(c + off + 0x5000 + 0xbb0) = -0x10000;
            *(int*)(c + off + 0x5000 + 0xbb8) = 0x400;
        } else {
            *(char*)(c + off + 0x5000 + 0xbc7) = 1;
            *(int*)(c + off + 0x5000 + 0xbb0) = 0x110000;
            *(int*)(c + off + 0x5000 + 0xbb8) = -0x400;
        }
        *(int*)(c + off + 0x5000 + 0xbb4) = -0xf8000;
        *(char*)(c + off + 0x5000 + 0xbca) = ((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3 >> 0xf) + 2;
        *(int*)(c + off + 0x5000 + 0xbbc) = 0x2000;
    }

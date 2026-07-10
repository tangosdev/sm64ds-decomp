//cpp
extern "C" {
extern short data_02082214[];
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void*, int, int);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void*);

int _ZN11PyramidLift8BehaviorEv(char* c)
{
    switch (*(unsigned char*)(c + 0x3f6)) {
    case 0:
        if (*(unsigned char*)(c + 0x3f7) != 0) {
            *(unsigned char*)(c + 0x3f6) = 1;
            *(short*)(c + 0x3f4) = 0;
        }
        break;
    case 1: {
        unsigned short ang = *(unsigned short*)(c + 0x3f4);
        int t = (int)((unsigned short)((int)(ang << 0x1c) >> 0x10));
        int idx = t >> 4;
        int s = *(short*)((char*)data_02082214 + (idx << 2));
        int d = (int)(((long long)s * 0xa + 0x800) >> 0xc);
        *(int*)(c + 0x60) = *(int*)(c + 0x374) + d;
        if (*(unsigned short*)(c + 0x3f4) == 8) {
            *(unsigned char*)(c + 0x3f6) = 2;
            *(int*)(c + 0xa8) = -0xa000;
        }
        {
            unsigned short *pa = (unsigned short*)(((int)c + 0x3f4) & 0xFFFFFFFFFFFFFFFFLL);
            *pa = *pa + 1;
        }
        break;
    }
    case 2: {
        int v = *(int*)(c + 0x60);
        int idx = *(unsigned char*)(c + 0x3f8);
        int* p = (int*)(c + idx * 0xc + 0x380);
        int lim = *p + 0x14000;
        if (v <= lim) {
            unsigned char *pb = (unsigned char*)(((int)c + 0x3f8) & 0xFFFFFFFFFFFFFFFFLL);
            *pb = *pb + 1;
        }
        {
            int *py = (int*)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFFLL);
            *py = *py + *(int*)(c + 0xa8);
        }
        if (*(int*)(c + 0x60) < 0x80000) {
            *(int*)(c + 0x60) = 0x80000;
            *(unsigned char*)(c + 0x3f6) = 3;
            *(short*)(c + 0x3f4) = 0;
        }
        break;
    }
    case 3: {
        int z = 0;
        unsigned short ang = *(unsigned short*)(c + 0x3f4);
        int t = (int)((unsigned short)((int)(ang << 0x1c) >> 0x10));
        int idx = t >> 4;
        int s = *(short*)((char*)data_02082214 + (idx << 2));
        int d = (int)(((long long)s * 0xa + 0x800) >> 0xc);
        *(int*)(c + 0x60) = d + 0x80000;
        {
            unsigned short *pa = (unsigned short*)(((int)c + 0x3f4) & 0xFFFFFFFFFFFFFFFFLL);
            if (*(unsigned short*)(c + 0x3f4) >= 8) {
                *(int*)(c + 0xa8) = z;
                *(int*)(c + 0x60) = 0x80000;
            }
            *pa = *pa + 1;
        }
        break;
    }
    }
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0))
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    *(unsigned char*)(c + 0x3f7) = 0;
    return 1;
}
}

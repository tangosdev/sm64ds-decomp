//cpp
extern signed short data_02082214[];
extern "C" unsigned char DecIfAbove0_Byte(unsigned char*);
extern "C" void func_ov002_020b9704(void*, int);
int ApproachLinear(int&, int, int);
namespace cstd { int fdiv(int, int); }

extern "C" void func_ov002_020b92c4(void* c) {
    if (DecIfAbove0_Byte((unsigned char*)((char*)c + 0x3cb))) {
        int a = (int)(*(unsigned char*)((char*)c + 0x3cb)) << 12;
        int fdivResult = cstd::fdiv(a, 0x1c000);
        unsigned short hw = *(unsigned short*)((char*)c + 0x3c8);
        int idx = hw >> 4;
        signed short odd = data_02082214[idx * 2 + 1];
        int t1 = (int)(((long long)odd * fdivResult + 0x800) >> 12);
        int u1 = (int)(((long long)t1 * 0x332 + 0x800) >> 12);
        int v1 = u1 + 0xffa;
        int w1 = (int)(((long long)v1 * 0xfa0 + 0x800) >> 12);
        *(int*)((char*)c + 0x84) = w1;
        signed short even = data_02082214[idx * 2];
        int t2 = (int)(((long long)even * fdivResult + 0x800) >> 12);
        int u2 = (int)(((long long)t2 * 0x332 + 0x800) >> 12);
        int v2 = u2 + 0xffa;
        int w2 = (int)(((long long)v2 * 0xfa0 + 0x800) >> 12);
        *(int*)((char*)c + 0x80) = w2;
        // materialized base for halfword at offset >= 0x100
        short* p = (short*)(((int)c + 0x3c8) & 0xFFFFFFFFFFFFFFFF);
        *p = *p + 0x2000;
    } else {
        int ret = ApproachLinear(*(int*)((char*)c + 0x84), 0xfa0, 0x199);
        if (ret) {
            func_ov002_020b9704(c, 2);
        }
    }
}

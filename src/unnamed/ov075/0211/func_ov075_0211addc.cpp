//cpp
typedef long long s64;
extern "C" short func_ov075_0211aeb8(char* c, int x);
extern "C" int func_ov075_0211aea4(int unused, int a);
namespace cstd { int fdiv(int, int); }

extern short data_02082214[];

extern "C" int func_ov075_0211addc(char *self, int x)
{
    int cnt = *(unsigned short*)(self + 0xa2);
    int b, d, v, r2, idx, t;
    short a;
    if (cnt == 0)
        return 0;
    if (x > (0x64 - cnt) * 0x18444)
        return 0;
    a = func_ov075_0211aeb8(self, x);
    b = func_ov075_0211aea4((int)self, 0x64 - *(unsigned short*)(self + 0xa2));
    d = cstd::fdiv(b, 0x384000);
    v = (int)((((s64)(-d) * x) + 0x800) >> 12);
    r2 = b + v;
    if (r2 < 0)
        r2 = 0;
    idx = (unsigned short)a >> 4;
    t = *(short*)((char*)data_02082214 + idx * 4);
    return (int)((((s64)t * r2) + 0x800) >> 12);
}

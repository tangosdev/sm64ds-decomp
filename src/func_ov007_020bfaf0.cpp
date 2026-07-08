//cpp
typedef int Fix12i;
namespace cstd { int fdiv(int a, int b); }
extern "C" void func_ov007_020bc02c(void* a, void* b);
extern "C" void func_ov007_020bbff0(void* c);
extern void* data_ov007_02104bd8;
extern const short SINE_TABLE[];

extern "C" void func_ov007_020bfaf0(char* c)
{
    char* r4 = *(char**)((char*)*(void**)&data_ov007_02104bd8 + 4);
    if (*(int*)(r4 + 0x94) == 0) return;
    unsigned int idx = (unsigned short)*(unsigned short*)(c + 0xd4) >> 4;
    Fix12i t1 = (int)(((long long)SINE_TABLE[idx * 2] * 0x1064 + 0x800) >> 12);
    Fix12i base = *(int*)(r4 + 0x40) * 0xd800 + 0x12c000;
    Fix12i t2 = (int)(((long long)base * t1 + 0x800) >> 12);
    int denom = (short)SINE_TABLE[idx * 2 + 1] * 0x60;
    Fix12i d = cstd::fdiv(t2, denom);
    *(int*)(r4 + 0x48) = cstd::fdiv(d, 0x5000);
    *(int*)(r4 + 0x40) = *(int*)(r4 + 0x48);
    func_ov007_020bc02c(r4, c + 0x44);
    func_ov007_020bbff0(r4);
}

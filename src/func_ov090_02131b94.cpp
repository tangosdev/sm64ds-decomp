//cpp
typedef int Fix12i;
struct BCA_File;
struct ModelAnim {
    int dummy;
};
extern "C" int RandomIntInternal(int* seed);
extern int RNG_STATE;

struct Data134490 { int a; BCA_File* bca; };
extern Data134490 data_ov090_02134490;

extern "C" unsigned int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    ModelAnim* thiz, BCA_File* f, int a, Fix12i b, unsigned int c);

extern "C" int func_ov090_02131b94(char* thiz)
{
    char* c = thiz;
    unsigned int r;
    short* s;
    r = (unsigned)RandomIntInternal(&RNG_STATE);
    s = (short*)(((unsigned int)c + 0x39a) & 0xFFFFFFFFFFFFFFFFull);
    *s = (short)(*s + ((int)(((r >> 8) & 3) << 0x1e) >> 16));
    r = (unsigned)RandomIntInternal(&RNG_STATE);
    s = (short*)(((long long)(int)(c + 0x39a)) & 0xFFFFFFFFFFFFFFFFll);
    *s = (short)(*s + ((int)(((r >> 8) & 7) << 0x1d) >> 16));
    r = (unsigned)RandomIntInternal(&RNG_STATE);
    *(short*)(c + 0x100) = (short)(((r >> 8) & 0x1f) + 0x96);
    *(int*)(c + 0x3a4) = 0x1000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        (ModelAnim*)(c + 0x30c), data_ov090_02134490.bca, 0, 0x1000, 0);
    return 1;
}

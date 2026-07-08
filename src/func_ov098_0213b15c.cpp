//cpp
extern "C" {
int RandomIntInternal(int* seed);
int AngleDiff(int a, int b);
}
extern int RNG_STATE;

extern "C" void func_ov098_0213b15c(char* c)
{
    int a;
    *(short*)(c + 0x178) = (short)(((unsigned int)RandomIntInternal(&RNG_STATE) >> 16 & 0xfff) + 0x1000);
    a = *(short*)(c + 0x8e);
    do {
        *(short*)(c + 0x17a) = (short)(*(short*)(c + 0x17c) + 0xe000 + ((unsigned int)RandomIntInternal(&RNG_STATE) >> 16 & 0x3fff));
    } while (AngleDiff(a, *(short*)(c + 0x17a)) < 0x800);
}

//cpp
struct C;
typedef void (C::*PMF)();

extern "C" void func_ov075_0211a194(char* self, int* src)
{
    int a0 = src[0];
    int a1 = src[1];
    a1 = a1 ? a1 : a1;
    *(int*)(self + 0x54) = a0;
    *(int*)(self + 0x58) = a1;
    int a2 = src[2];
    int a3 = src[3];
    a3 = a3 ? a3 : a3;
    *(int*)(self + 0x5c) = a2;
    *(int*)(self + 0x60) = a3;
    int a4 = src[4];
    int a5 = src[5];
    a5 = a5 ? a5 : a5;
    *(int*)(self + 0x64) = a4;
    *(int*)(self + 0x68) = a5;
    if (*(int*)(self + 0x54) != 0)
    {
        PMF* p = (PMF*)(self + 0x54);
        C* o = (C*)self;
        (o->**p)();
    }
}

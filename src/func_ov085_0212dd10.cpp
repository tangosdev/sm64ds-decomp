//cpp
extern "C" {
char* _ZN8dActor_c13ClosestPlayerEv(void);
void func_ov002_020c3f18(char* p);
void _Z14ApproachLinearRsss(short&, short, short);
void func_ov085_0212e728(char* c, void* p);
extern char data_ov085_021307c0[];

int func_ov085_0212dd10(char* c)
{
    char* p = _ZN8dActor_c13ClosestPlayerEv();
    if (!p) return 1;
    {
        short v = *(short*)(c + 0x8e);
        short w = v + 0x8000;
        int* t = (int*)(((int)c + 0x2c8));
        *(short*)(p + 0x8c) = 0;
        *(short*)(p + 0x8e) = w;
        *(short*)(p + 0x90) = 0;
        *t += 1;
    }
    if (*(int*)(c + 0x2c8) == 0x5a) func_ov002_020c3f18(p);
    {
        int s = *(int*)(c + 0x2c8);
        if (s > 0x57 && s < 0x5b)
            _Z14ApproachLinearRsss(*(short*)(c + 0x8c), 0x2000, 0x400);
        else
            _Z14ApproachLinearRsss(*(short*)(c + 0x8c), 0x1000, 0x400);
    }
    if (*(int*)(c + 0x2c8) > 0x78)
        func_ov085_0212e728(c, data_ov085_021307c0);
    return 1;
}
}

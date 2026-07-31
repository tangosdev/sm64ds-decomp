//cpp
struct Sound { static void PlayBank2_2D(unsigned int); };
struct C;
typedef void (C::*PMF)();
struct C {
    char pad10[0x10];
    PMF pmf;        // 0x10
};

extern "C" void func_ov006_020eb31c(char* c)
{
    short* t = (short*)(((int)c + 0x90));
    short v = (short)(*t - 1);
    *t = v;
    if (*(short*)(c+0x90) == 0) {
        *(unsigned char*)(c+0x94) = 0;
        *(unsigned char*)(c+0x93) = 1;
        *(int*)(c+0x44) = 0x1000;
        *(short*)(c+0x86) = 0x900;
        *(unsigned char*)(c+0x95) = 4;
        *(short*)(c+0x8e) = 0x78;
        {
            int a = *(int*)(c+0x10);
            int b = *(int*)(c+0x14);
            *(int*)c = b ? a : a;
            *(int*)(c+4) = b;
        }
        return;
    }
    C* o = (C*)c;
    (o->*(o->pmf))();
    if (*(short*)(c+0x90) != 0x30) return;
    if (*(int*)(c+0x70) == 0)
        Sound::PlayBank2_2D(0x1ea);
    *(unsigned char*)(c+0x95) = 3;
}

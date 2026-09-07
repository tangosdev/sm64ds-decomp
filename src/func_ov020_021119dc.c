    struct dCc_c;
    extern void func_0203568c(int* p, int v);
    extern void _Z14ApproachLinearRiii(int* v, int a, int b);
    extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void* a, struct dCc_c* c);
    
    void func_ov020_021119dc(void* thiz)
    {
        char* c = (char*)thiz;
        func_0203568c((int*)(c + 0x25c), 0x64000);
        if (*(unsigned short*)(c + 0x104) != 0) {
            unsigned short* p = (unsigned short*)(((int)c + 0x104));
            *p = (unsigned short)(*p - 1);
            if (*(unsigned short*)(c + 0x104) != 0) {
                int* q = (int*)(((int)c + 0x234));
                *q = *q | 1;
            } else {
                int* q = (int*)(((int)c + 0x234));
                *q = *q & ~1;
            }
        }
        _Z14ApproachLinearRiii((int*)(c + 0xa8), 0, 0x800);
        _Z14ApproachLinearRiii((int*)(c + 0x98), 0, 0x800);
        if (*(int*)(c + 0xa8) == 0 && *(int*)(c + 0x98) == 0) {
            func_0203568c((int*)(c + 0x25c), 0x32000);
            *(int*)(c + 0x424) = *(int*)(c + 0x428);
            if (*(int*)(c + 0x424) == 3)
                *(int*)(c + 0x424) = 2;
        }
        _ZN8dActor_c9UpdatePosEP5dCc_c(c, 0);
    }

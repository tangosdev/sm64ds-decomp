//cpp
extern "C" void func_ov096_021372c0(void);
extern "C" void func_ov096_02137088(void);
extern "C" void func_ov096_02136fd4(void);
extern "C" void *_ZN5Actor10FindWithIDEj(unsigned int id);
extern "C" void *_ZN5Actor18ClosestWithActorIDEj(void *actor, unsigned int id);
extern "C" int Vec3_Dist(void *a, void *b);
extern "C" int func_ov002_020de33c(char *c, int a);
extern "C" void _ZN12CylinderClsn5ClearEv(void *thiz);
extern "C" void _ZN12CylinderClsn6UpdateEv(void *thiz);
extern "C" void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern "C" void _ZN9Animation7AdvanceEv(void *thiz);

extern "C" int _ZN7Tornado8BehaviorEv(char *c)
{
    int s = *(int*)(c + 0x35c);
    switch (s) {
    case 0: func_ov096_021372c0(); break;
    case 1: func_ov096_02137088(); break;
    case 2: func_ov096_02136fd4(); break;
    }
    {
        unsigned short *p = (unsigned short*)(((int)c + 0x350) & 0xFFFFFFFFFFFFFFFF);
        *p = *p + 1;
    }
    if (s != *(int*)(c + 0x35c)) {
        *(unsigned short*)(c + 0x350) = 0;
        *(unsigned char*)(c + 0x360) = 0;
    }
    unsigned int id = *(unsigned int*)(c + 0xf8);
    if (id != 0 && (*(int*)(c + 0xf4) & 0x400000) != 0) {
        void *o = _ZN5Actor10FindWithIDEj(id);
        if (o != 0) {
            void *closest = _ZN5Actor18ClosestWithActorIDEj(c, 0x135);
            if (closest == 0 || Vec3_Dist((char*)o + 0x5c, (char*)closest + 0x5c) > 0x118000) {
                if (func_ov002_020de33c((char*)o, (int)c) != 0) {
                    *(void**)(c + 0x33c) = o;
                }
            }
        }
    }
    _ZN12CylinderClsn5ClearEv(c + 0xd4);
    _ZN12CylinderClsn6UpdateEv(c + 0xd4);
    Matrix4x3_FromTranslation(c + 0x2e0, *(int*)(c + 0x5c) >> 3, *(int*)(c + 0x60) >> 3, *(int*)(c + 0x64) >> 3);
    _ZN9Animation7AdvanceEv(c + 0x314);
    _ZN9Animation7AdvanceEv(c + 0x328);
    return 1;
}

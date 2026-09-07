//cpp
extern "C" int _ZNK10dBgCh_Actr10IsOnGroundEv(void *c);
extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int a, int b, int c, int d);
extern "C" void _ZN7fBase_c18MarkForDestructionEv(void *thiz);
extern "C" void func_02012694(int a0, void *a1);
extern "C" void func_ov081_02125fb8(void *c);

extern "C" int func_ov081_021260fc(char *thiz)
{
    if (*(int*)(thiz + 0x388) == 0 && *(int*)(thiz + 0xa0) == -0x3c000) {
        if (*(int*)(thiz + 0x380) > *(int*)(thiz + 0x60)) {
            *(int*)(thiz + 0xa8) = 0;
            *(int*)(thiz + 0x9c) = 0;
            *(int*)(thiz + 0x388) = 1;
        }
    }
    if (*(unsigned short*)(thiz + 0x100) == 0 ||
        _ZNK10dBgCh_Actr10IsOnGroundEv(thiz + 0x144) != 0) {
        func_02012694(0x3c, thiz + 0x74);
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
            0x11c, *(int*)(thiz + 0x5c), *(int*)(thiz + 0x60), *(int*)(thiz + 0x64));
        _ZN7fBase_c18MarkForDestructionEv(thiz);
    }
    if (*(int*)(thiz + 0xa4) != 0 || *(int*)(thiz + 0xac) != 0) {
        func_ov081_02125fb8(thiz);
    }
    return 1;
}

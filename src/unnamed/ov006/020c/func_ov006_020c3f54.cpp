//cpp
extern "C" {
extern void* func_020733a8(void* a, int b, int c, void* d, void* e);
extern void _ZN9ModelAnimC1Ev(void* t);
extern void* func_02017acc(void* t, unsigned int n);
extern void _ZN13SharedFilePtr9ConstructEj(void* t, unsigned int n);
extern void func_ov006_020c3e54();
extern void func_ov006_020c402c();
}

extern "C" void* func_ov006_020c3f54(char* c)
{
    func_020733a8(c + 8, 0x16, 0x98, (void*)func_ov006_020c402c, (void*)func_ov006_020c3e54);
    _ZN9ModelAnimC1Ev(c + 0xd18);
    func_02017acc(c + 0xd7c, 0x222);
    func_02017acc(c + 0xd84, 0x201);
    _ZN13SharedFilePtr9ConstructEj(c + 0xd8c, 0x224);
    _ZN13SharedFilePtr9ConstructEj(c + 0xd94, 0x223);
    _ZN13SharedFilePtr9ConstructEj(c + 0xd9c, 0x221);
    _ZN13SharedFilePtr9ConstructEj(c + 0xda4, 0x220);
    return c;
}

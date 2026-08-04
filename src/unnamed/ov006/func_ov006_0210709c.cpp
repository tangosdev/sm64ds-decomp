//cpp
struct C;
typedef void (C::*PMF)(int);

extern PMF data_ov006_02142840[];

extern "C" {
    void func_ov006_02105de4(C* c);
    void func_ov006_021050bc(C* c);
    void func_ov006_02105d20(C* c);
    void func_ov006_02105c88(C* c);
    void func_ov006_02105134(C* c);
}

extern "C" void func_ov006_0210709c(C* c)
{
    int i;
    char* b = (char*)c;
    func_ov006_02105de4(c);
    func_ov006_021050bc(c);
    *(unsigned char*)(b + 0x4fe9) = 0;
    for (i = 0; i < *(int*)(b + 0x4cb8); i++) {
        unsigned char idx = *(unsigned char*)(b + i + 0x4efa);
        (c->*data_ov006_02142840[idx])(i);
    }
    func_ov006_02105d20(c);
    func_ov006_02105c88(c);
    func_ov006_02105134(c);
}

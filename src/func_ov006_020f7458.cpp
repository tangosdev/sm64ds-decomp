//cpp
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern "C" Entry data_ov006_021423e0[];
struct C { char pad[0x53d4]; int idx; };
extern "C" void func_ov004_020b65e4(void);
extern "C" int func_ov006_020c19d0(void* p);
extern "C" int func_ov006_020f7458(C* c) {
    (c->*(data_ov006_021423e0[c->idx].pmf))();
    func_ov004_020b65e4();
    func_ov006_020c19d0((char*)c + 0x4f38);
    return 1;
}

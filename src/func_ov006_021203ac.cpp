//cpp
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern Entry data_ov006_02142eb0[];
extern "C" int func_ov006_0211e4e0(C* c);
extern "C" int func_ov006_021203ac(C* c) {
    (c->*data_ov006_02142eb0[ *(int*)((char*)c + 0x4be8) ].pmf)();
    func_ov006_0211e4e0(c);
    return 1;
}

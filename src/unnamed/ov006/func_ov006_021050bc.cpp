//cpp
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern "C" Entry data_ov006_02142860[];
struct C { char pad[0x4674]; unsigned char guard; unsigned char idx; };
extern "C" void func_ov006_021050bc(C* c) {
    if (!c->guard) return;
    (c->*(data_ov006_02142860[c->idx].pmf))();
}

//cpp
struct C; typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };
extern "C" Entry data_ov006_02142604[];
struct C { char pad[0x5c0e]; unsigned char guard; unsigned char idx; };
extern "C" void func_ov006_020fa844(C* c);
extern "C" void func_ov006_020fad34(C* c) {
    if (c->guard) {
        (c->*(data_ov006_02142604[c->idx].pmf))(0);
    }
    func_ov006_020fa844(c);
}

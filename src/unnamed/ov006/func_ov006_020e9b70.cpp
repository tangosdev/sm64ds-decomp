//cpp
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern Entry data_ov006_02141fac[];
struct C { char pad[0x5540]; int idx; };
extern "C" int func_ov006_020e82fc(C* c);
extern "C" int func_ov006_020e9b70(C* c) {
    int j = c->idx;
    (c->*data_ov006_02141fac[j].pmf)();
    return func_ov006_020e82fc(c);
}

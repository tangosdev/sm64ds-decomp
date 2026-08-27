//cpp
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern Entry data_ov006_02142440[];
struct C { char pad[0x53d8]; int idx; };
extern "C" int func_ov006_020f5c40(C* c);
extern "C" int func_ov006_020f7234(C* c) {
    int j = c->idx;
    (c->*data_ov006_02142440[j].pmf)();
    return func_ov006_020f5c40(c);
}

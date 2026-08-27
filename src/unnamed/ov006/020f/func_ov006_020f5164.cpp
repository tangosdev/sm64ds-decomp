//cpp
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern Entry data_ov006_02142304[];
struct C { char pad[0x5318]; int idx; };
extern "C" int func_ov006_020f3f10(C* c);
extern "C" int func_ov006_020f5164(C* c) {
    int j = c->idx;
    (c->*data_ov006_02142304[j].pmf)();
    return func_ov006_020f3f10(c);
}

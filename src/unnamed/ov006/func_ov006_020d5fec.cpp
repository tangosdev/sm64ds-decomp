//cpp
struct C; typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };
extern "C" Entry data_ov006_02141660[];
struct C { char pad[0x62ac]; unsigned char idx; unsigned char guard; };
extern "C" void func_ov006_020d5fec(C* c) {
    if (!c->guard) return;
    (c->*(data_ov006_02141660[c->idx].pmf))(0);
}

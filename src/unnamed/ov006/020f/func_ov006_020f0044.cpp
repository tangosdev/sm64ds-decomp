//cpp
struct C;
typedef void (C::*PMF)(int);
struct Entry { PMF pmf[1]; };
extern "C" {
extern Entry data_ov006_021421ec[];
}
struct C {
    char pad[0x47f4];
    unsigned char guard;
    unsigned char idx;
};
extern "C" void func_ov006_020f0044(C *c)
{
    if (c->guard == 0) return;
    int j = c->idx;
    (c->*data_ov006_021421ec[j].pmf[0])(0);
}

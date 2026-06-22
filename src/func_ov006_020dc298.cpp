//cpp
struct C;
typedef void (C::*PMF)(int);
struct Entry { PMF pmf[1]; };
extern "C" {
extern Entry data_ov006_021417c8[];
}
struct C {
    char pad[0x51bc];
    unsigned char guard;
    char pad2[2];
    unsigned char idx;
};
extern "C" void func_ov006_020dc298(C *c)
{
    if (c->guard == 0) return;
    int j = c->idx;
    (c->*data_ov006_021417c8[j].pmf[0])(0);
}

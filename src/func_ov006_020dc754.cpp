//cpp
struct C;
typedef void (C::*PMF)(int);
struct Entry { PMF pmf[1]; };
extern "C" {
extern Entry data_ov006_021417e8[];
}
struct C {
    char pad[0x51a0];
    unsigned char guard;
    unsigned char idx;
};
extern "C" void func_ov006_020dc754(C *c)
{
    if (c->guard == 0) return;
    int j = c->idx;
    (c->*data_ov006_021417e8[j].pmf[0])(0);
}

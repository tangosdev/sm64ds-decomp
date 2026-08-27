//cpp
struct C;
typedef void (C::*PMF)();
struct C {
    char pad[0x10];
    PMF pmf;
    int field18;
};
extern "C" void func_ov004_020b8714(C* c) {
    if (c->field18 == -1) return;
    if (c->pmf == 0) return;
    (c->*(c->pmf))();
}

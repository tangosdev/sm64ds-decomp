//cpp
struct C;
typedef void (C::*PMF)();
struct C {
    PMF pmf;
    char pad[0x18];
    int state;
};
extern "C" void func_ov004_020b321c(C* c) {
    if (c->state == 0x1d) return;
    if (c->pmf == 0) return;
    (c->*(c->pmf))();
}

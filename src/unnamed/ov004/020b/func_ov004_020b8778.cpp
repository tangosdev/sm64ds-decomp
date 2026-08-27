//cpp
struct C;
typedef void (C::*PMF)();
struct C {
    char pad[8];
    PMF pmf;
    char pad2[8];
    int field18;
    int field1c;
};
extern "C" {
void _Z14ApproachLinearRiii(int&, int, int);
void func_ov004_020b8778(C* c);
}
void func_ov004_020b8778(C* c) {
    if (c->field18 == -1) return;
    _Z14ApproachLinearRiii(c->field1c, 0, 1);
    if (c->pmf == 0) return;
    (c->*(c->pmf))();
}

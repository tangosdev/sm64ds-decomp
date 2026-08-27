//cpp
struct C;
typedef void (C::*PMF)();
struct C {
    char pad[8];
    PMF pmf;
    char pad2[0x10];
    int state;
};
extern "C" {
void func_ov004_020b42c0(void);
void func_ov004_020b31b4(C* c);
}
void func_ov004_020b31b4(C* c) {
    if (c->state == 0x1d) return;
    if (c->pmf != 0) {
        (c->*(c->pmf))();
        return;
    }
    func_ov004_020b42c0();
}

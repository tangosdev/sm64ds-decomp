//cpp
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern Entry data_ov006_02141810[];
struct C { char pad[4]; };
extern "C" {
void func_ov006_020dc754(void *c);
void func_ov006_020dc298(void *c);
void func_ov006_020dc900(void *c);
void func_ov006_020dce3c(void *c);
int func_ov006_020de69c(C *c);
}
int func_ov006_020de69c(C *c) {
    int idx = *(int *)((char *)c + 0x51c8);
    (c->*data_ov006_02141810[idx].pmf)();
    func_ov006_020dc754(c);
    func_ov006_020dc298(c);
    func_ov006_020dc900(c);
    func_ov006_020dce3c(c);
    return 1;
}

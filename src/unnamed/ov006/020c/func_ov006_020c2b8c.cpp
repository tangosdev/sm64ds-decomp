//cpp
extern "C" {
struct C;
typedef void (C::*PMF)();
extern int func_ov006_020c2be8(C* c);
extern void _ZN9Animation7AdvanceEv(char* a);
extern void _ZN14BlendModelAnim7AdvanceEv(char* b);
extern int func_ov006_020c2290(C* c);
struct C { PMF pmf; };
void func_ov006_020c2b8c(C* c) {
    func_ov006_020c2be8(c);
    if (c->pmf) (c->*(c->pmf))();
    _ZN9Animation7AdvanceEv((char*)c + 0xc8);
    _ZN9Animation7AdvanceEv((char*)c + 0xdc);
    _ZN14BlendModelAnim7AdvanceEv((char*)c + 8);
    func_ov006_020c2290(c);
}
}

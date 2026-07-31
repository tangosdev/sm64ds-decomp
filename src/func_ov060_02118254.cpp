//cpp
// @symbol func_ov060_02118254
// @emits daKpa3Bg_c_Behavior
/* recovered: renamed to Class_Method */
/* daKpa3Bg_c::Behavior - recovered from vtable slot identity */
struct C;
typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern Entry data_ov060_0211b1ac[];
extern "C" void _ZN8Platform21UpdateModelPosAndRotYEv(void* p);
extern "C" void _ZN8Platform19UpdateClsnPosAndRotEv(void* p);
struct C {
    unsigned char pad[0x328];
    unsigned char idx;
    unsigned char pad2[2];
    unsigned char flag;
};
extern "C" int daKpa3Bg_c_Behavior(C* c) {
    (c->*(data_ov060_0211b1ac[c->idx].pmf))();
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    c->flag = 0;
    return 1;
}

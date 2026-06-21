//cpp
struct Obj;
typedef void (Obj::*PMF)();
extern PMF data_ov006_02142234[];

extern "C" int func_ov006_020f3414(Obj *r0) {
    int idx = *(int *)((char *)r0 + 0x4000 + 0xf78);
    (r0->*data_ov006_02142234[idx])();
    return 1;
}

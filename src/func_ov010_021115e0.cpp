//cpp
// @symbol func_ov010_021115e0
// recovered name: daObjC1_Trap_c_Behavior
/* recovered: renamed to Class_Method */
/* daObjC1_Trap_c::Behavior - recovered from vtable slot identity */
struct C; typedef void (C::*PMF)();
extern "C" int _ZN5Actor13DistToCPlayerEv(void* a);
extern "C" void func_ov010_0211146c(void* c);
extern "C" void func_ov010_021113f0(void* c);
extern PMF data_ov010_02112d28[];

extern "C" int func_ov010_021115e0(C* c);
extern "C" int func_ov010_021115e0(C* c)
{
    char* p = (char*)c;
    if (*(unsigned char*)(p + 0x3ab)) {
        *(int*)(p + 0x3a4) = _ZN5Actor13DistToCPlayerEv(c);
        return 1;
    }
    (c->*data_ov010_02112d28[*(int*)(p + 0x3a0)])();
    func_ov010_0211146c(c);
    func_ov010_021113f0(c);
    return 1;
}

//cpp
// @symbol _ZN4Trap8BehaviorEv
/* recovered: renamed to Class_Method
 *
 * Trap::Behavior -- vtable slot 6, ov010 0x021115e0. Attributed by the
 * vtable: _ZTV4Trap carries this address at slot 6, where _ZTV10dBgActor_c
 * carries fBase_c's generic 0x02043b24.
 *
 * Kept as an extern "C" free function under the literal mangled name rather
 * than a real method -- same shape as _ZN4Trap13InitResourcesEv.c. */
struct C; typedef void (C::*PMF)();
extern "C" int _ZN8dActor_c13DistToCPlayerEv(void* a);
extern "C" void func_ov010_0211146c(void* c);
extern "C" void func_ov010_021113f0(void* c);
extern PMF data_ov010_02112d28[];

extern "C" int _ZN4Trap8BehaviorEv(C* c);
extern "C" int _ZN4Trap8BehaviorEv(C* c)
{
    char* p = (char*)c;
    if (*(unsigned char*)(p + 0x3ab)) {
        *(int*)(p + 0x3a4) = _ZN8dActor_c13DistToCPlayerEv(c);
        return 1;
    }
    (c->*data_ov010_02112d28[*(int*)(p + 0x3a0)])();
    func_ov010_0211146c(c);
    func_ov010_021113f0(c);
    return 1;
}

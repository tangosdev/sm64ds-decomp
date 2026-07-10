//cpp
struct C;
typedef void (C::*PMF)();
struct Entry { PMF pmf[1]; };
extern Entry data_ov080_02128438[];
struct C { char pad[0x17c]; int idx; };
extern "C" {
void _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(void* self, void* cyl);
int func_ov080_02124208(void* c);
void func_ov080_021243d8(char* t);
void _ZN12CylinderClsn5ClearEv(void* c);
void _ZN12CylinderClsn6UpdateEv(void* c);
int _ZN9MontyMole8BehaviorEv(C* c){
    char* p = (char*)c;
    _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(p, p + 0x138);
    int j = c->idx;
    (c->*data_ov080_02128438[j].pmf[0])();
    func_ov080_02124208(p);
    func_ov080_021243d8(p);
    _ZN12CylinderClsn5ClearEv(p + 0x138);
    _ZN12CylinderClsn6UpdateEv(p + 0x138);
    return 1;
}
}

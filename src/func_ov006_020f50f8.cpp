//cpp
struct C;
typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern "C" Entry data_ov006_021422bc[];
struct C {
  char pad[0x5318];
  int idx;
};
extern "C" void _ZN13dScMgMemory_c11StateResultEv(C* c){
  (c->*(data_ov006_021422bc[c->idx].pmf))();
}

//cpp
struct C;
typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern "C" Entry data_ov006_021422bc[];
struct C {
  char pad[0x5318];
  int idx;
};
extern "C" void func_ov006_020f50f8(C* c){
  (c->*(data_ov006_021422bc[c->idx].pmf))();
}

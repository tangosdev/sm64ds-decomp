//cpp
struct C;
typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern "C" Entry data_ov006_021423c0[];
struct C {
  char pad[0x53d8];
  int idx;
};
extern "C" void func_ov006_020f71c8(C* c){
  (c->*(data_ov006_021423c0[c->idx].pmf))();
}

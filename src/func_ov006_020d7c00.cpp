//cpp
struct C;
typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };
extern "C" Entry data_ov006_02141708[];
struct Elem { char pad[0x40]; };
struct C {};
extern "C" void func_ov006_020d7c00(C* c, int i){
  Elem* e = (Elem*)c + i;
  unsigned char idx = *((unsigned char*)e + 0x469b);
  (c->*(data_ov006_02141708[idx].pmf))(i);
}

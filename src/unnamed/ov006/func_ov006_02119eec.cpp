//cpp
struct C;
typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };
extern "C" Entry data_ov006_02142db0[];

struct Elem {
  unsigned char idx;
  char pad[0x23];
};

struct C {
  char pad[0x51d1];
  Elem arr[1];
};

extern "C" void func_ov006_02119eec(C* c, int i){
  (c->*(data_ov006_02142db0[c->arr[i].idx].pmf))(i);
}

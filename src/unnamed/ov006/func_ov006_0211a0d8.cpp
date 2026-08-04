//cpp
struct C;
typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };
extern "C" Entry data_ov006_02142d08[];
struct Sub { unsigned char idx; char pad[0x23]; };
struct C {
  char pad[0x51d1];
  Sub arr[1];
};
extern "C" void func_ov006_0211a0d8(C* c, int i){
  (c->*(data_ov006_02142d08[c->arr[i].idx].pmf))(i);
}

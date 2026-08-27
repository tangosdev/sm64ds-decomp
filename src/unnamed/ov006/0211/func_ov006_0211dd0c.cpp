//cpp
struct C;
typedef void (C::*PMF)(int);
extern PMF data_ov006_02142e88[];
struct C { char pad[0x4bba]; unsigned char idx; char gap; unsigned char g; };
extern "C" void func_ov006_0211dd0c(C *c){
  if (c->g == 0) return;
  (c->*data_ov006_02142e88[c->idx])(0);
}

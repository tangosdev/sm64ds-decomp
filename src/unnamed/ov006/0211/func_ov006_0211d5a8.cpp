//cpp
struct C;
typedef void (C::*PMF)(int);
extern PMF data_ov006_02142f18[];
struct C { char pad[0x4be0]; unsigned char g; unsigned char idx; };
extern "C" void func_ov006_0211d5a8(C *c){
  if (c->g == 0) return;
  (c->*data_ov006_02142f18[c->idx])(0);
}

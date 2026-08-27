//cpp
struct C;
typedef void (C::*PMF)(int);
extern PMF data_ov006_0214266c[];
struct C { char pad[0x5bc6]; unsigned char g; char gap; unsigned char idx; };
extern "C" void func_ov006_020faf6c(C*, int);
extern "C" void func_ov006_020fb60c(C *c){
  if (c->g == 0) return;
  (c->*data_ov006_0214266c[c->idx])(0);
  func_ov006_020faf6c(c, 0);
}

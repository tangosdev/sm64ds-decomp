//cpp
struct C; typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };
extern Entry data_ov002_02110e4c[];
struct C { unsigned char pad[0x15c]; unsigned char g; unsigned char idx; };
extern "C" void func_ov002_020f37a0(C* c){
  if(c->g==0) return;
  (c->*data_ov002_02110e4c[c->idx].pmf)(0);
}

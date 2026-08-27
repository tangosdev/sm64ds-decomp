//cpp
struct C; typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };
extern Entry data_ov002_02110ebc[];
struct C { unsigned char pad[0x1c8]; unsigned char g; unsigned char idx; };
extern "C" void func_ov002_020f2dd4(C* c){
  if(c->g==0) return;
  (c->*data_ov002_02110ebc[c->idx].pmf)(0);
}

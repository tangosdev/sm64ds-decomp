//cpp
struct C; typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };
extern Entry data_ov006_02143070[];
extern Entry data_ov006_02143020[];
struct Elem { unsigned char a; unsigned char b; char pad[0x22]; };
struct C { char head[0xbeb0]; Elem arr[1]; };
extern "C" void func_ov006_02129d94(C* c, int i) {
  int idx0 = c->arr[i].a;
  (c->*data_ov006_02143070[idx0].pmf)(i);
  int idx1 = c->arr[i].b;
  (c->*data_ov006_02143020[idx1].pmf)(i);
}

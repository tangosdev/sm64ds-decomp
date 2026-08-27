//cpp
extern "C" {
void _ZN9Animation7AdvanceEv(void *);
void func_ov006_020c4c00(void *c);
}
struct Foo {
  char pad[0x30];
  void (Foo::*pmf)();
};
extern "C" void func_ov006_020c4cd8(struct Foo *c){
  (c->*(c->pmf))();
  _ZN9Animation7AdvanceEv((char*)c+0x88);
  func_ov006_020c4c00(c);
}

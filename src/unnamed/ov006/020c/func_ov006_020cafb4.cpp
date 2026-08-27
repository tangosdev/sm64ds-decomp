//cpp
extern "C" {
extern int data_ov006_0212e038[];
struct C { virtual void a0(); virtual void a1(); virtual void a2(); virtual void a3(); virtual void a4(); virtual void f(void*); };
void func_ov006_020cafb4(char *o) {
  struct C *p = (struct C*)(o+0x6c);
  p->f((void*)data_ov006_0212e038);
}
}

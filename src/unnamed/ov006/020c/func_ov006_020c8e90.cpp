//cpp
extern "C" {
extern int data_ov006_0212e02c[];
struct C { virtual void a0(); virtual void a1(); virtual void a2(); virtual void a3(); virtual void a4(); virtual void f(void*); };
void func_ov006_020c8e90(char *o) {
  if(*(short*)(o+0x6e)==0) return;
  struct C *p = (struct C*)(o+0x78);
  p->f((void*)data_ov006_0212e02c);
}
}

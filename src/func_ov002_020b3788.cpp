//cpp
// @symbol func_ov002_020b3788
// recovered name: BigBrickBlock_OnAttacked2
/* recovered: renamed to Class_Method */
/* daObjBlockL_c::OnAttacked2 - recovered from vtable slot identity */
struct Sub { virtual void v0(); };
extern "C" {
void func_ov002_020b3788(char *c, char *arg1){
  struct Vt { int dummy[0x7c/4]; void (*fn)(void*); };
  int b = (*(unsigned short*)(c+0xc) == 0x11);
  if (b) {
    if (*(int*)(arg1+8) != 2) return;
    (*(struct Vt**)c)->fn(c);
    return;
  }
  (*(struct Vt**)c)->fn(c);
}
}

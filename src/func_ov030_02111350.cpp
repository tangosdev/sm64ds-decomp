//cpp
// @symbol func_ov030_02111350
// recovered name: daObjHmBskt_c_Render
/* recovered: renamed to Class_Method */
/* daObjHmBskt_c::Render - recovered from vtable slot identity */
struct Obj { virtual void f0(); virtual void f1(); virtual void f2(); virtual void f3(); virtual void f4(); virtual void m(int); };
extern "C" void _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern "C" void _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern "C" int func_ov030_02111350(char *c){
  _ZN8Platform21UpdateModelPosAndRotYEv(c);
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  ((Obj*)(c+0xd4))->m(0);
  return 1;
}

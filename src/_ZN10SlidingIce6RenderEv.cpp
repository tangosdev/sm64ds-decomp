//cpp
struct VObj {
  virtual void f0();
  virtual void f1();
  virtual void f2();
  virtual void f3();
  virtual void f4();
  virtual void m5(int);
};
extern "C" {
void _ZN8Platform21UpdateModelPosAndRotYEv(void*);
void _ZN8Platform19UpdateClsnPosAndRotEv(void*);
int _ZN10SlidingIce6RenderEv(char* c){
  int x = *(unsigned short*)(c+0xc)==0x5d;
  if(x){
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    ((VObj*)(c+0xd4))->m5(0);
  }
  return 1;
}
}

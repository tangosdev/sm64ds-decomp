//cpp
extern "C" {
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int, unsigned int, unsigned int, void*, unsigned int);
extern void _ZN6Camera9SetFlag_3Ev(void*);
extern void func_ov073_0211f144(void*);
extern int _ZN6Player12GetTalkStateEv(void*);
extern void func_ov073_0212157c(void*, void*);
extern void* CAMERA;
extern int data_ov073_02123370[];
int func_ov073_0211fbf4(char* c){
  void* pl = *(void**)(c+0x3e4);
  *(unsigned int*)(c+0x500) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(unsigned int*)(c+0x500), 3, 0x170, c+0x74, 0);
  _ZN6Camera9SetFlag_3Ev(CAMERA);
  func_ov073_0211f144(c);
  if(_ZN6Player12GetTalkStateEv(pl) == -1){
    func_ov073_0212157c(c, data_ov073_02123370);
  }
  return 1;
}
}

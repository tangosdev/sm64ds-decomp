//cpp
extern "C" {
extern int data_ov003_020b1704[];
extern int _ZN5ModelD1Ev[];
extern int _ZTV5Scene[];
extern int data_0208e4b8[];
int __destroy_arr(void* a, int b, int c, void* d);
int _ZN9ActorBaseD2Ev(void* c);
int func_ov003_020addfc(void* c){
  char* p=(char*)c;
  *(int*)p=(int)data_ov003_020b1704;
  __destroy_arr(p+0x64, 2, 0x50, _ZN5ModelD1Ev);
  *(int*)p=(int)_ZTV5Scene;
  *(int*)p=(int)data_0208e4b8;
  _ZN9ActorBaseD2Ev(c);
  return (int)c;
}
}

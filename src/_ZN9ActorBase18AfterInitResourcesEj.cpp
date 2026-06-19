//cpp
extern "C" {
extern int data_020a4b88[]; extern int data_02099f24[]; extern int data_020a4b78[]; extern int data_020a4b98[];
extern void func_0203b27c(void*, void*); extern void func_0204405c(void*, void*);
void _ZN9ActorBase18AfterInitResourcesEj(char* c, unsigned int a){
  if(a!=2) return;
  func_0203b27c(data_020a4b88, c+0x28);
  volatile int* p=data_02099f24; bool b=(p[0]==3); if(b){ *(bool*)(c+0x10)=true; return; }
  func_0204405c(data_020a4b78, c+0x28);
  func_0204405c(data_020a4b98, c+0x38);
  *(bool*)(c+0xe)=true;
}
}

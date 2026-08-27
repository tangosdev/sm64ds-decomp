//cpp
extern "C" {
extern int _ZN9Animation8FinishedEv(void*);
extern int Chuckya_ChangeState(void*, void*);
extern int data_ov062_0211de80[];
int func_ov062_0211632c(void* c){
  if(_ZN9Animation8FinishedEv((char*)c+0x350)){ *(int*)((char*)c+0x3f0)=0; Chuckya_ChangeState(c, data_ov062_0211de80); }
  return 1;
}
}

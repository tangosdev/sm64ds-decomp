//cpp
extern "C" {
struct S{int w[2];};
extern S data_ov090_02134498;
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,void*,int,int,unsigned int);
int func_ov090_02131608(void* c){
  *(int*)((char*)c+0x390)=0;
  *(int*)((char*)c+0x3a4)=0x1000;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c+0x30c, (void*)data_ov090_02134498.w[1], 0, 0x1000, 0);
  return 1;
}
}

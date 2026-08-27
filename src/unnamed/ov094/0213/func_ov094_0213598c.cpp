//cpp
extern "C" {
struct S{int w[2];};
extern S data_ov094_02136af8;
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,void*,int,int,unsigned int);
int func_ov094_0213598c(char* c){
  *(int*)(c+0x3e8)=0;
  *(short*)(c+0x100)=0;
  *(int*)(c+0x3f0)=0x2000;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x30c, (void*)data_ov094_02136af8.w[1], 0, 0x1000, 0);
  return 1;
}
}

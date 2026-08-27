//cpp
extern "C" {
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, void* bca, int a, int fix, unsigned int u);
extern int data_ov019_02113470[];
int func_ov019_02111754(char* c){
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, (void*)data_ov019_02113470[1], 0, 0x1000, 0);
  *(int*)(c+0x130)=0x1000;
  *(char*)(c+0x38f)=0;
  *(int*)(c+0x374)=5;
  return 1;
}
}

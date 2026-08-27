//cpp
extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* c, void* f, int a, int b, unsigned int u);
extern void* data_ov077_02127ca0[];
int func_ov077_02126758(char* c){
  *(int*)(c+0x98)=0;
  *(int*)(c+0x39c)=0x1000;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x340, data_ov077_02127ca0[1], 0x40000000, 0x1000, 0);
  return 1;
}
}

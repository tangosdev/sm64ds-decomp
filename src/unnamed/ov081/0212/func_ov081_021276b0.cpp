//cpp
extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* c, void* f, int a, int b, unsigned int u);
extern void* data_ov081_02128ec4[];
int func_ov081_021276b0(char* c){
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, data_ov081_02128ec4[1], 0, 0x1000, 0);
  *(int*)(c+0x130) = 0x1000;
  *(int*)(c+0x12c) = 0x6000;
  *(char*)(c+0x3f1) = 0;
  *(int*)(c+0x3e0) = 0;
  return 1;
}
}

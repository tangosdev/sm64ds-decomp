extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);
extern void _ZN12WithMeshClsn13SetLimMovFlagEv(void* p);
extern void func_0201267c(int id, void* p);
extern int data_ov071_02122f88[];
int func_ov071_02120130(char* c){
  *(int*)(c+0x9c) = -0x4000;
  *(int*)(c+0xa0) = -0x3e000;
  *(int*)(c+0x98) = 0x16000;
  *(int*)(c+0xa8) = 0x4d000;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, (void*)data_ov071_02122f88[1], 0, 0x1000, 0);
  *(int*)(c+0x130) = 0x1000;
  _ZN12WithMeshClsn13SetLimMovFlagEv(c+0x194);
  func_0201267c(0xf1, c+0x74);
  *(int*)(c+0x39c) = 1;
  return 1;
}

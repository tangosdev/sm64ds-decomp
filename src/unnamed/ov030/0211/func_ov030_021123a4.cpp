//cpp
extern "C" {
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
struct S { int w[2]; };
extern struct S data_ov030_02115d18;
int func_ov030_021123a4(char *c){
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, (void*)data_ov030_02115d18.w[1], 0, 0x1000, 0);
  *(int*)(c+0x130) = 0x1000;
  *(int*)(c+0x98) = 0x13000;
  *(unsigned char*)(c+0x3c6) = 0x1e;
  *(int*)(c+0x3b4) = 0xa;
  return 1;
}
}

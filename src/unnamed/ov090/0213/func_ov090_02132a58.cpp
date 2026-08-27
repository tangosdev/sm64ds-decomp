//cpp
extern "C" {
int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* anim, void* file, int a, int b, unsigned int u);
extern void* data_ov090_0213452c[];
int func_ov090_02132a58(char *c){
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void*)(c+0x30c), data_ov090_0213452c[1], 0, 0x1000, 0);
  *(int*)(c+0x3a8)=0;
  *(int*)(c+0x3fc)=0;
  *(int*)(c+0x400)=0;
  *(int*)(c+0x368)=0x1000;
  int i;
  for(i=0;i<0x14;i++) ((int*)(c+0x3ac))[i]=0;
  return 1;
}
}

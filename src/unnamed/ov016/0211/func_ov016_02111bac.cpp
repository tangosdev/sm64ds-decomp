//cpp
extern "C" {
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void* c, void* f, int a, int b, int d, unsigned short e);
extern void* data_ov016_02114d28[];
int func_ov016_02111bac(char* c){
  *(int*)(c+0x400)=0;
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x350, data_ov016_02114d28[1], 8, 0, 0x1000, 0);
  return 1;
}
}

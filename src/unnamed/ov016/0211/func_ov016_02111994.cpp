//cpp
extern "C" {
extern int func_02012694(int, void*, int);
extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void*, void*, int, int, int, unsigned short);
extern int data_ov016_02114d28[];
int func_ov016_02111994(char* c){
  *(int*)(c+0x400)=0;
  func_02012694(0xfa, c+0x74, 0);
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x350, (void*)data_ov016_02114d28[1], 8, 0, 0x1000, 0);
  return 1;
}
}

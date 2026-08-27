//cpp
extern "C" {
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void*, void*, int, int, int, unsigned short);
extern void* data_ov078_02126f18[];
int func_ov078_02123fb4(char *c){
  *(int*)(c+0x9c) = -0x2000;
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x2cc, data_ov078_02126f18[1], 0, 0x40000000, 0x1000, 0);
  return 1;
}
}

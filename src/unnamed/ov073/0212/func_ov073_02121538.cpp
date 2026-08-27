//cpp
extern "C" {
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void*, void*, int, int, int, unsigned short);
extern void* data_ov073_021232b8[];
int func_ov073_02121538(char *c){
  *(char*)(c+0x4c8)=0;
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x30c, data_ov073_021232b8[1], 4, 0, 0x1000, 0);
  return 1;
}
}

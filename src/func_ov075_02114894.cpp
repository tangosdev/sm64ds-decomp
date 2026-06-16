//cpp
extern "C" {
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void*, void*, int, int, int, unsigned short);
extern void* data_ov075_0211d424[];
void func_ov075_02114894(char *c){
  *(int*)(c+0x13c)=0;
  *(int*)(c+0x144)=0;
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c, data_ov075_0211d424[1], 4, 0, 0x1000, 0);
  *(int*)(c+0x110)=5;
  *(int*)(c+0x114)=4;
  *(char*)(c+0x153)=0;
}
}

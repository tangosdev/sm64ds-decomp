//cpp
struct BCA_File;
extern "C" {
void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void*, BCA_File&, int, int, int, unsigned short);
extern int* data_ov073_02123290[];
int func_ov073_02120098(char* c){
  *(char*)(c+0x4ca) = 0;
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x30c, *(BCA_File*)data_ov073_02123290[1], 4, 0x40000000, 0x1000, 0);
  return 1;
}
}

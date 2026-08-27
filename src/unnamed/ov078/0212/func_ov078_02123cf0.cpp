//cpp
struct BCA_File;
extern "C" {
void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void*, BCA_File&, int, int, int, unsigned short);
extern int* data_ov078_02126ef0[];
int func_ov078_02123cf0(char* c){
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x2cc, *(BCA_File*)data_ov078_02126ef0[1], 0, 0x40000000, 0x1000, 0);
  *(int*)(c+0x98)=0; return 1;
}}

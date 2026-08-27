//cpp
typedef int Fix12;
struct BCA_File;
struct BlendModelAnim { int SetAnim(BCA_File&, int, int, Fix12, unsigned short); };
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *, BCA_File&, int, int, Fix12, unsigned short);

extern int* data_ov078_02126ee8[];
extern "C" int func_ov078_02123bc4(char* c){
  *(int*)(c+0x9c)=-0x2000;
  *(int*)(c+0x4fc)=2;
  *(int*)(c+0x98)=0xa000;
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim*)(c+0x2cc), *(BCA_File*)data_ov078_02126ee8[1], 4, 0, 0x1000, 0);
  return 1;
}

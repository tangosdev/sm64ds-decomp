//cpp
struct BCA_File;
struct ModelAnim { void SetAnim(BCA_File*, int, int, unsigned int); };
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, BCA_File*, int, int, unsigned int);

struct S { int a; BCA_File* b; };
extern S data_ov014_02114980;
extern "C" void func_ov014_02111a6c(char* c){
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((ModelAnim*)(c+0x150), data_ov014_02114980.b, 0, 0x1000, 0);
  *(int*)(c+0xa8)=0;
  *(int*)(c+0x98)=0;
  *(int*)(c+0x9c)=-0x2000;
  *(char*)(c+0x604)=0;
  *(short*)(c+0x600)=0;
  *(short*)(c+0x500+0xfc)=0x3c;
  *(int*)(((int)c + 0xb0)) &= ~3;
  *(int*)(c+0x60)=*(int*)(c+0x5f0)+0xc8000;
}

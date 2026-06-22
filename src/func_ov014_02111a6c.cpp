//cpp
// NONMATCHING: base materialization / addressing (div=4). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct BCA_File;
struct ModelAnim { void SetAnim(BCA_File*, int, int, unsigned int); };
struct S { int a; BCA_File* b; };
extern S data_ov014_02114980;
extern "C" void func_ov014_02111a6c(char* c){
  ((ModelAnim*)(c+0x150))->SetAnim(data_ov014_02114980.b, 0, 0x1000, 0);
  *(int*)(c+0xa8)=0;
  *(int*)(c+0x98)=0;
  *(int*)(c+0x9c)=-0x2000;
  *(char*)(c+0x604)=0;
  *(short*)(c+0x600)=0;
  *(short*)(c+0x500+0xfc)=0x3c;
  {int *q=(int*)(c+0xb0); *q=*q & ~3;}
  *(int*)(c+0x60)=*(int*)(c+0x5f0)+0xc8000;
}

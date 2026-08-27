extern int data_ov004_020bc20c[];
extern int data_ov004_020bc1b4[];
struct S2 { int w[2]; };
void func_ov004_020b37f0(int* r0){
  char* c=(char*)r0;
  *(int*)(c+0x24)=0;
  *(int*)(c+0x28)=0x27000;
  *(struct S2*)(c+0)=*(struct S2*)data_ov004_020bc20c;
  *(struct S2*)(c+8)=*(struct S2*)data_ov004_020bc1b4;
}

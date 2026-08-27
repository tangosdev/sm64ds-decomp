//cpp
extern "C" {
extern int data_ov002_02110154[];
extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void* c, unsigned int a, int b, int f, unsigned int g);
void func_ov002_020d228c(char* c){
  _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_02110154);
  _ZN6Player7SetAnimEji5Fix12IiEj(c, 9, 0, 0x1000, 0);
  *(unsigned char*)(c+0x6e3)=4;
  *(int*)(c+0x56c)=0;
  *(int*)(c+0x570)=0;
  *(int*)(c+0x574)=0;
  *(unsigned char*)(c+0x721)=2;
  *(unsigned char*)(c+0x70c)=0;
}
}

//cpp
extern "C" {
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, unsigned int a, int b, int f, unsigned int g);
void func_ov002_020c9ac0(char* c){
  *(unsigned char*)(c+0x743)=1;
  *(unsigned char*)(c+0x716)=1;
  *(unsigned char*)(c+0x713)=0;
  *(int*)(c+0x9c)=0;
  *(int*)(c+0x98)=0;
  *(int*)(c+0xa8)=0;
  *(unsigned char*)(c+0x6e3)=0x10;
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0x93,0x40000000,0x1000,0);
}
}

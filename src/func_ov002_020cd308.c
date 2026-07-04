extern int _ZN6Player7SetAnimEji5Fix12IiEj(void* c, unsigned int a, int b, int f, unsigned int g);
void func_ov002_020cd308(char* c){
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0xae,0x40000000,0x1000,0);
  *(unsigned char*)(c+0x6e3)=6;
  *(unsigned short*)(c+0x6a4)=0x10;
  *(unsigned char*)(((int)c+0x6e5)&0xFFFFFFFFFFFFFFFFLL)&=0xa;
}

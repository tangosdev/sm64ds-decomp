typedef int Fix12i;
extern int func_ov002_020bf27c(char* c, int r1);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* p, unsigned int a, int b, Fix12i c, unsigned int d);
extern unsigned char data_020a0e40;
extern unsigned char data_0209f49c[];
void func_ov002_020e200c(char* c){
  *(int*)(c + 0x9c) = func_ov002_020bf27c(c, -0x4000);
  *(int*)(c + 0xa0) = -0x4b000;
  if (*(int*)(c + 0xa8) >= 0){
    *(int*)(c + 0x9c) = -0x8000;
    if (*(unsigned short*)&data_0209f49c[data_020a0e40 * 0x18] & 2)
      *(int*)(c + 0x9c) = -0x3400;
  }
  if (*(int*)(c + 0xa8) >= 0) return;
  if (*(unsigned char*)(c + 0x6e1) == 1)
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x50, 0x40000000, 0x1000, 0);
  if ((*(unsigned short*)&data_0209f49c[data_020a0e40 * 0x18] & 2) == 0) return;
  if (*(unsigned char*)(c + 0x6ff) == 0) return;
  *(int*)(c + 0x9c) = -0x2000;
  *(int*)(c + 0xa0) = -0x25800;
}

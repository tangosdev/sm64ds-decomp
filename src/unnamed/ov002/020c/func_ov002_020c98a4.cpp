//cpp
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(char*, int, int, int, unsigned int);
extern void func_0200d148(void*, unsigned char);
extern void* data_0209f318[];
void func_ov002_020c98a4(char* c){
  *(unsigned char*)(c+0x71a)=0;
  *(unsigned char*)(c+0x6e3)=0x18;
  _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x47, 0, 0x1000, 0);
  *(short*)(c+0x6a4)=2;
  *(int*)(c+0x98)=0;
  *(int*)(c+0xa8)=0;
  *(unsigned char*)(c+0x70c)=0;
  func_0200d148(data_0209f318[0], *(unsigned char*)(c+0x6d8));
}
}

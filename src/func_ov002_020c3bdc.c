typedef int Fix12i;
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* p, unsigned int a, int b, Fix12i c, unsigned int d);
extern int _ZN6Player12FinishedAnimEv(void* p);
extern void func_0200eec8(void);
extern void func_ov002_020c3cf0(void* p);
extern void Player_AdvanceAnims(void* p);

#pragma optimize_for_size on
void func_ov002_020c3bdc(char* c){
  if (*(unsigned char*)(c + 0x6e5) == 0){
    *(unsigned char*)(c + 0x6ff) = 1;
    *(short*)(c + 0x6ae) = 0x708;
    *(short*)(c + 0x94) = -0x8000;
    *(short*)(c + 0x8e) = *(short*)(c + 0x94);
    *(int*)(c + 0x64) = 0;
    *(int*)(c + 0x5c) = *(int*)(c + 0x64);
    *(int*)(c + 0x98) = 0;
    *(int*)(c + 0x9c) = -0x2000;
    if (*(unsigned char*)(c + 0x6de) == 0){
      _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x55, 0x40000000, 0x1000, 0);
      *(unsigned char *)(((int)c + 0x6e5)) += 1;
    }
  } else {
    if (_ZN6Player12FinishedAnimEv(c)){
      func_0200eec8();
      func_ov002_020c3cf0(c);
      _ZN6Player7SetAnimEji5Fix12IiEj(c, 0xc0, 0x40000000, 0x1000, 0);
      *(unsigned char*)(c + 0x743) = 3;
      *(unsigned char*)(c + 0x716) = 1;
      *(unsigned char*)(c + 0x713) = 0;
      *(int*)(c + 0xa8) = 0;
      *(int*)(c + 0x9c) = 0;
      *(int*)(c + 0x60) = 0x15e000;
    }
  }
  Player_AdvanceAnims(c);
}

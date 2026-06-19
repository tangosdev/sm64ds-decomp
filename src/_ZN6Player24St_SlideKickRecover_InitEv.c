typedef int Fix12i;
extern int func_ov002_020dab14(void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, Fix12i, unsigned int);
extern int func_ov002_020e25f0(void*, int);
extern int _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int, void*);
int _ZN6Player24St_SlideKickRecover_InitEv(void* c) {
  *(char*)((char*)c+0x71b)=0;
  *(char*)((char*)c+0x712)=1;
  *(char*)((char*)c+0x70d)=0;
  func_ov002_020dab14(c);
  *(char*)((char*)c+0x6e1)=0;
  *(char*)((char*)c+0x6de)=1;
  *(char*)((char*)c+0x6df)=0;
  *(int*)((char*)c+0xa8)=0x20000;
  _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x56, 0x40000000, 0x1000, 0);
  func_ov002_020e25f0(c, 0);
  _ZN5Sound9PlayBank0EjRK7Vector3(0xf, (char*)c+0x74);
  return 1;
}

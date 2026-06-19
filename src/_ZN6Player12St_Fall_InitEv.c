typedef int Fix12i;
extern int _ZN6Player6IsAnimEj(void*, unsigned int);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, Fix12i, unsigned int);
int _ZN6Player12St_Fall_InitEv(void* c) {
  *(char*)((char*)c+0x71b)=0;
  *(char*)((char*)c+0x712)=1;
  *(char*)((char*)c+0x70d)=0;
  *(char*)((char*)c+0x6e1)=0;
  *(char*)((char*)c+0x6de)=1;
  *(char*)((char*)c+0x6df)=0;
  if (_ZN6Player6IsAnimEj(c, 0x41)) {
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x44, 0x40000000, 0x1000, 0);
    *(char*)((char*)c+0x6e3)=1;
  } else {
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x54, 0x40000000, 0x1000, 0);
    *(char*)((char*)c+0x6e3)=0;
  }
  *(int*)((char*)c+0x684) = *(int*)((char*)c+0x60);
  return 1;
}

typedef int Fix12i;
extern int func_ov002_020dab14(void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, Fix12i, unsigned int);
extern int _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int, void*);
int _ZN6Player19St_GroundPound_InitEv(void* c) {
  unsigned int anim;
  func_ov002_020dab14(c);
  *(char*)((char*)c+0x6e1)=0;
  *(int*)((char*)c+0xa8)=0;
  *(int*)((char*)c+0x98)=0;
  *(short*)((char*)c+0x94)=*(short*)((char*)c+0x8e);
  anim=0x3b;
  if (*(unsigned char*)((char*)c+0x703)) anim=0xa3;
  _ZN6Player7SetAnimEji5Fix12IiEj(c, anim, 0x40000000, 0x1000, 0);
  *(char*)((char*)c+0x6e3)=2;
  _ZN5Sound9PlayBank0EjRK7Vector3(6, (char*)c+0x74);
  *(int*)((int*)(((int)c + 0x2ec) & 0xFFFFFFFFFFFFFFFF)) |= 0x20;
  return 1;
}

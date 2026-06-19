typedef int Fix12i;
extern int func_ov002_020e2be4(void*);
extern int func_ov002_020e2ba8(void*);
extern int func_ov002_020e2b6c(void*);
extern int func_ov002_020e2ad0(void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, Fix12i, unsigned int);
extern int func_ov002_020e25f0(void*, int);
int _ZN6Player16St_SideFlip_InitEv(void* c) {
  *(char*)((char*)c+0x71b)=0;
  if (func_ov002_020e2be4(c)) return 1;
  if (func_ov002_020e2ba8(c)) return 1;
  if (func_ov002_020e2b6c(c)) return 1;
  *(char*)((char*)c+0x712)=1;
  *(char*)((char*)c+0x70d)=0;
  *(char*)((char*)c+0x6e1)=0;
  *(char*)((char*)c+0x6de)=1;
  *(char*)((char*)c+0x6df)=0;
  *(int*)((char*)c+0x98)=0x8000;
  *(int*)((char*)c+0xa8)=0x3e000;
  func_ov002_020e2ad0(c);
  _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x4d, 0x40000000, 0x1000, 0);
  *(short*)((char*)c+0x94) = *(short*)((char*)c+0x6d2);
  *(short*)((char*)c+0x8e) = *(short*)((char*)c+0x6d2) + 0x8000;
  func_ov002_020e25f0(c, 0);
  return 1;
}

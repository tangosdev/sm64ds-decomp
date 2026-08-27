typedef int Fix12i;
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, Fix12i, unsigned int);
extern int _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int, void*);
extern int func_ov002_020e25f0(void*, int);
void func_ov002_020c9b7c(void* c) {
  *(char*)((char*)c+0x6e3)=0xb;
  *(char*)((char*)c+0x713)=0;
  if (*(unsigned char*)((char*)c+0x6de)) {
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x56, 0, 0x1000, 0);
    _ZN5Sound9PlayBank0EjRK7Vector3(6, (char*)c+0x74);
  } else {
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x4f, 0x40000000, 0x1000, 0);
    func_ov002_020e25f0(c, 0);
  }
  *(int*)((char*)c+0x9c)=0;
  *(int*)((char*)c+0x98)=0;
  *(int*)((char*)c+0xa8)=0;
}

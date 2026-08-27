typedef int Fix12;
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, Fix12, unsigned int);
extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int, unsigned int, void*);
void func_ov002_020c990c(void* c) {
  *(unsigned char*)((char*)c+0x71a)=1;
  if (*(unsigned char*)((char*)c+0x6de)) {
    *(unsigned char*)((char*)c+0x6e3)=0x16;
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x54, 0x40000000, 0x1000, 0);
  } else {
    *(unsigned char*)((char*)c+0x6e3)=0x17;
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x98, 0x40000000, 0x1000, 0);
  }
  *(int*)((char*)c+0x98)=0;
  *(int*)((char*)c+0xa8)=0;
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)((char*)c+0x6d9), 0x29, (char*)c+0x74);
}

extern int func_ov002_020ae8b8(void*, void*);
extern int _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int, void*);
int func_ov002_020ae890(void* c, void* a){
  *(short*)((char*)c+0x94)=*(short*)((char*)a+0x8e);
  func_ov002_020ae8b8(c,a);
  return _ZN5Sound9PlayBank0EjRK7Vector3(0xa,(char*)c+0x74);
}

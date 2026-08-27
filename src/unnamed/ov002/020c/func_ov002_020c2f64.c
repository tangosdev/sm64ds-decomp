extern int _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int, void*);
void func_ov002_020c2f64(void* c) {
  if (*(unsigned char*)((char*)c+0x6df)) return;
  *(unsigned char*)((char*)c+0x6df) = 1;
  if (*(unsigned char*)((char*)c+0x6f9)) {
    _ZN5Sound9PlayBank0EjRK7Vector3(0xa1, (char*)c+0x74);
    return;
  }
  if (*(unsigned char*)((char*)c+0x703)) {
    _ZN5Sound9PlayBank0EjRK7Vector3(0xd2, (char*)c+0x74);
    return;
  }
  _ZN5Sound9PlayBank0EjRK7Vector3(*(int*)((char*)c+0x66c)+0x40, (char*)c+0x74);
}

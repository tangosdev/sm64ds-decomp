extern int _ZN5Actor11UntrackStarERa(void*, signed char*);
void func_ov002_020e6d88(void* c) {
  *(int*)((char*)c+0x80) = 0;
  *(int*)((char*)c+0x84) = 0;
  *(int*)((char*)c+0x88) = 0;
  *(unsigned short*)(((int)c + 0x4a2)) &= ~0x100;
  _ZN5Actor11UntrackStarERa(c, (signed char*)(((int)c + 0x498)));
  c = (void*)(int)((unsigned long long)(int)c);
  *(unsigned short*)(((int)c + 0x4a2)) &= ~0x200;
  *(unsigned short*)((char*)c+0x492) = 0;
  *(unsigned short*)((char*)c+0x100) = 0;
}

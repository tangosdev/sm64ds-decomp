extern int _ZN7fBase_c18MarkForDestructionEv(void*);
void func_ov002_020b1a60(void* c) {
  {
    unsigned short* n = (unsigned short*)(((int)c + 0x3a8));
    *n += 1;
  }
  {
    int* s = (int*)(((int)*(void**)((char*)c+0x39c) + 0x5c));
    *(int*)((char*)c+0x5c) = s[0];
    *(int*)((char*)c+0x60) = s[1];
    *(int*)((char*)c+0x64) = s[2];
  }
  {
    int* g = (int*)(((int)c + 0x60));
    *g += 0xc8000;
  }
  if (*(unsigned short*)((char*)c+0x300+0xa8) < 0x41) return;
  _ZN7fBase_c18MarkForDestructionEv(c);
  *(unsigned short*)((char*)c+0x300+0xa8) = 0;
}

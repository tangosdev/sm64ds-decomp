extern int data_020874cc[];
int func_02005060(void* c){
  *(int*)((char*)c+0x13c)=(int)data_020874cc;
  void* r0=*(void**)((char*)c+0x110);
  int r3=*(short*)((char*)c+0x100+0x7c);
  int r1=*(short*)((char*)r0+0x8e);
  r1=(short)(r1+0x8000);
  *(short*)((char*)c+0x100+0x84)=(short)(r1-r3);
  return 1;
}

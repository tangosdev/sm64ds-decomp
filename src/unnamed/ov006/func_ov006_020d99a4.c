void func_ov006_020d99a4(void* c){
  char* p=(char*)c;
  *(int*)(p+0x1c)=-0x28000;
  *(int*)(p+0x20)=*(int*)(p+8);
  int t=*(int*)(p+4);
  unsigned ip=0x80000-t;
  *(short*)(p+0x28)=(short)((int)ip/0x10000);
  *(int*)(p+0x14)=0x10000;
  *(int*)(p+0x18)=0;
  p[0x2b]=0xa;
}

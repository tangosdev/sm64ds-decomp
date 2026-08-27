void func_ov006_020d9020(void* c){
  unsigned char* p=(unsigned char*)c;
  if(p[0xc4]==0){ p[0xc3]=1; p[0xc4]=1; *(short*)(p+0xc0)=0; }
  *(int*)(p+0x6000+0x2d0)=2;
}

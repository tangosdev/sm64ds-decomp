void func_ov006_020f4bbc(unsigned char* c){
  short n=*(short*)(c+0x5326);
  short k;
  short* q;
  if(n<1) return;
  k=*(short*)(c+0x5328);
  *(unsigned char*)(c+(0xb-k)*0x18+0x51bb)=1;
  q=(short*)(((int)c+0x5328));
  *q=*q+1;
  if(*(unsigned char*)(c+0x533c)==1){
    if(*(short*)(c+0x5328)>=5) *(int*)(c+0x5318)=2;
  } else {
    if(*(short*)(c+0x5328)>=4) *(int*)(c+0x5318)=2;
  }
}

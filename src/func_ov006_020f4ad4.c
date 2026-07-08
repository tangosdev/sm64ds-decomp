void func_ov006_020f4ad4(unsigned char* c){
  short n=*(short*)(c+0x5326);
  short k;
  short* q;
  if(n<3) return;
  k=*(short*)(c+0x5328);
  *(unsigned char*)(c+(0xb-k)*0x18+0x51bb)=1;
  q=(short*)(((int)c+0x5328) & 0xFFFFFFFFFFFFFFFF);
  *q=*q+1;
  if(*(short*)(c+0x5328)>=0xc) *(int*)(c+0x5318)=4;
}

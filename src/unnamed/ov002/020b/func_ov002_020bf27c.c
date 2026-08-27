int func_ov002_020bf27c(char* c, int r1){
  int v=*(int*)(c+8);
  int b;
  if(*(unsigned char*)(c+0x703)!=0) return r1;
  b=(int)(v==3);
  if(b!=0) return r1*0x5a/100;
  if(v==1) r1>>=2;
  return r1;
}

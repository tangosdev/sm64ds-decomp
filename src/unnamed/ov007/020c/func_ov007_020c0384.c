void func_ov007_020c0384(int *c, int x){
  if(c[2]==0){ c[3]=x; c[2]=c[3]; }
  else {
    *(int*)(x+0x34)=c[3];
    *(int*)(c[3]+0x30)=x;
    c[3]=x;
  }
}

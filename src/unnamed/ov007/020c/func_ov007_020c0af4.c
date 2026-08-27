void func_ov007_020c0af4(int *c, int x){
  if(c[0]==0){ c[1]=x; c[0]=c[1]; }
  else {
    *(int*)(x+0x40)=c[1];
    *(int*)(c[1]+0x3c)=x;
    c[1]=x;
  }
}

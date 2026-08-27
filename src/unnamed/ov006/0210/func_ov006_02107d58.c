void func_ov006_02107d58(int *p){
  int i;
  for(i=0;i<5;i++) p[i]=0;
  *(short*)((char*)p+0x14)=4;
  *(short*)((char*)p+0x16)=0;
}

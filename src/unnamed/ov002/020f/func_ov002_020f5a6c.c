void func_ov002_020f5a6c(char *p){
  int i;
  for(i=0;i<4;i++){
    *(unsigned char*)(p+0x44)=0;
    *(unsigned char*)(p+0x45)=0;
    *(unsigned char*)(p+0x4a)=0;
    p+=0x4c;
  }
}

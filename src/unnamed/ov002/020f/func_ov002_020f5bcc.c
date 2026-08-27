void func_ov002_020f5bcc(char *p){
  int i;
  for(i=0;i<4;i++){
    if(*(unsigned char*)(p+0x44)) *(unsigned char*)(p+0x4a)=1;
    p+=0x4c;
  }
}

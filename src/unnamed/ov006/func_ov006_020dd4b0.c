extern void func_020127a4(int r0, int r1, int r2, int r3);

void func_ov006_020dd4b0(char* base, int id){
  int i;
  char* p=base;
  for(i=0;i<0x28;i++){
    if(*(unsigned char*)(p+0x4677)!=0 &&
       *(unsigned char*)(p+0x4675)==4 &&
       id==*(unsigned char*)(p+0x467b)){
      int v=*(int*)(base+id*0x18+0x4ac4);
      *(int*)(base+i*0x1c+0x4664)=v-0x20000;
      *(unsigned char*)(base+i*0x1c+0x4676)=1;
      *(unsigned char*)(base+i*0x1c+0x4675)=5;
      *(int*)(base+i*0x1c+0x466c)=-0x4800;
      *(short*)(base+i*0x1c+0x4670)=0;
      int w=(*(int*)(base+id*0x18+0x4ac0)>>12)-0x80;
      int r=w>>1;
      if(r>=0x3c) r=0x3c;
      if(r<=-0x3c) r=-0x3c;
      func_020127a4(2,0xef,0xffff,r);
      return;
    }
    p+=0x1c;
  }
}

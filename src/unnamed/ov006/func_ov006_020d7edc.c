void func_ov006_020d7edc(unsigned char* c,int idx){
  unsigned char* slot=c+idx*0x40;
  if(*(unsigned short*)(slot+0x4690)!=0){
    unsigned short* p=(unsigned short*)(c+0x4690+idx*0x40);
    *p=*p-1;
    if(*(short*)(slot+0x4690)<0) *(unsigned short*)(slot+0x4690)=0;
  } else {
    *(unsigned char*)(slot+0x4697)=4;
    *(unsigned char*)(slot+0x4694)=3;
    *(unsigned char*)(slot+0x4695)=0;
    *(unsigned short*)(slot+0x468e)=0;
  }
}

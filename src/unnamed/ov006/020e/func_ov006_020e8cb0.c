void func_ov006_020e8cb0(char* c, int i){
  int idx = i*0x14;
  char* r2 = c + 0x528c;
  unsigned short* p = (unsigned short*)(r2 + idx);
  if(*p != 0){
    *p = *p - 1;
    if(*(short*)p < 0) *(short*)p = 0;
    return;
  }
  char* r0 = c + idx;
  *(char*)(r0+0x5000+0x292)=1;
  *(char*)(r0+0x5000+0x291)=1;
}

void func_ov006_020ddca0(char* c, int i){
  int idx = i*0x1c;
  char* r2 = c + 0x4670;
  unsigned short* p = (unsigned short*)(r2 + idx);
  if(*p != 0){
    *p = *p - 1;
    if(*(short*)p < 0) *(short*)p = 0;
    return;
  }
  char* r0 = c + idx;
  *(char*)(r0+0x4000+0x675)=1;
  *(char*)(r0+0x4000+0x679)=1;
}

extern void func_ov006_020cd720(short *p);
void func_ov006_020cd6d8(short *c){
  func_ov006_020cd720(c);
  *(short*)((char*)c+0x20)=0;
}

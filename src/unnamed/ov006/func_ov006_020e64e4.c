void func_ov006_020e64e4(int c){
  extern int func_ov006_020e668c();
  extern int func_ov006_020e5ffc();
  func_ov006_020e668c(c);
  *(char*)((char*)c+0x5000+0x5bb)=1;
  *(short*)((char*)c+0x5500+0xb4)=0;
  *(char*)((char*)c+0x5000+0x5c3)=1;
  func_ov006_020e5ffc(c);
  *(int*)((char*)c+0x5000+0x580)=1;
}

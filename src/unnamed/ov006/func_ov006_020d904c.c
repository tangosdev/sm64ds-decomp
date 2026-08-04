void func_ov006_020d904c(void* c){
  extern int func_ov006_020d6630();
  extern int func_ov006_020d62e0();
  extern int func_ov006_020d604c();
  func_ov006_020d6630(c);
  func_ov006_020d62e0(c);
  func_ov006_020d604c(c);
  *(int*)((char*)c+0x6000+0x2d0)=1;
}

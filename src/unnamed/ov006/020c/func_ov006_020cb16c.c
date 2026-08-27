extern int func_ov006_020ccd78();
void func_ov006_020cb16c(int *c){
  if(*(int*)((char*)c+0x20) >= -0x120000) return;
  *(int*)((char*)c+0x38)=0;
  func_ov006_020ccd78();
}

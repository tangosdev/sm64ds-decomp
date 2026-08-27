extern int func_ov006_020c8658(void *c);
void func_ov006_020c8048(void *c){
  if(*(int*)((char*)c+0x18) >= -0x120000) return;
  *(int*)((char*)c+0x24)=0;
  func_ov006_020c8658(c);
}

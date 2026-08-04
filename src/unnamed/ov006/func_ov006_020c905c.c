extern void func_ov006_020ca3a8(char *o);
void func_ov006_020c905c(char *o){
  if(*(int*)(o+0x28) >= -0x120000) return;
  *(int*)(o+0x40)=0;
  func_ov006_020ca3a8(o);
}

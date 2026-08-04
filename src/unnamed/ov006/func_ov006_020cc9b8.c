extern int data_ov006_021405c8;
extern void func_ov006_020cc198(int *c);
void func_ov006_020cc9b8(int *c){
  *(int*)((char*)c+0x20)=data_ov006_021405c8;
  if(*(int*)((char*)c+0x1c) <= -0x5c008) return;
  func_ov006_020cc198(c);
}

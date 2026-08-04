extern int data_ov006_021405c8[];
extern int data_ov006_0214055c;
extern void func_ov006_020cc198(int *c);
void func_ov006_020cc63c(int *c){
  short idx=*(short*)((char*)c+0x52);
  *(int*)((char*)c+0x20)=data_ov006_021405c8[idx];
  if(*(int*)((char*)c+0x1c) >= 0x5bff8) return;
  data_ov006_0214055c--;
  func_ov006_020cc198(c);
}

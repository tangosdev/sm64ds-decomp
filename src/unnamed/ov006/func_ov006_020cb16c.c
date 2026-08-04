extern int data_ov007_020ccd78();
void func_ov006_020cb16c(int *c){
  if(*(int*)((char*)c+0x20) >= -0x120000) return;
  *(int*)((char*)c+0x38)=0;
  data_ov007_020ccd78();
}

extern void func_ov006_020c8c6c(short* p);
extern short data_ov006_021404d8[];
extern int data_ov006_021404c4[];
extern int data_ov006_021404c8[];
void func_ov006_020c8a9c(int a0,int a1){
  int i;
  short* p=data_ov006_021404d8;
  for(i=0;i<3;i++){
    func_ov006_020c8c6c(p);
    p=(short*)((char*)p+0x20);
  }
  data_ov006_021404c4[0]=a0;
  data_ov006_021404c8[0]=a1;
}

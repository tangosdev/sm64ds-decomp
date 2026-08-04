extern void func_ov006_020c7860(int);
extern void func_ov006_020c8a64(void);
extern unsigned short data_ov006_02140404[];
extern int data_ov006_02140418[];
extern int data_ov006_02140420[];
void func_ov006_020c712c(void){
  int i;
  data_ov006_02140404[0] = data_ov006_02140404[0] + 0x800;
  for(i=0;i<data_ov006_02140418[0];i++){
    func_ov006_020c7860(data_ov006_02140420[0] + i*0xb8);
  }
  func_ov006_020c8a64();
}

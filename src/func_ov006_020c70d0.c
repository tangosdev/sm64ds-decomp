extern void _ZN16dMgJump3DMario_c19func_ov006_020c7734Ev(char *x);
extern void func_ov006_020c8a30(void);
extern int data_ov006_02140418[];
extern char* data_ov006_02140420[];
void func_ov006_020c70d0(void){
  int i;
  for(i=0;i<data_ov006_02140418[0];i++){
    _ZN16dMgJump3DMario_c19func_ov006_020c7734Ev((char *)((int)data_ov006_02140420[0]+i*0xb8));
  }
  func_ov006_020c8a30();
}

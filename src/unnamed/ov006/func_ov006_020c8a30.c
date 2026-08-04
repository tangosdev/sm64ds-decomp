extern int data_ov006_02140518[];
extern void func_ov006_020c8af4(void*);
void func_ov006_020c8a30(void){
  int i;
  char *p=(char*)data_ov006_02140518;
  for(i=2;i>=0;i--){
    func_ov006_020c8af4(p);
    p-=0x20;
  }
}

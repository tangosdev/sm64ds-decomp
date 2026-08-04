extern char data_ov006_021404d8[];
extern void func_ov006_020c8ba8(void *p);
void func_ov006_020c8a64(void){
  int i = 0;
  char *p = data_ov006_021404d8;
  for(;;){
    func_ov006_020c8ba8(p);
    i++;
    p += 0x20;
    if(i >= 3) break;
  }
}

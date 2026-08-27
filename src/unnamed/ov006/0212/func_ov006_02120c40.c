extern void func_ov006_02120ab8(int);
extern int data_ov006_02142f70;
extern int data_ov006_02142f68;
void func_ov006_02120c40(void){
  int i = 0;
  if(data_ov006_02142f70 > 0){
    int off = 0;
    do {
      func_ov006_02120ab8(data_ov006_02142f68 + off);
      i++;
      off += 0x24;
    } while(i < data_ov006_02142f70);
  }
}

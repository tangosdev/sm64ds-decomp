extern void func_ov006_020c4c54(int);
extern int data_ov006_02140328;
extern int data_ov006_02140324;
void func_ov006_020c425c(void){
  int i = 0;
  if(data_ov006_02140328 > 0){
    int off = 0;
    do {
      func_ov006_020c4c54(data_ov006_02140324 + off);
      i++;
      off += 0xf0;
    } while(i < data_ov006_02140328);
  }
}

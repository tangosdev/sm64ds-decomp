extern void func_ov004_020b0d8c(void *c, int arg1, int arg2);
void func_ov006_020dc814(char *c){
  if(*(int*)(c+0x5000+0x1c8) != 5) return;
  int v = *(int*)(c+0x5000+0x1cc);
  if(v > 0x80) return;
  if(v == 0) return;
  func_ov004_020b0d8c(c, 0xe0, 0xa0);
}

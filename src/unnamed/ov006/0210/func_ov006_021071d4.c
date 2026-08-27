extern void func_ov006_02105118(char *p);
void func_ov006_021071d4(char *c){
  func_ov006_02105118(c);
  *(int*)(c + 0x4ca8) = 1;
  *(int*)(c + 0x4cac) = 0;
}

//cpp
extern "C" {
extern int data_ov102_0214ea78[];
extern int func_ov102_0214d1f8(void *c, void *p);
int func_ov102_0214cf4c(char *c, char *p){
  if(*(int*)(c+0x3ac)==(int)data_ov102_0214ea78) return 0;
  *(short*)(c+0x3bc)=*(short*)(p+0x8e);
  func_ov102_0214d1f8(c, data_ov102_0214ea78);
  return 1;
}
}

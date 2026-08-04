extern void func_ov006_0211474c(void* a, int b, int c, void* s);
extern int data_ov006_0213ed38[];
int func_ov006_0210e480(char* c, int b, int d, int* s){
  int tmp[2];
  tmp[0] = s[0];
  tmp[1] = s[1];
  func_ov006_0211474c(c, b, d, tmp);
  *(int*)c = (int)data_ov006_0213ed38;
  *(int*)(c+0x28) = 0x8000;
  return (int)c;
}

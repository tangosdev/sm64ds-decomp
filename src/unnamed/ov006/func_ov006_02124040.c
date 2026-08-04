extern void func_ov006_020caa08(void* a);
struct S2 { int a; int b; };
extern struct S2 data_ov006_0213fbe8;
void func_ov006_02124040(char* c){
  *(int*)(c+0x7b84) = 0xb4;
  *(short*)(c+0x7ba4) = 1;
  func_ov006_020caa08(c+0x7b00);
  *(struct S2*)(c+0x5004) = data_ov006_0213fbe8;
}

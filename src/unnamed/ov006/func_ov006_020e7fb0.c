struct M2 { int a, b; };
extern void func_ov006_020e7910(char* p);
extern struct M2 data_ov006_0213c774;
void func_ov006_020e7fb0(char* c){
  func_ov006_020e7910(c+0x84);
  *(int*)(c+0x208) = 0;
  *(struct M2*)(c+0x210) = data_ov006_0213c774;
}

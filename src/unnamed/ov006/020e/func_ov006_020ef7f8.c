struct P2 { int a, b; };
extern struct P2 data_ov006_0213cc94;
void func_ov006_020ef7f8(char* c){
  *(short*)(c+0x5a74) = 0x78;
  *(int*)(c+0x5a64) = 0;
  *(int*)(c+0x5a68) = 0x4000;
  *(struct P2*)(c+0x5004) = data_ov006_0213cc94;
}

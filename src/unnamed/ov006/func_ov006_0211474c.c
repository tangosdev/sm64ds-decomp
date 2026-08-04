extern int data_ov006_0213ed10;
void func_ov006_0211474c(char* c, int a1, int a2, int* a3){
  int lo, hi;
  *(int*)(c) = (int)&data_ov006_0213ed10;
  *(int*)(c+4) = a1;
  *(int*)(c+0x2c) = a2;
  hi = a3[1]; lo = a3[0];
  *(int*)(c+0x18) = lo;
  *(int*)(c+0x1c) = hi;
  hi = a3[1]; lo = a3[0];
  *(int*)(c+8) = lo;
  *(int*)(c+0xc) = hi;
  *(int*)(c+0x28) = 0;
  *(int*)(c+0x20) = 0;
  *(int*)(c+0x24) = 0;
  *(unsigned char*)(c+0x30) = 1;
}

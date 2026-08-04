extern void func_ov004_020afdd0(void* a0, int a1, int a2, int a3, int a4);
extern int* data_ov006_0213ec04[];
void func_ov006_02110928(char* c){
  int flag = 0;
  if (*(int*)(*(char**)(c+4) + 0x4660)) flag = 1;
  func_ov004_020afdd0(data_ov006_0213ec04[0], *(int*)(c+8)>>12, (*(int*)(c+0xc)>>12)-0x40, -1, flag);
  func_ov004_020afdd0(data_ov006_0213ec04[1], *(int*)(c+8)>>12, (*(int*)(c+0xc)>>12)-0x30, -1, flag);
  func_ov004_020afdd0(data_ov006_0213ec04[1], *(int*)(c+8)>>12, *(int*)(c+0xc)>>12, -1, flag);
  func_ov004_020afdd0(data_ov006_0213ec04[1], *(int*)(c+8)>>12, (*(int*)(c+0xc)>>12)+0x10, -1, flag);
  func_ov004_020afdd0(data_ov006_0213ec04[2], *(int*)(c+8)>>12, (*(int*)(c+0xc)>>12)+0x20, -1, flag);
}

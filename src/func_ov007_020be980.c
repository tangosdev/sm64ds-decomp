extern void func_ov007_020bc3dc(void* c, int i, int a2);
void func_ov007_020be980(char *r0, int i, int a2){
  func_ov007_020bc3dc(*(void**)(r0+4), i, a2);
  *(int*)(*(int*)(r0+4)+0x94)=1;
  *(int*)(*(int*)(r0+4)+0x98)=1;
}

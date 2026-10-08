extern void* func_ov007_020c3d40(int a, int sz);
extern void func_ov007_020c90f8(void* p);
void* func_ov007_020c908c(void* r5){
  char* o = (char*)func_ov007_020c3d40(0, 0x1c);
  *(void**)(o + 0x10) = r5;
  func_ov007_020c90f8(o);
  return o;
}

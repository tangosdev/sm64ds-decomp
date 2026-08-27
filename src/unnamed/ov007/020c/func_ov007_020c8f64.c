extern void* func_ov007_020c3d40(int a, int sz);
extern void func_ov007_020c90f8(void* p);
void* func_ov007_020c8f64(void* r5){
  char* o = (char*)func_ov007_020c3d40(0, 0x2c);
  *(void**)(o + 0x10) = r5;
  func_ov007_020c90f8(o);
  return o;
}

extern void* func_ov007_020c3df4(int a, int b);
extern void func_ov007_020c83fc(void* o);
extern short func_02053200(int x);
void* func_ov007_020c844c(int p, int q){
  int* obj = (int*)func_ov007_020c3df4(0, 0x24);
  func_ov007_020c83fc(obj);
  *obj = p;
  if(q == 0x1000)
    *(short*)((char*)obj+0x1c) = 0x1000;
  else
    *(short*)((char*)obj+0x1c) = func_02053200(q);
  return obj;
}

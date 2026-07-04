//cpp
extern "C" {
extern void func_ov002_020e6fbc(char* c, int arg);
extern void func_ov002_020e9464(char* p);
extern void CollectStarInCurLevel(unsigned int id);
extern void func_02012790(int a);
extern int data_0209b454;
void func_ov002_020e7090(char* c, int arg){
  *(int*)(c+0x438) = arg;
  *(int*)((*(int*)(c+0x438)+0xb0) & 0xFFFFFFFFFFFFFFFF) |= 0x4000000;
  *(int*)(((int)c+0xb0) & 0xFFFFFFFFFFFFFFFF) |= 0x4000000;
  data_0209b454 |= 0x4000000;
  func_ov002_020e6fbc(c, 0);
  *(unsigned char*)(c+0x49c) = 1;
  func_ov002_020e9464(c);
  CollectStarInCurLevel(*(unsigned char*)(c+0x49d));
  func_02012790(0x2d);
}
}

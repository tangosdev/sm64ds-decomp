struct Vector3 { int x; int y; int z; };
extern void AddVec3(struct Vector3* dst, struct Vector3* a, struct Vector3* b);
void func_ov021_021127b4(char* c){
  int sum = *(int*)(c+0xa8) + *(int*)(c+0x9c);
  int m = *(int*)(c+0xa0);
  if (sum >= m) m = sum;
  *(int*)(c+0xa8) = m;
  AddVec3((struct Vector3*)(c+0x5c), (struct Vector3*)(c+0xa4), (struct Vector3*)(c+0x5c));
}

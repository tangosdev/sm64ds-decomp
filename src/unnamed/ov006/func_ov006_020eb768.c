typedef int Fix12i;
typedef struct { int x, y, z; } Vector3;
extern Fix12i func_0203d5dc(const Vector3* a, const Vector3* b);
int func_ov006_020eb768(char* a, const Vector3* b){
  int i;
  const Vector3* p = (const Vector3*)(a + 0x18);
  for (i = 0; i < 5; i++){
    if (func_0203d5dc(p, b) < 0x14000) return 1;
    p = (const Vector3*)((char*)p + 8);
  }
  return 0;
}

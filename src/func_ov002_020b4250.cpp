//cpp
// @symbol func_ov002_020b4250
// @emits BigBrickBlock_AfterClsn
/* recovered: shared common types, renamed to Class_Method */
/* daObjBlockL_c::AfterClsn - recovered from vtable slot identity */
struct V3{int x,y,z;};
extern "C" {
void func_ov002_020b41b8(struct V3*, char*);
int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int, unsigned int, struct V3*, void*, int, int);
void BigBrickBlock_AfterClsn(char* c){
  struct V3 v;
  func_ov002_020b41b8(&v, c);
  _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x115, 0, &v, 0, *(signed char*)(c+0xcc), -1);
}
}

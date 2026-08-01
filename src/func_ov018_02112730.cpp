//cpp
// @symbol func_ov018_02112730
// recovered name: daSCre_c_Behavior
/* recovered: shared common types, renamed to Class_Method */
/* daSCre_c::Behavior - recovered from vtable slot identity */
struct Vector3 { int x,y,z; };
struct Vector3_16 { short x,y,z; };
struct Actor {
  int DistToCPlayer();
  static int Spawn(unsigned int, unsigned int, const Vector3&, const Vector3_16*, int, int);
  void MarkForDestruction();
};
extern "C" int func_ov018_02112730(Actor* c){
  if (c->DistToCPlayer() < 0x64000) {
    Actor::Spawn(0xb2, (*(int*)((char*)c+8) & 0xf) | 0x40, *(Vector3*)((char*)c+0x5c), 0, *(signed char*)((char*)c+0xcc), -1);
  }
  c->MarkForDestruction();
  return 1;
}

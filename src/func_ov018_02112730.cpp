//cpp
// @symbol func_ov018_02112730
// recovered name: daSCre_c_Behavior
/* recovered: shared common types, renamed to Class_Method */
/* daSCre_c::Behavior - verified 2004/b56 byte-match (ov018), strict-reloc */
struct Vector3 { int x,y,z; };
struct Vector3_16 { short x,y,z; };
struct Actor {
  int DistToCPlayer();
  static int Spawn(unsigned int, unsigned int, const Vector3&, const Vector3_16*, signed char, short);
  void MarkForDestruction();
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN9ActorBase18MarkForDestructionEv(void *);

extern "C" int func_ov018_02112730(Actor* c){
  if (c->DistToCPlayer() < 0x64000) {
    Actor::Spawn(0xb2, (*(int*)((char*)c+8) & 0xf) | 0x40, *(Vector3*)((char*)c+0x5c), 0, *(signed char*)((char*)c+0xcc), -1);
  }
  _ZN9ActorBase18MarkForDestructionEv(c);
  return 1;
}

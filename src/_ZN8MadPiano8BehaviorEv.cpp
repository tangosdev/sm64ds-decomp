//cpp
struct Vector3 { int x,y,z; };
struct CylinderClsn;
extern "C" int func_ov063_0211ddf4(void* c);
extern "C" int Vec3_HorzDist(const struct Vector3* a, const struct Vector3* b);
extern "C" void func_020383fc(void* p);
extern "C" void func_0203568c(void* p, int v);
extern "C" void func_02035684(void* p, int v);
extern "C" void func_ov063_0211d88c(char* t);
extern "C" void func_ov063_0211d828(char* self);
extern "C" void func_ov063_0211d5f4(void* self);

struct Actor {
  void UpdatePos(struct CylinderClsn*);
  struct Actor* ClosestPlayer();
};
struct MeshColliderBase {
  int IsEnabled();
  void Disable();
};
struct Platform {
  int IsClsnInRange(int, int);
};

extern "C" int _ZN8MadPiano8BehaviorEv(char* self)
{
  func_ov063_0211ddf4(self);
  ((struct Actor*)self)->UpdatePos(0);
  if (Vec3_HorzDist((struct Vector3*)(self+0x6d4), (struct Vector3*)(self+0x5c)) > 0x180000) {
    *(int*)(self+0x5c) = *(int*)(self+0x68);
    *(int*)(self+0x60) = *(int*)(self+0x6c);
    *(int*)(self+0x64) = *(int*)(self+0x70);
  }
  {
    int r1 = *(int*)(self+0x6cc);
    if (*(int*)(self+0x60) <= r1) *(int*)(self+0x60) = r1;
  }
  func_020383fc(self+0x50c);
  func_0203568c(self+0x50c, 0x159000);
  func_02035684(self+0x50c, 0x159000);
  if (*(unsigned char*)((char*)((struct Actor*)self)->ClosestPlayer()+0x6fb) != 0) {
    if (((struct MeshColliderBase*)(self+0x124))->IsEnabled() != 0)
      ((struct MeshColliderBase*)(self+0x124))->Disable();
  } else {
    ((struct Platform*)self)->IsClsnInRange(0x1f4000, 0);
  }
  func_ov063_0211d88c(self);
  func_ov063_0211d828(self);
  func_ov063_0211d5f4(self);
  return 1;
}

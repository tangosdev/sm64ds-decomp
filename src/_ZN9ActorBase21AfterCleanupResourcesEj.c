//cpp
#include "types.h"
/* ActorBase::AfterCleanupResources(u32 vfSuccess) at 0x02043b2c
 *
 * Only runs when vfSuccess == VS_SUCCESS (2); otherwise returns immediately.
 *   func_0203b3c0(&gGlobalA, &this->sceneNode);   (this+0x14)
 *   func_0203b27c(&gGlobalB, &this->behavNode);   (this+0x28)
 *   if (this->unk4C) Heap::_Destroy(this->unk4C);
 *   if (this->unk48) func_02044334(this->unk48);
 *   this->~ActorBase();                            virtual call at vtable+0x40
 *   Memory::Deallocate(this, Memory::gameHeapPtr);
 *
 * NOTE: compiled as C++ (the virtual-call codegen `mov r0,r4; ldr r1,[r0]`
 * only reproduces from a real C++ virtual dispatch; a function-pointer-through-
 * a-data-field reads the vtable from r4 directly and swaps those two words).
 */
struct Heap;
struct SceneNode { char b[0x14]; };
struct PListNode { char b[0x10]; };

extern "C" {
  char gGlobalA;                 /* 0x020a4b6c */
  char gGlobalB;                 /* 0x020a4ba8 */
  void func_0203b3c0(void*, void*);
  void func_0203b27c(void*, void*);
  void Heap_Destroy(void*);              /* 0x0203c74c = Heap::_Destroy */
  void func_02044334(void*);
  void Memory_Deallocate(void*, struct Heap*); /* 0x0203c1e8 */
  struct Heap* Memory_gameHeapPtr;             /* 0x020a0eac */
}

struct ActorBase {
  /* 0x00 vtable */
  u32 uniqueID;            /* 0x04 */
  u32 param1;              /* 0x08 */
  unsigned short actorID;  /* 0x0c */
  unsigned char aliveState;/* 0x0e */
  unsigned char shouldBeKilled; /* 0x0f */
  unsigned char unk10, unk11, unk12, unk13; /* 0x10..0x13 */
  SceneNode sceneNode;     /* 0x14 */
  PListNode behavNode;     /* 0x28 */
  PListNode renderNode;    /* 0x38 */
  void* unk48;             /* 0x48 */
  void* unk4C;             /* 0x4c */

  virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
  virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
  virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
  virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
  virtual void Destructor();        /* index 16 -> vtable+0x40 = ~ActorBase (D1).
                                       NOT OnPendingDestroy, which is slot 12 /
                                       vtable+0x30 -- see notes/actor-vtables.md. */
  virtual void AfterCleanupResources(u32 vfSuccess);
};

void ActorBase::AfterCleanupResources(u32 vfSuccess) {
  if (vfSuccess != 2) return;
  func_0203b3c0(&gGlobalA, &this->sceneNode);
  func_0203b27c(&gGlobalB, &this->behavNode);
  if (this->unk4C) Heap_Destroy(this->unk4C);
  if (this->unk48) func_02044334(this->unk48);
  this->Destructor();   /* ~ActorBase, then free below */
  Memory_Deallocate(this, Memory_gameHeapPtr);
}

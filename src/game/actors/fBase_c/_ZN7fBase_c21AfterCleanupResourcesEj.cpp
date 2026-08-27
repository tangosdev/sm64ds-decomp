//cpp
#include "types.h"
/* fBase_c::AfterCleanupResources(u32 vfSuccess) at 0x02043b2c
 *
 * Only runs when vfSuccess == VS_SUCCESS (2); otherwise returns immediately.
 *   func_0203b3c0(&data_020a4b6c, &this->sceneNode);   (this+0x14)
 *   func_0203b27c(&data_020a4ba8, &this->behavNode);   (this+0x28)
 *   if (this->unk4C) Heap::_Destroy(this->unk4C);
 *   if (this->unk48) func_02044334(this->unk48);
 *   this->~fBase_c();                            virtual call at vtable+0x40
 *   Memory::Deallocate(this, Memory::gameHeapPtr);
 *
 * NOTE: compiled as C++ (the virtual-call codegen `mov r0,r4; ldr r1,[r0]`
 * only reproduces from a real C++ virtual dispatch; a function-pointer-through-
 * a-data-field reads the vtable from r4 directly and swaps those two words).
 */
/* Both of these used to be called as `Heap_Destroy` and `Memory_Deallocate`, names no
   module defines. The mangled names say what they really are, and both model as
   ordinary C++ -- every parameter is a pointer, so unlike the Fix12<int> methods
   elsewhere in this sweep there is nothing here that forces an extern "C" escape.
   _Destroy takes no arguments: the operand is the implicit `this`. Deallocate carries
   both of its parameters in the mangled name, so it is static. */
struct Heap { void _Destroy(); };                       /* 0x0203c74c */
struct Memory { static void Deallocate(void*, Heap*); };/* 0x0203c1e8 */
struct SceneNode { char b[0x14]; };
struct PListNode { char b[0x10]; };

extern "C" {
#ifdef SM64DS_PLATFORM_PC
  extern char data_020a4b6c;          /* host storage is supplied by actor_vtables.cpp */
#else
  char data_020a4b6c;                 /* 0x020a4b6c */
#endif
  char data_020a4ba8;                 /* 0x020a4ba8 */
  void func_0203b3c0(void*, void*);
  void func_0203b27c(void*, void*);
  void func_02044334(void*);
  Heap* data_020a0eac;                /* 0x020a0eac = Memory::gameHeapPtr */
}

struct fBase_c {
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
  virtual void Destructor();        /* index 16 -> vtable+0x40 = ~fBase_c (D1).
                                       NOT OnPendingDestroy, which is slot 12 /
                                       vtable+0x30 -- see notes/actor-vtables.md. */
  virtual void AfterCleanupResources(u32 vfSuccess);
};

void fBase_c::AfterCleanupResources(u32 vfSuccess) {
  if (vfSuccess != 2) return;
  func_0203b3c0(&data_020a4b6c, &this->sceneNode);
  func_0203b27c(&data_020a4ba8, &this->behavNode);
  if (this->unk4C) ((Heap*)this->unk4C)->_Destroy();
  if (this->unk48) func_02044334(this->unk48);
  this->Destructor();   /* ~fBase_c, then free below */
  Memory::Deallocate(this, data_020a0eac);
}

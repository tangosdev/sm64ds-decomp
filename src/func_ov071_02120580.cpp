//cpp
extern "C" {
extern int _ZN6Player15IsCollectingCapEv(void* p);
extern void _ZN5Actor15GivePlayerCoinsER6Playerhj(void* a, void* p, unsigned char h, unsigned int j);
extern void _ZN6Player20RegisterEggCoinCountEjbb(void* p, unsigned int n, int b1, int b2);
extern void func_ov071_021202ec(void* a, int idx);
extern void _ZN9ActorBase18MarkForDestructionEv(void* a);
void func_ov071_02120580(char* a, void* p) {
  volatile int force_stack;
  int *bp;
  int t;
  if (_ZN6Player15IsCollectingCapEv(p))
    _ZN5Actor15GivePlayerCoinsER6Playerhj(a, p, *(unsigned char*)(a + 0x3aa), 0);
  else
    _ZN6Player20RegisterEggCoinCountEjbb(p, *(unsigned char*)(a + 0x3aa), 0, 0);
  if (*(int*)(a + 8) != 0) {
    a[0x3aa] = 0;
    bp = (int *)(((int)(a) + 0xb0) & 0xFFFFFFFFFFFFFFFFULL);
    t = *bp;
    t &= ~0x40000;
    *bp = t;
    func_ov071_021202ec(a, 0);
    return;
  } else {
    _ZN9ActorBase18MarkForDestructionEv(a);
    return;
  }
}
}

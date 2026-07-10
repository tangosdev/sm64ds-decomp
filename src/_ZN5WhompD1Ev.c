extern void _ZN18MovingMeshColliderD1Ev(void*);
extern void _ZN11ShadowModelD1Ev(void*);
extern void _ZN15TextureSequenceD1Ev(void*);
extern void _ZN9ModelAnimD1Ev(void*);
extern void _ZN12WithMeshClsnD1Ev(void*);
extern int func_ov002_020aed18(int* x);
extern int _ZTV5Whomp[];
void* _ZN5WhompD1Ev(char* r4) {
  *(int*)r4 = (int)_ZTV5Whomp;
  _ZN18MovingMeshColliderD1Ev(r4 + 0x418);
  _ZN11ShadowModelD1Ev(r4 + 0x344);
  _ZN15TextureSequenceD1Ev(r4 + 0x330);
  _ZN9ModelAnimD1Ev(r4 + 0x2cc);
  _ZN12WithMeshClsnD1Ev(r4 + 0x110);
  func_ov002_020aed18((int*)r4);
  return r4;
}

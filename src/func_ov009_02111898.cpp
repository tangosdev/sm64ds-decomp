//cpp
extern "C" {
extern void Vec3_Asr(void* dst, void* src, int n);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZ(void* m, short a);
extern void Matrix4x3_ApplyInPlaceToRotationY(void* m, short a);
extern int _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* thiz, void* sm, void* m, int rad, int h, unsigned u);
extern int _ZN9Animation7AdvanceEv(void* a);
struct PMF { int fn; int ptr; };
struct Mtx { int w[12]; };
extern struct PMF data_ov036_02113c48[];
extern struct Mtx MATRIX_SCRATCH_PAPER;
int func_ov009_02111898(char* c){
  int idx = *(int*)(c+0x17c);
  struct PMF* m = &data_ov036_02113c48[idx];
  char* obj = c + (m->ptr >> 1);
  int p = m->ptr;
  void (*f)(void*);
  if(p & 1){
    f = *(void(**)(void*))(*(int*)obj + m->fn);
  } else {
    f = (void(*)(void*))m->fn;
  }
  f(obj);
  int tmp[3];
  Vec3_Asr(tmp, c+0x5c, 3);
  Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, tmp[0], tmp[1], tmp[2]);
  *(short*)(c+0x8e) = *(short*)(c+0x94);
  Matrix4x3_ApplyInPlaceToRotationZ(&MATRIX_SCRATCH_PAPER, *(short*)(c+0x90));
  Matrix4x3_ApplyInPlaceToRotationY(&MATRIX_SCRATCH_PAPER, *(short*)(c+0x8e));
  *(struct Mtx*)(c+0xf0) = MATRIX_SCRATCH_PAPER;
  _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c+0x138, c+0xf0, 0x1e000, 0x7d0000, 0xf);
  _ZN9Animation7AdvanceEv(c+0x124);
  return 1;
}
}

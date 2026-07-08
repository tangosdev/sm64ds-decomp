//cpp
struct V { virtual void v0(); virtual void v1(); };
struct Elem { void* a; void* b; char pad[8]; };
extern Elem data_ov100_02148204[];
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void* p);
extern void UnloadKeyModels(int i);
extern void* DOOR_OPEN_ANIM_PTR;
int func_ov100_0214542c(char* c) {
  int idx = *(int*)(c + 8);
  Elem* e = &data_ov100_02148204[idx];
  V* obj;
  _ZN13SharedFilePtr7ReleaseEv(e->a);
  _ZN13SharedFilePtr7ReleaseEv(&DOOR_OPEN_ANIM_PTR);
  obj = (V*)*(void**)(c + 0x138);
  if (obj != 0) {
    if (obj != 0) obj->v1();
    _ZN13SharedFilePtr7ReleaseEv(e->b);
  }
  if (*(void**)(c + 0x13c) != 0) {
    unsigned int v = *(unsigned int*)(c + 8);
    if (v >= 9 && v <= 0xd) UnloadKeyModels(v - 7);
    _ZN13SharedFilePtr7ReleaseEv(*(void**)(c + 0x13c));
  }
  return 1;
}
}

typedef int Fix12i;
struct Player { char p[0x400]; };
int func_ov002_020e3078(struct Player* self, void* s) {
  return *(void**)((char*)self+0x374) == s;
}

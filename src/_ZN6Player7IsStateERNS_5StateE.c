typedef int Fix12i;
struct Player { char p[0x400]; };
int _ZN6Player7IsStateERNS_5StateE(struct Player* self, void* s) {
  return *(void**)((char*)self+0x370) == s;
}

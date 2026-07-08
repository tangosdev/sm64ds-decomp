extern int _ZN13SharedFilePtr7ReleaseEv(void*);
extern void UnloadSilverStarAndNumber(void);
extern char ONE_UP_MUSHROOM_MODEL_PTR[];
extern char SUPER_MUSHROOM_MODEL_PTR[];
extern char SHELL_MODEL_PTRS[];
int func_ov002_020b4394(char* c){
  int v = *(unsigned short*)(c+0xc);
  switch(v){
  case 0x141: _ZN13SharedFilePtr7ReleaseEv(ONE_UP_MUSHROOM_MODEL_PTR); break;
  case 0x142: _ZN13SharedFilePtr7ReleaseEv(SUPER_MUSHROOM_MODEL_PTR); break;
  case 0x143: _ZN13SharedFilePtr7ReleaseEv(SHELL_MODEL_PTRS); break;
  case 0x144: UnloadSilverStarAndNumber(); break;
  }
  return 1;
}

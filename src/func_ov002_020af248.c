extern int _ZN8dActor_c24KillAndTrackInDeathTableEv(void*);
int func_ov002_020af248(char* c, int n){
  int v = *(unsigned short*)(c+0x38c);
  if(v < n) return 0;
  if(v < n + 0x28){
    *(unsigned char*)(c+0x38f) = (v & 1) != 0;
  } else {
    _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
    return 1;
  }
  return 0;
}

typedef long long s64;
extern s64 func_02059650(void);
void _ZN5Timer9StopTimerEv(char *c){
  if (*(unsigned char*)(c+8) == 0)
    return;
  *(unsigned char*)(c+8) = 0;
  *(s64*)c = func_02059650() - *(s64*)c;
}

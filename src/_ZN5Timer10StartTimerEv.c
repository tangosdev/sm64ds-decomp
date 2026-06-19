typedef long long s64;
extern s64 func_02059650(void);
void _ZN5Timer10StartTimerEv(char *c){
  *(unsigned char*)(c+8) = 1;
  *(s64*)c = func_02059650() - *(s64*)c;
}

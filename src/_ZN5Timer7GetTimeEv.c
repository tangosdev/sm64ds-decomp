typedef long long s64;
extern s64 func_02059650(void);
s64 _ZN5Timer7GetTimeEv(char *c){
  if (*(unsigned char*)(c+8) == 0)
    return *(s64*)c;
  return func_02059650() - *(s64*)c;
}

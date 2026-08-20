extern int _ZN4dBgW9IsEnabledEv(void *);
extern void _ZN4dBgW7DisableEv(void *);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
int func_ov002_020b4b6c(char *t, void **f){
  int i;
  char *m=t+0x4b0;
  for(i=0;i<5;i++){
    if(_ZN4dBgW9IsEnabledEv(m))
      _ZN4dBgW7DisableEv(m);
    _ZN13SharedFilePtr7ReleaseEv(f[0]);
    _ZN13SharedFilePtr7ReleaseEv(f[1]);
    m+=0x1c8;
    f+=3;
  }
  return 1;
}

extern int _ZN4dBgW9IsEnabledEv(void *);
extern void _ZN4dBgW7DisableEv(void *);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
int func_ov080_021270dc(char *t, void **f){
  if(_ZN4dBgW9IsEnabledEv(t+0x124))
    _ZN4dBgW7DisableEv(t+0x124);
  _ZN13SharedFilePtr7ReleaseEv(f[0]);
  _ZN13SharedFilePtr7ReleaseEv(f[1]);
  return 1;
}

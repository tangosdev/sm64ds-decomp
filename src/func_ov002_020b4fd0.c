extern int _ZN4dBgW9IsEnabledEv(void*);
extern int _ZN4dBgW7DisableEv(void*);
extern int _ZN4dBgW6EnableEP8dActor_c(void*, char*);
int func_ov002_020b4fd0(char* c){
  int b = (int)((*(int*)(c+0xb0) & 8) != 0);
  if(b){
    if(_ZN4dBgW9IsEnabledEv(*(void**)(c+0x320)))
      _ZN4dBgW7DisableEv(*(void**)(c+0x320));
    return 1;
  }
  if(!_ZN4dBgW9IsEnabledEv(*(void**)(c+0x320)))
    _ZN4dBgW6EnableEP8dActor_c(*(void**)(c+0x320), c);
  return 0;
}

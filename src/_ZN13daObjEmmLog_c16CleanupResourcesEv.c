// @symbol _ZN13daObjEmmLog_c16CleanupResourcesEv
// recovered name: daObjEmmLog_c_CleanupResources
/* recovered: renamed to Class_Method */
/* daObjEmmLog_c::CleanupResources - name recovered from the vtable slot it fills.
   The body is a decompilation verified against the ROM, not an
   inferred stub. */
extern int _ZN4dBgW9IsEnabledEv();
extern int _ZN4dBgW7DisableEv();
extern int _ZN13SharedFilePtr7ReleaseEv();
extern int *data_ov052_021124d4[];
int _ZN13daObjEmmLog_c16CleanupResourcesEv(char *c){
  if(_ZN4dBgW9IsEnabledEv((char*)c+0x124))
    _ZN4dBgW7DisableEv((char*)c+0x124);
  _ZN13SharedFilePtr7ReleaseEv(data_ov052_021124d4[0]);
  _ZN13SharedFilePtr7ReleaseEv(data_ov052_021124d4[1]);
  return 1;
}

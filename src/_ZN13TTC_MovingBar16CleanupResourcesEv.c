extern int _ZN16MeshColliderBase9IsEnabledEv(void *);
extern void _ZN16MeshColliderBase7DisableEv(void *);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern char data_ov065_0211d35c[];
extern char data_ov065_0211d360[];
int _ZN13TTC_MovingBar16CleanupResourcesEv(char *t){
  if(_ZN16MeshColliderBase9IsEnabledEv(t+0x124))
    _ZN16MeshColliderBase7DisableEv(t+0x124);
  _ZN13SharedFilePtr7ReleaseEv(*(void**)(data_ov065_0211d35c + (unsigned char)t[0x31e]*0xc));
  _ZN13SharedFilePtr7ReleaseEv(*(void**)(data_ov065_0211d360 + (unsigned char)t[0x31e]*0xc));
  return 1;
}

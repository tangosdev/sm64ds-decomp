extern int _ZN16MeshColliderBase9IsEnabledEv(void *);
extern void _ZN16MeshColliderBase7DisableEv(void *);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern char data_ov102_0214e188[];
extern char data_ov102_0214e18c[];
int _ZN13FortressTower16CleanupResourcesEv(char *t){
  if(_ZN16MeshColliderBase9IsEnabledEv(t+0x124))
    _ZN16MeshColliderBase7DisableEv(t+0x124);
  _ZN13SharedFilePtr7ReleaseEv(*(void**)(data_ov102_0214e188 + (unsigned char)t[0x31e]*0xc));
  _ZN13SharedFilePtr7ReleaseEv(*(void**)(data_ov102_0214e18c + (unsigned char)t[0x31e]*0xc));
  return 1;
}

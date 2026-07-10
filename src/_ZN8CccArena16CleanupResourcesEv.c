extern int _ZN16MeshColliderBase9IsEnabledEv(void *);
extern void _ZN16MeshColliderBase7DisableEv(void *);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern char data_ov073_021231bc[];
extern char data_ov073_021231c0[];
int _ZN8CccArena16CleanupResourcesEv(char *t){
  if(_ZN16MeshColliderBase9IsEnabledEv(t+0x124))
    _ZN16MeshColliderBase7DisableEv(t+0x124);
  _ZN13SharedFilePtr7ReleaseEv(*(void**)(data_ov073_021231bc + (unsigned char)t[0x32c]*0xc));
  _ZN13SharedFilePtr7ReleaseEv(*(void**)(data_ov073_021231c0 + (unsigned char)t[0x32c]*0xc));
  return 1;
}

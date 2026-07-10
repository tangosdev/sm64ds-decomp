extern int _ZN16MeshColliderBase9IsEnabledEv(void *);
extern void _ZN16MeshColliderBase7DisableEv(void *);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern char data_ov002_02108ab0[];
extern char data_ov002_02108ab4[];
int _ZN13BigBrickBlock16CleanupResourcesEv(char *t){
  if(_ZN16MeshColliderBase9IsEnabledEv(t+0x124))
    _ZN16MeshColliderBase7DisableEv(t+0x124);
  _ZN13SharedFilePtr7ReleaseEv(*(void**)(data_ov002_02108ab0 + (unsigned char)t[0x32c]*0xc));
  _ZN13SharedFilePtr7ReleaseEv(*(void**)(data_ov002_02108ab4 + (unsigned char)t[0x32c]*0xc));
  return 1;
}

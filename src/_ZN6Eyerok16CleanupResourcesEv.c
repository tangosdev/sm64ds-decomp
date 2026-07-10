extern int _ZN16MeshColliderBase9IsEnabledEv(void *);
extern void _ZN16MeshColliderBase7DisableEv(void *);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern char data_ov066_0211ae6c[];
extern char data_ov066_0211ae4c[];
extern char data_ov066_0211aeb4[];
extern char data_ov066_0211aebc[];
extern char data_ov066_0211ae9c[];
extern char data_ov066_0211ae3c[];
extern char data_ov066_0211ae2c[];
extern char data_ov066_0211ae5c[];
extern char data_ov066_0211ae84[];
extern char data_ov066_0211aea4[];
extern char data_ov066_0211ae8c[];
extern char data_ov066_0211ae54[];
extern char data_ov066_0211ae94[];
extern char data_ov066_0211ae64[];
extern char data_ov066_0211ae44[];
extern char data_ov066_0211ae74[];
extern char data_ov066_0211ae7c[];
extern char data_ov066_0211ae24[];
extern char data_ov066_0211aeac[];
extern char data_ov066_0211ae14[];
extern char data_ov066_0211ae1c[];
extern char data_ov066_0211ae34[];

int _ZN6Eyerok16CleanupResourcesEv(char *c){
  if(_ZN16MeshColliderBase9IsEnabledEv(c+0x674))
    _ZN16MeshColliderBase7DisableEv(c+0x674);
  if(*(int*)(c+0x49c)==0){
    _ZN13SharedFilePtr7ReleaseEv(data_ov066_0211ae6c);
    _ZN13SharedFilePtr7ReleaseEv(data_ov066_0211ae4c);
    _ZN13SharedFilePtr7ReleaseEv(data_ov066_0211aeb4);
    _ZN13SharedFilePtr7ReleaseEv(data_ov066_0211aebc);
    _ZN13SharedFilePtr7ReleaseEv(data_ov066_0211ae9c);
    _ZN13SharedFilePtr7ReleaseEv(data_ov066_0211ae3c);
    _ZN13SharedFilePtr7ReleaseEv(data_ov066_0211ae2c);
    _ZN13SharedFilePtr7ReleaseEv(data_ov066_0211ae5c);
    _ZN13SharedFilePtr7ReleaseEv(data_ov066_0211ae84);
    _ZN13SharedFilePtr7ReleaseEv(data_ov066_0211aea4);
    _ZN13SharedFilePtr7ReleaseEv(data_ov066_0211ae8c);
    _ZN13SharedFilePtr7ReleaseEv(data_ov066_0211ae54);
    _ZN13SharedFilePtr7ReleaseEv(data_ov066_0211ae94);
    _ZN13SharedFilePtr7ReleaseEv(data_ov066_0211ae64);
    _ZN13SharedFilePtr7ReleaseEv(data_ov066_0211ae44);
    _ZN13SharedFilePtr7ReleaseEv(data_ov066_0211ae74);
    _ZN13SharedFilePtr7ReleaseEv(data_ov066_0211ae7c);
    _ZN13SharedFilePtr7ReleaseEv(data_ov066_0211ae24);
    _ZN13SharedFilePtr7ReleaseEv(data_ov066_0211aeac);
    _ZN13SharedFilePtr7ReleaseEv(data_ov066_0211ae14);
    _ZN13SharedFilePtr7ReleaseEv(data_ov066_0211ae1c);
    _ZN13SharedFilePtr7ReleaseEv(data_ov066_0211ae34);
  }
  return 1;
}

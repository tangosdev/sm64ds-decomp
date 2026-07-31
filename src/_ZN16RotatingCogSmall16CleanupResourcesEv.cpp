//cpp
extern "C" {
extern int _ZN16MeshColliderBase9IsEnabledEv(void *);
extern void _ZN16MeshColliderBase7DisableEv(void *);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern char data_ov035_02112c78[];
extern char data_ov056_02112c68[];
extern char data_ov035_02112c70[];
extern char data_ov035_02112c60[];
}

struct RotatingCogSmall {
    int CleanupResources();
};

int RotatingCogSmall::CleanupResources()
{
    char * t = (char *)this;
if(*(int*)(t+0x32c)==0){
    if(_ZN16MeshColliderBase9IsEnabledEv(t+0x124))
      _ZN16MeshColliderBase7DisableEv(t+0x124);
    _ZN13SharedFilePtr7ReleaseEv(data_ov035_02112c78);
    _ZN13SharedFilePtr7ReleaseEv(data_ov056_02112c68);
  } else {
    int on = (*(unsigned short*)(t+0xc)==0x79);
    if(on)
      _ZN13SharedFilePtr7ReleaseEv(data_ov035_02112c70);
    else
      _ZN13SharedFilePtr7ReleaseEv(data_ov035_02112c60);
  }
  return 1;

}

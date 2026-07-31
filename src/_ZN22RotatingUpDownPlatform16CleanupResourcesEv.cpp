//cpp
extern "C" {
extern int _ZN13SharedFilePtr7ReleaseEv(void*);
extern int _ZN16MeshColliderBase7DisableEv(void*);
extern void* data_ov091_021344fc[];
extern void* data_ov091_021344f4[];
}

struct RotatingUpDownPlatform {
    int CleanupResources();
};

int RotatingUpDownPlatform::CleanupResources()
{
    char* c = (char*)this;
_ZN13SharedFilePtr7ReleaseEv(data_ov091_021344fc[*(unsigned char*)(c+0x352)]);
  _ZN13SharedFilePtr7ReleaseEv(data_ov091_021344f4[*(unsigned char*)(c+0x352)]);
  _ZN16MeshColliderBase7DisableEv(c+0x124);
  return 1;

}

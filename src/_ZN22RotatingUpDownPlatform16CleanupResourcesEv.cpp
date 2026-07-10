//cpp
extern "C" {
extern int _ZN13SharedFilePtr7ReleaseEv(void*);
extern int _ZN16MeshColliderBase7DisableEv(void*);
extern void* data_ov091_021344fc[];
extern void* data_ov091_021344f4[];
int _ZN22RotatingUpDownPlatform16CleanupResourcesEv(char* c){
  _ZN13SharedFilePtr7ReleaseEv(data_ov091_021344fc[*(unsigned char*)(c+0x352)]);
  _ZN13SharedFilePtr7ReleaseEv(data_ov091_021344f4[*(unsigned char*)(c+0x352)]);
  _ZN16MeshColliderBase7DisableEv(c+0x124);
  return 1;
}
}

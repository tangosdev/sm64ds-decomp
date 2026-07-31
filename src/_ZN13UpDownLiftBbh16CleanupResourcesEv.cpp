//cpp
extern "C" {
extern int _ZN16MeshColliderBase9IsEnabledEv(void*);
extern void _ZN16MeshColliderBase7DisableEv(void*);
extern void _ZN13SharedFilePtr7ReleaseEv(void*);
extern void* data_ov095_02136f68[];
extern void* data_ov095_02136f74[];
}

struct UpDownLiftBbh {
    int CleanupResources();
};

int UpDownLiftBbh::CleanupResources()
{
    void* c = (void*)this;
if(_ZN16MeshColliderBase9IsEnabledEv((char*)c+0x124))
    _ZN16MeshColliderBase7DisableEv((char*)c+0x124);
  _ZN13SharedFilePtr7ReleaseEv(data_ov095_02136f68[*(int*)((char*)c+0x328)]);
  _ZN13SharedFilePtr7ReleaseEv(data_ov095_02136f74[*(int*)((char*)c+0x328)]);
  return 1;

}

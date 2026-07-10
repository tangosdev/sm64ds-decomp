extern int _ZN16MeshColliderBase9IsEnabledEv(void* self);
extern void _ZN16MeshColliderBase7DisableEv(void* self);
extern void _ZN13SharedFilePtr7ReleaseEv(void* self);
extern void* data_ov065_0211c0a8[];
struct E { void* p; char pad[8]; };
extern struct E data_ov065_0211cfd0[];
extern struct E data_ov065_0211cfd4[];

int _ZN15TtcRotatingCube16CleanupResourcesEv(char* c)
{
    if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x124)) {
        _ZN16MeshColliderBase7DisableEv(c + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov065_0211c0a8[*(unsigned char*)(c + 0x377)]);
    _ZN13SharedFilePtr7ReleaseEv(data_ov065_0211cfd0[*(unsigned char*)(c + 0x377)].p);
    _ZN13SharedFilePtr7ReleaseEv(data_ov065_0211cfd4[*(unsigned char*)(c + 0x377)].p);
    return 1;
}

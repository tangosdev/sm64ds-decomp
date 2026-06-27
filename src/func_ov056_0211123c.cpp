//cpp
extern "C" int _ZN16MeshColliderBase9IsEnabledEv(void *);
extern "C" void _ZN16MeshColliderBase7DisableEv(void *);
extern "C" void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov056_02113314;

extern "C" int func_ov056_0211123c(char *c) {
    if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x124)) {
        _ZN16MeshColliderBase7DisableEv(c + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv((void *)((int *)&data_ov056_02113314)[0]);
    _ZN13SharedFilePtr7ReleaseEv((void *)((int *)&data_ov056_02113314)[1]);
    return 1;
}

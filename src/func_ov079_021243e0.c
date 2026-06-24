extern int _ZN16MeshColliderBase9IsEnabledEv(void* m);
extern void _ZN16MeshColliderBase7DisableEv(void* m);
extern void _ZN16MeshColliderBase6EnableEP5Actor(void* m, void* actor);
extern void* _ZN5Actor13ClosestPlayerEv(void* c);
extern int Vec3_Dist(void* a, void* b);

struct Vec3 { int x, y, z; };

int func_ov079_021243e0(char* c, int r4)
{
    struct Vec3 v;
    char* player;
    int dist;
    int y;

    if (*(unsigned char*)(c + 0x404) == 0) {
        if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x418))
            _ZN16MeshColliderBase7DisableEv(c + 0x418);
        return 1;
    }

    if (*(unsigned char*)(c + 0x414) != 0 && *(int*)(c + 0x3b0) != 0) {
        if (!_ZN16MeshColliderBase9IsEnabledEv(c + 0x418))
            _ZN16MeshColliderBase6EnableEP5Actor(c + 0x418, c);
        return 0;
    }

    v.x = *(int*)(c + 0x5c);
    y = *(int*)(c + 0x60);
    v.y = y;
    v.z = *(int*)(c + 0x64);
    if (r4 == 0) {
        int z2 = *(int*)(c + 0xb8);
        v.y = y + *(int*)(c + 0xb4);
        r4 = z2 << 3;
    }

    player = (char*)_ZN5Actor13ClosestPlayerEv(c);
    dist = Vec3_Dist(&v, player + 0x5c);
    if (dist > r4) {
        if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x418))
            _ZN16MeshColliderBase7DisableEv(c + 0x418);
        return 1;
    }

    if (!_ZN16MeshColliderBase9IsEnabledEv(c + 0x418))
        _ZN16MeshColliderBase6EnableEP5Actor(c + 0x418, c);
    return 0;
}

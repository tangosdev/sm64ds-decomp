struct Vector3 { int x, y, z; };
extern void _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(void *thiz, struct Vector3 *v, int f);
extern short Vec3_HorzAngle(struct Vector3 *a, struct Vector3 *b);
extern int _ZN16MeshColliderBase9IsEnabledEv(void *c);
extern void _ZN16MeshColliderBase7DisableEv(void *c);
void _ZN8Platform14KillByMegaCharER6Player(char *c, char *player) {
    struct Vector3 v;
    v.x = *(int*)(c + 0x5c);
    v.y = *(int*)(c + 0x60);
    v.z = *(int*)(c + 0x64);
    _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(c, &v, 0x5dc000);
    *(unsigned char*)(c + 0x31c) = 1;
    *(int*)(c + 0x9c) = -0x2000;
    *(int*)(c + 0xa0) = -0x3c000;
    *(int*)(c + 0x98) = 0x14000;
    *(int*)(c + 0xa8) = 0x1e000;
    *(short*)(c + 0x94) = Vec3_HorzAngle((struct Vector3*)(player + 0x5c), (struct Vector3*)(c + 0x5c));
    *(unsigned char*)(c + 0x31d) = 0x1e;
    if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x124) == 0) return;
    _ZN16MeshColliderBase7DisableEv(c + 0x124);
}

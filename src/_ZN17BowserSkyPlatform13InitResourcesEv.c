//cpp
// NONMATCHING: base materialization / addressing (div=12). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct Vector3 { int x, y, z; };

extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *sfp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, void *f, int a, int b);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
    void *thiz, void *actor, const Vector3 &v, int radius, int height, unsigned a, unsigned b);
extern short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern int Vec3_HorzLen(const Vector3 *v);
extern int AddSpikeBomb(void *p);

extern void *data_ov060_0211b1c4;

int _ZN17BowserSkyPlatform13InitResourcesEv(char *c) {
    Vector3 v;
    Vector3 z;
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov060_0211b1c4), 1, -1);
    v.x = 0;
    v.y = -0x96000;
    v.z = 0;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
        c + 0x124, c, v, 0x96000, 0x12c000, 0x204004, 0);
    *(int *)(c + 0x80) = 0x1000;
    *(int *)(c + 0x84) = 0x1000;
    *(int *)(c + 0x88) = 0x1000;
    *(unsigned char *)(c + 0x1ae) = 0xff;
    z.x = 0;
    z.y = 0;
    z.z = 0;
    Vec3_HorzAngle(&z, (const Vector3 *)(c + 0x5c));
    int *p178 = (int *)(c + 0x178);
    *(int *)(c + 0x184) = 0x2ee000;
    *(int *)(c + 0x174) = *(int *)(c + 0x5c);
    *p178 = *(int *)(c + 0x60);
    *(int *)(c + 0x17c) = *(int *)(c + 0x64);
    *p178 = *p178 + (*(int *)(c + 0x184) >> 3);
    *(int *)(c + 0x180) = Vec3_HorzLen((const Vector3 *)(c + 0x5c));
    *(int *)(c + 0x170) = 0;
    *(int *)(c + 0x1a8) = AddSpikeBomb(c);
    return 1;
}
}

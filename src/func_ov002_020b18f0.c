typedef int Fix12i;
struct Vector3 { Fix12i x, y, z; };
struct Vector3_16;
extern int _ZN5Event6GetBitEj(unsigned int b);
extern int SublevelToLevel(int i);
extern void _ZN5Event6SetBitEj(unsigned int b);
extern void _ZN9PowerStar13AddStarMarkerEv(char* c);
extern char* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, struct Vector3* v, struct Vector3_16* rot, int e, int f);
extern signed char data_0209f2f8;
extern short data_0209f358[];

void func_ov002_020b18f0(char* c)
{
    char* r;
    struct Vector3 vec;
    struct Vector3* p;
    int y;
    if (_ZN5Event6GetBitEj(0x1f)) return;
    if (SublevelToLevel(data_0209f2f8) >= 0xf) return;
    if (c == 0) return;
    if (data_0209f358[*(unsigned char*)(c + 0x6d8)] < 0x64) return;
    p = (struct Vector3*)(((long long)(int)(c + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
    vec.x = p->x;
    y = p->y;
    vec.y = y;
    vec.z = p->z;
    vec.y = y + 0x12c000;
    r = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xb2, 0x20, &vec, 0, *(signed char*)(c + 0xcc), -1);
    if (r == 0) return;
    _ZN5Event6SetBitEj(0x1f);
    _ZN9PowerStar13AddStarMarkerEv(r);
}

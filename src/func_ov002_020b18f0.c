// NONMATCHING: base materialization / addressing (div=14). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef int Fix12i;
struct Vector3 { Fix12i x, y, z; };
struct Vector3_16;
extern int _ZN5Event6GetBitEj(unsigned int b);
extern int SublevelToLevel(int i);
extern void _ZN5Event6SetBitEj(unsigned int b);
extern void _ZN9PowerStar13AddStarMarkerEv(char* c);
extern char* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, struct Vector3* v, struct Vector3_16* rot, int e, int f);
extern signed char LEVEL_ID;
extern short NUM_COINS[];

void func_ov002_020b18f0(char* c)
{
    char* r;
    struct Vector3 vec;
    if (_ZN5Event6GetBitEj(0x1f)) return;
    if (SublevelToLevel(LEVEL_ID) >= 0xf) return;
    if (c == 0) return;
    if (NUM_COINS[*(unsigned char*)(c + 0x6d8)] < 0x64) return;
    vec.x = *(Fix12i*)(c + 0x5c);
    vec.y = *(Fix12i*)(c + 0x60);
    vec.z = *(Fix12i*)(c + 0x64);
    vec.y = *(Fix12i*)(c + 0x60) + 0x12c000;
    r = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xb2, 0x20, &vec, 0, *(signed char*)(c + 0xcc), -1);
    if (r == 0) return;
    _ZN5Event6SetBitEj(0x1f);
    _ZN9PowerStar13AddStarMarkerEv(r);
}

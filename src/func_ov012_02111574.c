//cpp
// NONMATCHING: base materialization / addressing (div=14). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
struct Vector3 { int x, y, z; };
extern void _ZN5Sound15PlaySecretSoundEP5ActorPt(void* a, unsigned short* p);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int d, const Vector3& v, unsigned int e);
extern void _ZN7Minimap19UpdateLevelSpecificEv(void);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void* thiz);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* thiz);
extern void _ZN9Animation7AdvanceEv(void* thiz);
extern int SAVE_DATA[];
extern int WATER_HEIGHT;

int func_ov012_02111574(char* c)
{
    if (*(unsigned char*)(c + 0x33e) != 0)
        _ZN5Sound15PlaySecretSoundEP5ActorPt(c, (unsigned short*)(c + 0x33c));
    if (*(int*)(c + 0x60) <= *(int*)(c + 0x334)) {
        *(int*)(c + 0x60) = *(int*)(c + 0x334);
    } else if (*(int*)((char*)SAVE_DATA + 8) & 0x80000) {
        *(unsigned int*)(c + 0x338) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(unsigned int*)(c + 0x338), 3, 0x96, *(Vector3*)(c + 0x74), 0);
        *(int*)(c + 0x60) -= 0x5000;
        *(unsigned char*)(c + 0x33e) = 1;
        if (*(int*)(c + 0x60) <= *(int*)(c + 0x334)) {
            *(int*)(c + 0x60) = *(int*)(c + 0x334);
            _ZN7Minimap19UpdateLevelSpecificEv();
        }
    }
    *(int*)&WATER_HEIGHT = *(int*)(c + 0x60);
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    *(int*)(c + 0x32c) = 0x1000;
    _ZN9Animation7AdvanceEv(c + 0x320);
    return 1;
}
}

/* func_ov062_021163b0 at 0x021163b0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov062).
 */
typedef unsigned char u8;
typedef int Fix12i;
typedef struct { int x, y, z; } Vector3;

extern int _ZN5Actor24KillAndTrackInDeathTableEv(void* c);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* c);
extern int _ZN5Actor14TriplePoofDustEv(void* c);
extern int func_02012694(int a, void* p);
extern void func_0200d8c8(void* cam, void* v, int strength);
extern int _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(void* a, Vector3* v, unsigned n, Fix12i f, short s);
extern int data_02092138;
extern void* data_0209f318;

int func_ov062_021163b0(char* c)
{
    Vector3 v[2];
    if (data_02092138 > *(int*)(c + 0x60)) {
        _ZN5Actor24KillAndTrackInDeathTableEv(c);
        return 1;
    }
    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x144) != 0) {
        _ZN5Actor14TriplePoofDustEv(c);
        _ZN5Actor24KillAndTrackInDeathTableEv(c);
        func_02012694(0x125, c + 0x74);
        func_0200d8c8(data_0209f318, c + 0x5c, 0x7d0000);
        v[0].x = *(int*)(c + 0x5c);
        v[0].y = *(int*)(c + 0x60);
        v[0].z = *(int*)(c + 0x64);
        v[0].y += 0x32000;
        v[1].x = *(int*)(c + 0x5c);
        v[1].y = *(int*)(c + 0x60);
        v[1].z = *(int*)(c + 0x64);
        _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(c, &v[1], *(u8*)(c + 0x10a) + 1, 0xa000, 0);
    }
    return 1;
}
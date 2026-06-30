/* func_ov002_020af3a8 at 0x020af3a8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */
typedef unsigned int u32;
typedef int Fix12i;

struct Vec { Fix12i x, y, z; };
struct Vector3_16;

extern int func_ov002_020af1dc(char* c);
extern void func_ov002_020bdf8c(int);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(u32 id, struct Vec* v);
extern void GiveLives(int delta);
extern void _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 a, u32 b, struct Vec* v, struct Vector3_16* rot, int e, int f);
extern void _ZN5Actor24KillAndTrackInDeathTableEv(void* thiz);

void func_ov002_020af3a8(char* c)
{
    int r = func_ov002_020af1dc(c);
    if (r == 0)
        return;

    unsigned short h = *(unsigned short*)(c + 0xc);
    unsigned is115 = (h == 0x115);
    if (is115) {
        func_ov002_020bdf8c(r);
    } else {
        unsigned is114 = (h == 0x114);
        if (is114) {
            struct Vec vec;
            _ZN5Sound9PlayBank3EjRK7Vector3(0x6e, (struct Vec*)(c + 0x74));
            GiveLives(1);
            vec.x = *(Fix12i*)(c + 0x5c);
            vec.y = *(Fix12i*)(c + 0x60);
            vec.z = *(Fix12i*)(c + 0x64);
            vec.y += 0xb4000;
            _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                0x14b, 8, &vec, 0, *(signed char*)(c + 0xcc), -1);
        }
    }
    _ZN5Actor24KillAndTrackInDeathTableEv(c);
}
#include "types.h"
// @symbol func_ov002_020af2b0
// recovered name: OneUpMushroom_OnTurnIntoEgg
/* recovered: shared common types, renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types, renamed to Class_Method */
/* da1up_c::OnTurnIntoEgg - verified 2004/b56 byte-match (ov002), strict-reloc */
/* func_ov002_020af2b0 at 0x020af2b0
 *
 * Matched byte-for-byte with mwccarm 2004/b56 (ov002).
 */
struct Vec { Fix12i x, y, z; };
struct Vector3_16;

extern void _ZN5Sound9PlayBank3EjRK7Vector3(u32 id, struct Vec* v);
extern void _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 a, u32 b, struct Vec* v, struct Vector3_16* rot, int e, int f);
extern void _ZN5Actor24KillAndTrackInDeathTableEv(void* thiz);

void func_ov002_020af2b0(char* c, int arg1)
{
    int t = *(int*)(c + 0x384);
    if (t == 0xb) {
        func_ov002_020af684(c, 5, arg1);
        return;
    }
    if (t == 0xc) {
        func_ov002_020af684(c, 7, arg1);
        return;
    }
    unsigned isMatch = (*(unsigned short*)(c + 0xc) == 0x114);
    if (isMatch) {
        struct Vec vec;
        _ZN5Sound9PlayBank3EjRK7Vector3(0x6e, (struct Vec*)(c + 0x74));
        GiveLives(1);
        vec.x = *(Fix12i*)(c + 0x5c);
        vec.y = *(Fix12i*)(c + 0x60);
        vec.z = *(Fix12i*)(c + 0x64);
        vec.y += 0xb4000;
        _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
            0x14b, 8, &vec, 0, *(signed char*)(c + 0xcc), -1);
        _ZN5Actor24KillAndTrackInDeathTableEv(c);
    } else {
        func_ov002_020bdf8c(arg1);
        _ZN5Actor24KillAndTrackInDeathTableEv(c);
    }
}

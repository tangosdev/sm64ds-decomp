#include "types.h"
/* func_ov002_020baa2c at 0x020baa2c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */
struct Vector3;
extern s32 Vec3_Dist(const struct Vector3* a, const struct Vector3* b);

void func_ov002_020baa2c(void* self, void* other) {
    char* o = (char*)other;
    char* s = (char*)self;
    unsigned int eq = (*(u16*)(o + 0xc) == 0xbf);
    if (eq == 0) return;
    if (*(u8*)(o + 0x706) != 0) {
        if (*(u8*)(o + 0x6f9) == 0) {
            if (*(u8*)(o + 0x703) == 0) return;
        }
    }
    if (Vec3_Dist((struct Vector3*)(s + 0x5c), (struct Vector3*)(o + 0x5c)) < 0x98000)
        *(u8*)(s + 0x350) = 1;
}

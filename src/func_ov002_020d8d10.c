/* func_ov002_020d8d10 at 0x020d8d10
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */
typedef unsigned int u32;
typedef int Fix12i;
typedef short s16;
typedef unsigned short u16;

extern s16 data_02082214[];
struct Callback;
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 slot, u32 effect, Fix12i x, Fix12i y, Fix12i z, const void* rot, struct Callback* cb);

struct Vector3_16f { s16 x, y, z; };
struct Vec { Fix12i x, y, z; };
struct Self { char pad0[8]; int state; char pad2[0x82]; s16 angle; };

void func_ov002_020d8d10(struct Self* self, struct Vec* pos)
{
    struct Vector3_16f vec;

    vec.x = data_02082214[((unsigned short)(short)(self->angle + 0x8000) >> 4) * 2];
    vec.y = 0;
    vec.z = data_02082214[(((unsigned short)(short)(self->angle + 0x8000) >> 4) * 2) + 1];

    if (self->state != 2) {
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            0, 0xbf, pos->x, pos->y, pos->z, &vec, 0);
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            0, 0xc0, pos->x, pos->y, pos->z, 0, 0);
    } else {
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            0, 0x12f, pos->x, pos->y, pos->z, &vec, 0);
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            0, 0x130, pos->x, pos->y, pos->z, &vec, 0);
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            0, 0x131, pos->x, pos->y, pos->z, 0, 0);
    }
}

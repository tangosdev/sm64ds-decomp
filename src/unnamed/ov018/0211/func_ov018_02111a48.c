/* func_ov018_02111a48 at 0x02111a48
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov018).
 */
typedef int Fix12i;
typedef short s16;
typedef struct { Fix12i x, y, z; } Vector3;

extern Fix12i Vec3_HorzDist(const Vector3* a, const Vector3* b);
extern s16 Vec3_HorzAngle(const Vector3* a, const Vector3* b);
extern int AngleDiff(int a, int b);
extern s16 Vec3_VertAngle(const Vector3* v1, const Vector3* v0);
extern void _Z14ApproachLinearRsss(s16* dst, s16 target, s16 step);

void func_ov018_02111a48(char* a, char* b)
{
    Fix12i dist;
    s16 horzAngle;
    s16 delta, vert;
    Vector3 v0;
    Vector3 v1;
    char* q;
    Fix12i tx, ty, tz;

    if (b == 0) return;

    dist = Vec3_HorzDist((Vector3*)(a + 0x5c), (Vector3*)(b + 0x5c));
    horzAngle = Vec3_HorzAngle((Vector3*)(a + 0x5c), (Vector3*)(b + 0x5c));

    if (dist < 0x1c2000 && AngleDiff(horzAngle, *(s16*)(a + 0x8e)) < 0x1400) {
        tz = *(Fix12i*)(b + 0x64);
        ty = *(Fix12i*)(b + 0x60) + 0x640000;
        tx = *(Fix12i*)(b + 0x5c);
        v0.x = tx;
        v0.z = tz;
        v0.y = ty;
        q = *(char**)(a + 0xe8) + 0xf0;
        v1.x = *(Fix12i*)(q + 0x24);
        v1.y = *(Fix12i*)(q + 0x28);
        v1.z = *(Fix12i*)(q + 0x2c);
        vert = Vec3_VertAngle(&v1, &v0);
        delta = horzAngle - *(s16*)(a + 0x8e);
    } else {
        vert = 0;
        delta = 0;
    }

    _Z14ApproachLinearRsss((s16*)(a + 0x382), delta, 0x250);
    _Z14ApproachLinearRsss((s16*)(a + 0x380), vert, 0x250);
}
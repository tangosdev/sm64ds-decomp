typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

struct Vec3 { s32 x, y, z; };
struct Vec1 { s32 a; };

extern s32 data_0209b454;
extern s8 data_0209f2f8;

extern void func_ov002_020e9590(void *self);
extern void _ZN9ActorBase18MarkForDestructionEv(void *self);
extern void func_02012694(u32 id, void *v);
extern void func_ov002_020e9448(void *p);
extern char *_ZN5Actor10FindWithIDEj(u32 id);
extern s32 Vec3_HorzDist(void *a, void *b);
extern void func_ov002_020e947c(void *c, struct Vec3 *p, s32 n);
extern s32 func_ov002_020e8dd8(void *self);
extern void func_ov002_020e7e24(void *self);
extern void func_ov002_020e7d08(void *self);

void func_ov002_020ea9d0(void *arg0)
{
    char *c = (char *)arg0;
    char *other;
    s32 area;
    struct Vec3 *op;
    struct Vec3 v0;
    struct Vec3 v1;
    struct Vec3 v2;
    struct Vec3 v3;

    if (*(u8 *)(c + 0x49d) != 0) {
        func_ov002_020e9590(c);
        if (*(s32 *)(c + 0x434) == 0) {
            _ZN9ActorBase18MarkForDestructionEv(c);
            return;
        }
    }
    *(u32 *)((int)(((long long)(int)(c + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL)) |= 0x4000000;
    data_0209b454 |= 0x4000000;
    *(u16 *)(c + 0x496) = 0;
    func_02012694(0x57, c + 0x74);
    *(s32 *)(c + 0xa8) = 0x20000;
    func_ov002_020e9448(c);
    other = _ZN5Actor10FindWithIDEj(*(u32 *)(c + 0x434));
    if (*(s32 *)(c + 0x43c) == 4) {
        if (other == 0 || Vec3_HorzDist(c + 0x5c, other + 0x5c) == 0) {
            *(s32 *)(c + 0xa8) = 0x18000;
            *(s32 *)(c + 0x440) = 3;
        } else {
            *(s32 *)(c + 0x440) = 2;
            op = (struct Vec3 *)((int)(((long long)(int)(other + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL));
            *(struct Vec1 *)&v0.x = *(struct Vec1 *)&op->x;
            *(struct Vec1 *)&v0.y = *(struct Vec1 *)&op->y;
            *(struct Vec1 *)&v0.z = *(struct Vec1 *)&op->z;
            v0.y = v0.y + 0xc8000;
            area = data_0209f2f8;
            if (area == 0x11) {
                v1.x = v0.x;
                v1.y = v0.y;
                v1.z = v0.z;
                func_ov002_020e947c(c, &v1, 0x64000);
            } else if (area == 0xb && *(u8 *)(c + 0x49d) == 3) {
                v2.x = v0.x;
                v2.y = v0.y;
                v2.z = v0.z;
                func_ov002_020e947c(c, &v2, 0x46000);
            } else {
                v3.x = v0.x;
                v3.y = v0.y;
                v3.z = v0.z;
                func_ov002_020e947c(c, &v3, 0x190000);
            }
        }
    } else {
        *(s32 *)(c + 0x440) = 1;
    }
    func_ov002_020e8dd8(c);
    func_ov002_020e7e24(c);
    func_ov002_020e7d08(c);
}

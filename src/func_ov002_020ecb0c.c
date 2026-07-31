typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

struct Vec3 { s32 x, y, z; };
struct Vec3_16f { s16 x, y, z; };
struct Spawn { struct Vec3_16f rot; struct Vec3 pos; };

extern s16 data_02082214[];

extern char *_ZN5Actor10FindWithIDEj(u32 id);
extern void func_ov002_020ec728(void *self);
extern void func_ov002_020ed63c(void *self, s32 a);
extern s32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 a, u32 b, s32 x, s32 y, s32 z, struct Vec3_16f *rot, void *cb);
extern s32 _ZN5Sound8PlayLongEjjjRK7Vector3j(u32 a, u32 b, u32 c, void *v, u32 d);
extern void func_ov002_020ed738(void *self);
extern s32 func_ov002_020eddc4(void *self);
extern s32 _ZNK12WithMeshClsn8IsOnWallEv(void *self);
extern s32 func_02035638(void *self);
extern void func_ov002_020edca4(void *self);
extern s32 func_ov002_020ed6cc(void *self);
extern void _ZN12CylinderClsn5ClearEv(void *self);
extern void _ZN12CylinderClsn6UpdateEv(void *self);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *cc);
extern void WithMeshClsn_UpdateContinuous_Veneer(void *self);

void func_ov002_020ecb0c(void *arg0)
{
    char *c = (char *)arg0;
    char *o;
    u32 id;
    s32 flag;
    s32 b;
    u32 fb0;
    s32 onwall;
    struct Spawn sp;
    s16 rx;
    s16 rz;
    u16 ang;
    struct Vec3_16f *pr;
    s32 pz;
    s32 px;

    id = *(u32 *)(c + 0x410);
    o = 0;
    if (id != 0)
        o = _ZN5Actor10FindWithIDEj(id);
    if (o != 0) {
        b = (s32)((*(u32 *)(o + 0xb0) & 0x10000000) != 0);
        if (b == 0) {
            func_ov002_020ec728(c);
            func_ov002_020ed63c(c, 1);
            return;
        }
    }
    fb0 = *(u32 *)(c + 0xb0);
    flag = 0;
    b = (s32)((fb0 & 0x100) != 0);
    if (b == 0) {
        b = (s32)((fb0 & 0x2000) != 0);
        if (b != 0) {
            func_ov002_020ed63c(c, 3);
        } else {
            b = (s32)((fb0 & 0x400) != 0);
            if (b != 0) {
                s32 py = *(volatile s32 *)(c + 0x60) + 0x28000;
                pz = *(s32 *)(c + 0x64);
                px = *(s32 *)(c + 0x5c);
                sp.pos.y = py;
                sp.pos.z = pz;
                sp.pos.x = px;
                ang = *(u16 *)(c + 0x8e);
                onwall = data_02082214[(ang >> 4) * 2];
                rx = (s16)onwall;
                sp.rot.y = 0;
                pr = &sp.rot;
                sp.rot.x = rx;
                ang = *(u16 *)(c + 0x8e);
                rz = data_02082214[(ang >> 4) * 2 + 1];
                sp.rot.z = rz;
                *(s32 *)(c + 0x414) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                    *(u32 *)(c + 0x414), 0x2c, sp.pos.x, sp.pos.y, pz, pr, 0);
                *(s32 *)(c + 0x418) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
                    *(u32 *)(c + 0x418), 3, 0x93, c + 0x74, 0);
                flag = 1;
                func_ov002_020ed738(c);
                if (func_ov002_020eddc4(c) != 0)
                    return;
                onwall = _ZNK12WithMeshClsn8IsOnWallEv(c + 0x144);
                if ((onwall | func_02035638(c + 0x144)) != 0) {
                    func_ov002_020edca4(c);
                    return;
                }
            }
        }
    }
    if (func_ov002_020ed6cc(c) != 0) {
        func_ov002_020edca4(c);
        return;
    }
    _ZN12CylinderClsn5ClearEv(c + 0x110);
    if (flag == 1)
        _ZN12CylinderClsn6UpdateEv(c + 0x110);
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x110);
    WithMeshClsn_UpdateContinuous_Veneer(c + 0x144);
}

//cpp
typedef unsigned char u8;
typedef unsigned short u16;

struct Fix12 { int v; Fix12(int a) : v(a) {} };

struct Actor { int GetSubtraction(short a, short b); };
struct WithMeshClsn { int IsOnGround() const; };

extern "C" {
void func_ov060_02111cc0(char *c, int idx, int fix);
int func_ov060_02115a30(void *o);
void _Z14ApproachLinearRsss(short *dst, short target, short step);
int _Z14ApproachLinearRiii(int *dst, int target, int step);
unsigned int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int a, unsigned int b, int c, int d, int e, void const *f, void *g);
}

extern "C" void func_ov060_02113d8c(char *r4)
{
    if (*(u16 *)(r4 + 0x3fc) == 0)
        *(int *)(r4 + 0x98) = 0;

    switch (*(u8 *)(r4 + 0x423)) {
    case 0:
        func_ov060_02111cc0(r4, 0x18, 0x40000000);
        *(u16 *)(r4 + 0x3fe) = 0;
        if (func_ov060_02115a30(r4) != 0)
            *(u8 *)(r4 + 0x423) = 1;
        break;
    case 1:
        func_ov060_02111cc0(r4, 0x19, 0);
        *(int *)(r4 + 0x98) = 0x2a000;
        if (func_ov060_02115a30(r4) != 0) {
            u16 *p = (u16 *)(((long long)(int)(r4 + 0x3fe)) & 0xFFFFFFFFFFFFFFFFLL);
            *p = *p + 1;
            if (*(u16 *)((r4 + 0x300) + 0xfe) > 0xa)
                *(u8 *)(r4 + 0x423) = 3;
            if (*(u16 *)((r4 + 0x300) + 0xfe) >= 2) {
                if (((Actor *)r4)->GetSubtraction(*(short *)(r4 + 0x406), *(short *)(r4 + 0x8e)) > 0x2000) {
                    *(u8 *)(r4 + 0x423) = 3;
                    *(int *)(r4 + 0x448) = 0;
                }
            }
        }
        _Z14ApproachLinearRsss((short *)(r4 + 0x8e), *(short *)(r4 + 0x406), 0x200);
        break;
    case 3:
        *(u16 *)(r4 + 0x3fe) = 0;
        func_ov060_02111cc0(r4, 0x1a, 0x40000000);
        *(unsigned int *)(r4 + 0x448) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned int *)(r4 + 0x448), 0x101, *(int *)(r4 + 0x5c), *(int *)(r4 + 0x60) + 0x32000, *(int *)(r4 + 0x64), 0, 0);
        if (_Z14ApproachLinearRiii((int *)(r4 + 0x98), 0, 0x1000) != 0)
            *(u8 *)(r4 + 0x423) = 2;
        break;
    case 2:
        *(int *)(r4 + 0x98) = 0;
        if (func_ov060_02115a30(r4) != 0) {
            if (*(u8 *)(r4 + 0x414) == 2)
                *(u16 *)(r4 + 0x3fc) = 0xa;
            else
                *(u16 *)(r4 + 0x3fc) = 0x1e;
            if (*(u16 *)((r4 + 0x300) + 0xfe) > *(u16 *)((r4 + 0x300) + 0xfc)) {
                *(int *)(r4 + 0x40c) = 0;
                *(int *)(r4 + 0x3f8) = 0x1000;
            }
            {
                u16 *p = (u16 *)(((long long)(int)(r4 + 0x3fe)) & 0xFFFFFFFFFFFFFFFFLL);
                *p = *p + 1;
            }
        }
        break;
    default:
        break;
    }

    if (((WithMeshClsn *)(r4 + 0x14c))->IsOnGround())
        return;
    *(int *)(r4 + 0x40c) = 0xa;
    *(int *)(r4 + 0x5c) = *(int *)(r4 + 0x3c8);
    *(int *)(r4 + 0x60) = *(int *)(r4 + 0x3cc);
    *(int *)(r4 + 0x64) = *(int *)(r4 + 0x3d0);
    *(int *)(r4 + 0x98) = 0;
}

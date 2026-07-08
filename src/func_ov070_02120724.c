typedef short s16;

struct M12 { int w[12]; };
struct Vec3 { int x, y, z; };

extern void Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
extern void _ZN13RaycastGroundC1Ev(void *rg);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *rg, struct Vec3 *pos, void *actor);
extern int _ZN13RaycastGround10DetectClsnEv(void *rg);
extern void _ZN13RaycastGroundD1Ev(void *rg);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *c, void *sm, void *mtx, int h, int g, unsigned int u);
extern signed char LEVEL_ID[];

struct RG { char pad[0x44]; int ground; char pad2[8]; };

void func_ov070_02120724(char *c)
{
    int g;

    if (*(int*)(c + 0x420) == 2) {
        Matrix4x3_FromRotationXYZExt(c + 0xf0,
            *(s16*)(c + 0x8c), *(s16*)(c + 0x8e), *(s16*)(c + 0x90));
        *(int*)(c + 0x114) = *(int*)(c + 0x5c) >> 3;
        *(int*)(c + 0x118) = *(int*)(c + 0x60) >> 3;
        *(int*)(c + 0x11c) = *(int*)(c + 0x64) >> 3;
        *(int*)(c + 0x3f8) = *(int*)(c + 0x5c) >> 3;
        *(int*)(c + 0x3fc) = *(int*)(c + 0x60) >> 3;
        *(int*)(c + 0x400) = *(int*)(c + 0x64) >> 3;
    } else {
        *(int*)(c + 0x114) = *(int*)(c + 0x404) >> 3;
        *(int*)(c + 0x118) = *(int*)(c + 0x408) >> 3;
        *(int*)(c + 0x11c) = *(int*)(c + 0x40c) >> 3;
        *(struct M12*)(c + 0x154) = *(struct M12*)(c + 0xf0);
        *(int*)(c + 0x3f8) = *(int*)(c + 0x404) >> 3;
        *(int*)(c + 0x3fc) = *(int*)(c + 0x408) >> 3;
        *(int*)(c + 0x400) = *(int*)(c + 0x40c) >> 3;
    }

    g = 0x1f4000;
    if (LEVEL_ID[0] == 0x11) {
        struct RG rg;
        struct Vec3 pos;
        int y;
        pos.x = *(int*)(c + 0x5c);
        y = *(int*)(c + 0x60);
        pos.y = y;
        pos.z = *(int*)(c + 0x64);
        pos.y = y - 0xa000;
        _ZN13RaycastGroundC1Ev(&rg);
        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &pos, 0);
        if (_ZN13RaycastGround10DetectClsnEv(&rg) != 0) {
            g = (*(int*)(c + 0x60) - rg.ground) + 0x28000;
        }
        _ZN13RaycastGroundD1Ev(&rg);
    }

    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x1b0, c + 0x3d4, 0x5a000, g, 0xf);
}

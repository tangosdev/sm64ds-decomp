typedef struct Vector3 { int x, y, z; } Vector3;
typedef struct RaycastGround { int filler[17]; int resultY; int pad[2]; } RaycastGround;

extern void _ZN13RaycastGroundC1Ev(RaycastGround* c);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround* c, Vector3* pos, void* actor);
extern int _ZN13RaycastGround10DetectClsnEv(RaycastGround* c);
extern void _ZN13RaycastGroundD1Ev(RaycastGround* c);
extern int _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);

int func_0200fc44(int a, Vector3* pos, int flag) {
    if (flag) {
        RaycastGround rg;
        _ZN13RaycastGroundC1Ev(&rg);
        int* yp = (int*)((int)pos + 4);
        *yp += 0x32000;
        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, pos, 0);
        if (_ZN13RaycastGround10DetectClsnEv(&rg))
            pos->y = rg.resultY;
        _ZN13RaycastGroundD1Ev(&rg);
    }
    int* yp2 = (int*)((int)pos + 4);
    *yp2 += 0x19000;
    return _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb3, pos->x, pos->y, pos->z);
}

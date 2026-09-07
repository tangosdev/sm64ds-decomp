typedef struct Vector3 { int x, y, z; } Vector3;
typedef struct dBgCh_Gnd { int filler[17]; int resultY; int pad[2]; } dBgCh_Gnd;

extern void _ZN9dBgCh_GndC1Ev(dBgCh_Gnd* c);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd* c, Vector3* pos, void* actor);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd* c);
extern void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd* c);
extern int _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);

int func_0200fc44(int a, Vector3* pos, int flag) {
    if (flag) {
        dBgCh_Gnd rg;
        _ZN9dBgCh_GndC1Ev(&rg);
        int* yp = (int*)((int)pos + 4);
        *yp += 0x32000;
        _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, pos, 0);
        if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg))
            pos->y = rg.resultY;
        _ZN9dBgCh_GndD1Ev(&rg);
    }
    int* yp2 = (int*)((int)pos + 4);
    *yp2 += 0x19000;
    return _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb3, pos->x, pos->y, pos->z);
}

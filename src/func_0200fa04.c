typedef struct Vector3 { int x, y, z; } Vector3;
typedef struct dBgCh_Gnd { char filler[0x44]; int clsnY; char rest[0x8]; } dBgCh_Gnd;

extern void _ZN9dBgCh_GndC1Ev(dBgCh_Gnd*);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd*, Vector3*, void*);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd*);
extern void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd*);
extern int _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int, int, int, int);

int func_0200fa04(int a, Vector3* pos, int flag)
{
    dBgCh_Gnd rc;
    if (flag) {
        _ZN9dBgCh_GndC1Ev(&rc);
        *(int*)(((int)pos + 4)) += 0x32000;
        _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rc, pos, 0);
        if (_ZN9dBgCh_Gnd10DetectClsnEv(&rc))
            pos->y = rc.clsnY;
        _ZN9dBgCh_GndD1Ev(&rc);
    }
    *(int*)(((int)pos + 4)) += 0x78000;
    return _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x1b, pos->x, pos->y, pos->z);
}

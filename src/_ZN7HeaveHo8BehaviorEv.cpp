//cpp
typedef short s16;
typedef unsigned short u16;
typedef int Fix12;
struct Klass; typedef void (Klass::*PMF)();
struct M { char pad[8]; PMF pmf; };
struct CylinderClsn;
struct WithMeshClsn;
struct Vector3 { int x, y, z; };
extern "C" {
unsigned short DecIfAbove0_Short(unsigned short *p);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, CylinderClsn *cc);
int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
void *_ZNK12WithMeshClsn14GetFloorResultEv(void *self);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *self, Vector3 *v);
int func_02010844(void *unused, Vector3 *v, s16 angle);
int _ZN5Enemy15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_(void *self, WithMeshClsn *wm, Fix12 a, s16 b, int c, int d, void *e);
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, WithMeshClsn *wm, unsigned int j);
void func_ov077_02126dac(char *t);
void func_ov077_02126528(char *c);
void _ZN12CylinderClsn5ClearEv(CylinderClsn *self);
void _ZN12CylinderClsn6UpdateEv(CylinderClsn *self);
void _ZN9Animation7AdvanceEv(void *self);
extern int data_0209f32c;
extern char data_ov077_02127cd8[];
}

extern "C" int _ZN7HeaveHo8BehaviorEv(char *c)
{
    int b;
    Vector3 v;
    int r5;
    M *m;

    if (*(int *)(c + 0x60) < data_0209f32c) {
        *(int *)(c + 0x5c) = *(int *)(c + 0x404);
        *(int *)(c + 0x60) = *(int *)(c + 0x408);
        *(int *)(c + 0x64) = *(int *)(c + 0x40c);
        return 1;
    }

    DecIfAbove0_Short((unsigned short *)(c + 0x100));
    DecIfAbove0_Short((unsigned short *)(c + 0x426));

    m = *(M **)(c + 0x3fc);
    if (m->pmf != 0)
        (((Klass *)c)->*(m->pmf))();

    _ZN5Actor9UpdatePosEP12CylinderClsn(c, (CylinderClsn *)(c + 0x144));

    r5 = 0;
    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x184)) {
        void *fr = _ZNK12WithMeshClsn14GetFloorResultEv(c + 0x184);
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char *)fr + 4, &v);
        r5 = func_02010844(c, &v, *(s16 *)(c + 0x8e));
    }

    b = _ZN5Enemy15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_(c, (WithMeshClsn *)(c + 0x184), 0x3c000, (s16)0x2888, 0, 1, (void *)0x32000);
    if (b == 0) {
        if (r5 < 0)
            r5 = (s16)-r5;
        if (r5 <= 0x100)
            goto writeback;
    }
    *(int *)(c + 0x5c) = *(int *)(c + 0x410);
    *(int *)(c + 0x60) = *(int *)(c + 0x414);
    *(int *)(c + 0x64) = *(int *)(c + 0x418);
writeback:
    *(int *)(c + 0x410) = *(int *)(c + 0x5c);
    *(int *)(c + 0x414) = *(int *)(c + 0x60);
    *(int *)(c + 0x418) = *(int *)(c + 0x64);
    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, (WithMeshClsn *)(c + 0x184), 2);

    *(s16 *)(c + 0x8e) = *(s16 *)(c + 0x94);
    func_ov077_02126dac(c);

    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x184) && *(void **)(c + 0x3fc) != (void *)data_ov077_02127cd8) {
        func_ov077_02126528(c);
    }
    _ZN12CylinderClsn5ClearEv((CylinderClsn *)(c + 0x110));
    _ZN12CylinderClsn6UpdateEv((CylinderClsn *)(c + 0x110));
    _ZN12CylinderClsn5ClearEv((CylinderClsn *)(c + 0x144));
    _ZN12CylinderClsn6UpdateEv((CylinderClsn *)(c + 0x144));

    _ZN9Animation7AdvanceEv(c + 0x390);
    return 1;
}

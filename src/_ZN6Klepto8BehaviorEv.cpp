//cpp
struct Klass; typedef void (Klass::*PMF)();
struct M { char pad[8]; PMF pmf; };
struct CylinderClsn;
struct WithMeshClsn;
extern "C" {
unsigned short DecIfAbove0_Short(unsigned short *p);
void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void *self, CylinderClsn *cc);
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, WithMeshClsn *wm, unsigned int j);
void func_ov062_0211c6a8(void *self);
void *_ZN5Actor10FindWithIDEj(unsigned int id);
void func_02012790(int a, int b);
void func_ov062_0211c658(void *c, PMF *p);
void _ZN9ActorBase18MarkForDestructionEv(void *self);
void _ZN14BlendModelAnim7AdvanceEv(void *self);
void func_ov062_0211b51c(void *self);
void _ZN12CylinderClsn5ClearEv(CylinderClsn *self);
void _ZN12CylinderClsn6UpdateEv(CylinderClsn *self);
extern char data_ov062_0211e17c[];
extern char data_ov062_0211e14c[];
}

extern "C" int _ZN6Klepto8BehaviorEv(char *c)
{
    M *m;
    int b;

    DecIfAbove0_Short((unsigned short *)(c + 0x100));
    DecIfAbove0_Short((unsigned short *)(c + 0x444));

    m = *(M **)(c + 0x42c);
    if (m->pmf != 0)
        (((Klass *)c)->*(m->pmf))();

    {
        int accum = *(int *)(c + 0xa8);
        int a0 = *(int *)(c + 0x9c);
        int lim = *(int *)(c + 0xa0);
        int sum = accum + a0;
        if (sum >= lim)
            lim = sum;
        int t = *(int *)(c + 0xac);
        *(int *)(c + 0xa8) = lim;
        *(int *)(c + 0xac) = t;
    }
    _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(c, (CylinderClsn *)(c + 0x110));
    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, (WithMeshClsn *)(c + 0x178), 0);

    *(short *)(c + 0x8c) = *(short *)(c + 0x92);
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
    *(short *)(c + 0x90) = *(short *)(c + 0x96);
    func_ov062_0211c6a8(c);

    unsigned int actorId = *(unsigned int *)(c + 0x44c);
    if (actorId != 0) {
        void *p = _ZN5Actor10FindWithIDEj(actorId);
        if (p != 0) {
            if (*(int *)(c + 0x468) == 1) {
                *(int *)((char *)p + 0x5c) = *(int *)(c + 0x450);
                *(int *)((char *)p + 0x60) = *(int *)(c + 0x454);
                *(int *)((char *)p + 0x64) = *(int *)(c + 0x458);
                goto skip_destroy;
            } else if (*(unsigned char *)((char *)p + 0x403) == 0) {
                *(int *)((char *)p + 0x5c) = *(int *)(c + 0x450);
                *(int *)((char *)p + 0x60) = *(int *)(c + 0x454);
                *(int *)((char *)p + 0x64) = *(int *)(c + 0x458);
                goto skip_destroy;
            } else {
                *(int *)(c + 0x44c) = 0;
                func_02012790(0xa, 0);
                *(unsigned short *)(c + 0x400 + 0x44) = 0x1e;
                func_ov062_0211c658(c, (PMF *)data_ov062_0211e17c);
                goto skip_destroy;
            }
        } else {
            *(int *)(c + 0x44c) = 0;
            func_02012790(0xa, 0);
            *(unsigned short *)(c + 0x400 + 0x44) = 0x1e;
            func_ov062_0211c658(c, (PMF *)data_ov062_0211e17c);
            goto skip_destroy;
        }
    }

    if (*(int *)(c + 0x468) == 1 && *(unsigned char *)(c + 0x448) != 2) {
        b = (*(int *)(c + 0xb0) & 8) != 0;
        if (b != 0) {
            _ZN9ActorBase18MarkForDestructionEv(c);
        }
    }
skip_destroy:
    _ZN14BlendModelAnim7AdvanceEv((void *)(c + 0x334));
    if (*(void **)(c + 0x42c) != (void *)data_ov062_0211e14c) {
        func_ov062_0211b51c(c);
    }

    _ZN12CylinderClsn5ClearEv((CylinderClsn *)(c + 0x110));
    _ZN12CylinderClsn6UpdateEv((CylinderClsn *)(c + 0x110));
    _ZN12CylinderClsn5ClearEv((CylinderClsn *)(c + 0x144));
    _ZN12CylinderClsn6UpdateEv((CylinderClsn *)(c + 0x144));

    return 1;
}

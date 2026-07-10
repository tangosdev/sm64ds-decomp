//cpp
struct WithMeshClsn;
struct CylinderClsn;
struct Enemy;
typedef void (Enemy::*PMF)();
struct Holder { char pad[8]; PMF fn; };

extern "C" {
typedef unsigned int u32;
extern int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(Enemy *thiz, WithMeshClsn *c);
extern void _ZN12CylinderClsn5ClearEv(void *thiz);
extern void _ZN12CylinderClsn6UpdateEv(void *thiz);
extern void func_ov090_02133338(char *c);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(Enemy *thiz, void *clsn);
extern void _ZN9Animation7AdvanceEv(void *thiz);
extern void func_ov090_021330c8(char *thiz);
extern char *_ZN5Actor13ClosestPlayerEv(Enemy *thiz);
}

struct Enemy { char pad[0x800]; };

extern "C" int _ZN10CheepCheep8BehaviorEv(Enemy *thiz)
{
    char *c = (char *)thiz;
    if (_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(thiz, (WithMeshClsn *)(c + 0x150)) != 0) {
        _ZN12CylinderClsn5ClearEv(c + 0x110);
        if (*(unsigned char *)(c + 0x107) != 0) {
            if (*(unsigned short *)(c + 0x104) == 0) {
                _ZN12CylinderClsn6UpdateEv(c + 0x110);
            }
        }
        func_ov090_02133338(c);
        return 1;
    }

    DecIfAbove0_Short((unsigned short *)(c + 0x100));
    _ZN5Actor9UpdatePosEP12CylinderClsn(thiz, (void *)(c + 0x110));
    {
        Holder *q = *(Holder **)(c + 0x370);
        if (q->fn != 0) (thiz->*(q->fn))();
    }
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
    *(int *)(c + 0x368) = 0x1000;
    _ZN9Animation7AdvanceEv(c + 0x35c);
    func_ov090_02133338(c);
    func_ov090_021330c8(c);
    _ZN12CylinderClsn5ClearEv(c + 0x110);
    {
        char *p = _ZN5Actor13ClosestPlayerEv(thiz);
        if (p != 0 && *(unsigned char *)(p + 0x6fb) == 0) {
            _ZN12CylinderClsn6UpdateEv(c + 0x110);
        }
    }
    return 1;
}

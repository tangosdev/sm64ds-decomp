//cpp
extern "C" {
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void*);
extern char* _ZNK12WithMeshClsn14GetFloorResultEv(void*);
extern int func_02037e78(int* p);
extern void func_ov084_021296cc(void*);
extern void _ZN5Enemy9SpawnCoinEv(void*);
extern void func_ov084_02129498(void*);
extern void _ZN8CapEnemy10ReleaseCapERK7Vector3(void*, void*);
extern void _ZN8CapEnemy15RespawnIfHasCapEv(void*);
extern int func_02037e38(unsigned int* p);
extern int func_02037e84(int* p);
extern void _ZN10ClsnResultD1Ev(void*);
extern int data_02099368[];
}

struct V3 { int x, y, z; };

extern "C" void func_ov084_021294d0(char* c)
{
    char obj[0x28];
    if (!_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x1b4))
        return;

    char* fr = _ZNK12WithMeshClsn14GetFloorResultEv(c + 0x1b4);
    if (func_02037e78((int*)(fr + 4))) {
        func_ov084_021296cc(c);
        _ZN5Enemy9SpawnCoinEv(c);
        func_ov084_02129498(c);
        V3 v;
        v.x = 0; v.y = 0x6c000; v.z = 0;
        _ZN8CapEnemy10ReleaseCapERK7Vector3(c, &v);
        *(int*)(c + 0x5c) = *(int*)(c + 0x41c);
        *(int*)(c + 0x60) = *(int*)(c + 0x420);
        *(int*)(c + 0x64) = *(int*)(c + 0x424);
        _ZN8CapEnemy15RespawnIfHasCapEv(c);
        return;
    }

    char* fr2 = _ZNK12WithMeshClsn14GetFloorResultEv(c + 0x1b4);
    int r5;
    {
    char* d = obj + 4;
    int a = *(int*)(fr2 + 4);
    int b = *(int*)(fr2 + 8);
    *(int*)(d) = b ? a : a;
    *(int*)(d + 4) = b;
    *(int*)(d + 8) = *(int*)(fr2 + 0xc);
    *(int*)(d + 0xc) = *(int*)(fr2 + 0x10);
    *(int*)(d + 0x10) = *(int*)(fr2 + 0x14);
    *(int*)(obj) = (int)data_02099368;
    *(unsigned short*)(obj + 0x18) = *(unsigned short*)(fr2 + 0x18);
    *(unsigned short*)(obj + 0x1a) = *(unsigned short*)(fr2 + 0x1a);
    *(int*)(obj + 0x1c) = *(int*)(fr2 + 0x1c);
    *(int*)(obj + 0x20) = *(int*)(fr2 + 0x20);
    *(int*)(obj + 0x24) = *(int*)(fr2 + 0x24);
    r5 = func_02037e38((unsigned int*)d);
    }
    if (func_02037e84((int*)(obj + 4)) == 8) {
        if (r5 == 6 || r5 == 7 || r5 == 8 || r5 == 9)
            goto action;
    }
    if (r5 == 0x13 || r5 == 1)
        goto action;
    if ((unsigned)(r5 - 4) > 1)
        goto dtor;
action:
    _ZN5Enemy9SpawnCoinEv(c);
    func_ov084_02129498(c);
    if ((*(unsigned char*)(c + 0x113) & 0xf) < 6 ||
        *(unsigned char*)(c + 0x464) == 2) {
        *(int*)(c + 0x5c) = *(int*)(c + 0x41c);
        *(int*)(c + 0x60) = *(int*)(c + 0x420);
        *(int*)(c + 0x64) = *(int*)(c + 0x424);
        V3 v2;
        v2.x = 0; v2.y = 0x6c000; v2.z = 0;
        _ZN8CapEnemy10ReleaseCapERK7Vector3(c, &v2);
        _ZN8CapEnemy15RespawnIfHasCapEv(c);
    }
dtor:
    _ZN10ClsnResultD1Ev(obj);
}

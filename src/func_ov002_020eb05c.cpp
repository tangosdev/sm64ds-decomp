//cpp
struct C;
typedef void (C::*PMF)();

struct V3 { int x, y, z; };

extern "C" {
extern void func_ov002_020e700c(char *c);
extern int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(char *c, char *clsn);
extern void func_ov002_020d718c(void *p);
extern void func_ov002_020e84ec(char *c);
extern void _ZN12CylinderClsn5ClearEv(char *c);
extern void func_ov002_020e763c(char *c);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(char *c, void *clsn);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(char *c, const void *v);
extern void _ZN12CylinderClsn6UpdateEv(char *c);
extern void func_ov002_020e7eb8(char *c);
extern int data_0209b454;
extern int data_ov002_0210aa0c[3];
}

extern PMF data_ov002_021109d8[];

struct C { char pad[0x800]; };

extern "C" int func_ov002_020eb05c(char *c)
{
    func_ov002_020e700c(c);
    *(int *)(c + 0x4a8) = 0;
    *(int *)(c + 0x4ac) = 0;
    *(int *)(c + 0x4b0) = 0;

    if (_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(c, c + 0x150) != 0) {
        int state = *(int *)(c + 0x440);
        if (state >= 5 && state <= 7 && *(void **)(c + 0xd0) != 0) {
            func_ov002_020d718c(*(void **)(c + 0xd0));
            *(int *)(c + 0xd0) = 0;
            *(int *)((int)(c + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) &= ~0xe0000;
            func_ov002_020e84ec(c);
            _ZN12CylinderClsn5ClearEv(c + 0x110);
            return 1;
        }
        if ((data_0209b454 & 0x4000000) != 0) {
            if ((int)((*(int *)(c + 0xb0) & 0x4000000) != 0) != 0) {
                char *p = *(char **)(c + 0xd0);
                if (p != 0)
                    *(int *)((int)(p + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) |= 0x4000000;
            }
        }
        func_ov002_020e84ec(c);
        _ZN12CylinderClsn5ClearEv(c + 0x110);
        return 1;
    }

    *(int *)(c + 0xd0) = 0;
    func_ov002_020e763c(c);
    (((C *)c)->*data_ov002_021109d8[*(int *)(c + 0x440)])();
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
    func_ov002_020e84ec(c);
    _ZN12CylinderClsn5ClearEv(c + 0x110);
    {
        V3 v;
        v.x = data_ov002_0210aa0c[0];
        v.y = data_ov002_0210aa0c[1];
        v.z = data_ov002_0210aa0c[2];
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c + 0x110, &v);
    }
    if (*(unsigned char *)(c + 0x49f) == 0)
        _ZN12CylinderClsn6UpdateEv(c + 0x110);
    func_ov002_020e7eb8(c);
    return 1;
}

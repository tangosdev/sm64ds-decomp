//cpp
struct Vector3 { int x, y, z; };
struct Actor;
struct RaycastGround { char buf[0x54]; };

extern "C" int _ZNK12WithMeshClsn10IsOnGroundEv(void *thiz);
extern "C" void _ZN13RaycastGroundC1Ev(struct RaycastGround *thiz);
extern "C" void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(struct RaycastGround *thiz, const struct Vector3 *v, struct Actor *a);
extern "C" int _ZN13RaycastGround10DetectClsnEv(struct RaycastGround *thiz);
extern "C" void _ZN13RaycastGroundD1Ev(struct RaycastGround *thiz);

extern "C" void func_ov070_02121be4(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    struct RaycastGround rg;
    struct Vector3 v;

    if (!_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x130)) return;

    _ZN13RaycastGroundC1Ev(&rg);
    {
        int z = *(int *)(c + 0x64);
        int y = *(int *)(c + 0x60) + 0x1e000;
        int x = *(int *)(c + 0x5c);
        v.x = x;
        v.y = y;
        v.z = z;
    }
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &v, (struct Actor *)c);
    if (_ZN13RaycastGround10DetectClsnEv(&rg) == 0 ||
        *(int *)((char *)&rg + 0x44) < *(int *)(c + 0x60) - 0x32000) {
        *(int *)(c + 0x5c) = *(int *)(c + 0x68);
        *(int *)(c + 0x60) = *(int *)(c + 0x6c);
        *(int *)(c + 0x64) = *(int *)(c + 0x70);
    }
    _ZN13RaycastGroundD1Ev(&rg);
}

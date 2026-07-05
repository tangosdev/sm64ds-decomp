//cpp
extern "C" {
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* thiz);
extern void _ZN13RaycastGroundC1Ev(void* self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void* self, void* pos, void* act);
extern int func_02038ea4(void* thiz);
extern void _ZN13RaycastGroundD1Ev(void* self);

int func_ov030_02111dd0(char* c)
{
    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x194) != 0) {
        char rg[0x54];
        int v[3];
        int y, z, x, s;
        _ZN13RaycastGroundC1Ev(rg);
        y = *(int*)(c + 0x60);
        z = *(int*)(c + 0x64);
        x = *(int*)(c + 0x5c);
        s = y + 0x1e000;
        v[0] = x;
        v[1] = s;
        v[2] = z;
        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(rg, v, c);
        if (func_02038ea4(rg) == 0 || *(int*)(c + 0x60) - *(int*)(rg + 0x44) > 0x2000) {
            *(int*)(c + 0x5c) = *(int*)(c + 0x38c);
            *(int*)(c + 0x60) = *(int*)(c + 0x390);
            *(int*)(c + 0x64) = *(int*)(c + 0x394);
            _ZN13RaycastGroundD1Ev(rg);
            return 1;
        }
        *(int*)(c + 0x38c) = *(int*)(c + 0x5c);
        *(int*)(c + 0x390) = *(int*)(c + 0x60);
        *(int*)(c + 0x394) = *(int*)(c + 0x64);
        _ZN13RaycastGroundD1Ev(rg);
    }
    return 0;
}
}

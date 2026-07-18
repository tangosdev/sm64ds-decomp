typedef unsigned char u8;
typedef unsigned short u16;

extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
extern void *_ZNK12WithMeshClsn14GetFloorResultEv(void *self);
extern int func_02037e78(int *p);
extern int func_ov002_020bf30c(void *c, int a);
extern void _ZN13RaycastGroundC1Ev(void *self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *self, void *pos, void *act);
extern int _ZN13RaycastGround10DetectClsnEv(void *self);
extern void _ZN13RaycastGroundD1Ev(void *self);
extern int _ZN6Player7IsStateERNS_5StateE(void *thiz, void *st);

extern char data_ov002_0211013c;

void func_ov002_020c2b08(void *arg0)
{
    char *c = (char *)arg0;
    char rg[0x54];
    int v[3];

    if (*(int *)(c + 8) != 1)
        return;
    if (*(u8 *)(c + 0x703) != 0)
        return;

    if (*(u8 *)(c + 0x6ea) == 0) {
        int limit;

        if (!_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x380))
            return;
        if (!func_02037e78((int *)((char *)_ZNK12WithMeshClsn14GetFloorResultEv(c + 0x380) + 4)))
            return;

        limit = func_ov002_020bf30c(c, 0x20000);
        if (*(int *)(c + 0x98) >= limit)
            *(u16 *)(c + 0x6ba) = 0x3c;
        else
            *(u16 *)(c + 0x6ba) = 0x1e;
        *(u8 *)(c + 0x6ea) = 1;
        return;
    }

    if (*(u16 *)(c + 0x6ba) != 0) {
        if (!_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x380))
            goto clear_timer;
        if (!func_02037e78((int *)((char *)_ZNK12WithMeshClsn14GetFloorResultEv(c + 0x380) + 4)))
            goto clear_timer;

        *(u8 *)(c + 0x707) = 1;

        if (*(u16 *)(c + 0x6ba) != 0) {
            if (*(int *)(c + 0x98) >= 0x1000)
                goto big_block;
        }

        *(u16 *)(c + 0x6ba) = 0;
        *(int *)(((long long)(int)(c + 0x60)) & 0xFFFFFFFFFFFFFFFFLL) += *(int *)(c + 0x690);
        if (*(int *)(c + 0x60) < *(int *)(c + 0x644))
            *(int *)(c + 0x60) = *(int *)(c + 0x644);
        return;

    big_block:
        {
            int limit2 = func_ov002_020bf30c(c, 0x28000);

            if (*(int *)(c + 0x98) < limit2) {
                if (*(u16 *)(c + 0x6ba) >= 0x1e)
                    *(u16 *)(c + 0x6ba) = 0x1e;
            }

            *(int *)(c + 0x55c) = 0;
            {
                int t = *(int *)(c + 0x55c);
                *(int *)(c + 0x554) = t;
                *(int *)(c + 0x558) = 0x1000;
            }

            {
                int yy = *(int *)(c + 0x60);
                int zz = *(int *)(c + 0x64);
                int xx = *(int *)(c + 0x5c);
                int yy2 = yy + 0x64000;
                v[0] = xx;
                v[1] = yy2;
                v[2] = zz;
            }

            _ZN13RaycastGroundC1Ev(rg);
            _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(rg, v, c);

            if (_ZN13RaycastGround10DetectClsnEv(rg) != 0) {
                *(int *)(c + 0x644) = *(int *)(rg + 0x44);
                if (*(int *)(c + 0x60) + *(int *)(c + 0x690) < *(int *)(rg + 0x44))
                    *(u16 *)(c + 0x6ba) = 0;
            }

            _ZN13RaycastGroundD1Ev(rg);
            return;
        }

    clear_timer:
        *(u16 *)(c + 0x6ba) = 0;
        return;
    }

    *(u8 *)(c + 0x6ec) = 0;
    if (!_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x380))
        return;
    if (func_02037e78((int *)((char *)_ZNK12WithMeshClsn14GetFloorResultEv(c + 0x380) + 4)) != 0)
        return;
    if (*(u8 *)(c + 0x707) != 0)
        return;

    if (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_0211013c)) {
        *(u8 *)(c + 0x6ea) = 0;
        *(u8 *)(c + 0x6ec) = 1;
    }
    return;
}

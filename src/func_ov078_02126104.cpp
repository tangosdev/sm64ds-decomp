//cpp
struct C; typedef int (C::*PMF)();
struct C { char pad[0x420]; PMF *pp; };

struct Vector3 { int x, y, z; };

struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); };
struct Derived { char pad[0x2cc]; Base base; };

extern "C" {
extern int _ZN5Actor13DistToCPlayerEv(void *self);
extern void _ZN14BlendModelAnim7AdvanceEv(void *self);
extern void func_ov078_02125de0(char *c);
extern int func_ov078_02125c98(void *c);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
extern void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void *self, void *clsn);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, void *wmc, unsigned int flags);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void *self);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
extern int func_ov078_02125c48(void *c, void *p);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void *self, const Vector3 *v);
extern void _ZN12CylinderClsn5ClearEv(void *self);
extern void _ZN12CylinderClsn6UpdateEv(void *self);

extern void *CAMERA;
extern char data_ov078_0212707c[];
extern char data_ov078_021270bc[];
extern char data_ov078_0212703c[];
extern char data_ov078_021270fc[];
extern Vector3 data_ov078_02126e00;
}

extern "C" int func_ov078_02126104(C *c)
{
    char *self = (char *)c;

    if (_ZN5Actor13DistToCPlayerEv(c) < 0x1770000) {
        *(C **)((char *)CAMERA + 0x114) = c;
    }

    if (*(void **)((char *)c->pp + 8) != 0) {
        PMF *p = c->pp + 1;
        (c->**p)();
    }

    {
        Base *b = &((Derived *)self)->base;
        *(int *)(self + 0x328) = *(int *)(self + 0x4fc) << 0xc;
        b->v3();
    }
    _ZN14BlendModelAnim7AdvanceEv(self + 0x2cc);

    if ((char *)c->pp == data_ov078_0212707c) {
        void *r1 = *(void **)(self + 0x494);
        int b;
        if (r1 != 0) {
            b = (*(int *)(self + 0xb0) & 0x4000) != 0;
            if (b != 0 && *(int *)((char *)r1 + 0xc8) != 0) {
                goto skip_de0;
            }
        }
        func_ov078_02125de0(self);
    skip_de0:
        func_ov078_02125c98(self);
        return 1;
    }

    DecIfAbove0_Short((unsigned short *)(self + 0x100));
    DecIfAbove0_Byte((unsigned char *)(self + 0x505));
    DecIfAbove0_Byte((unsigned char *)(self + 0x504));

    if ((char *)c->pp != data_ov078_021270bc) {
        _ZN5Actor9UpdatePosEP12CylinderClsn(self, self + 0x33c);
    } else {
        _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(self, self + 0x33c);
    }

    if ((char *)c->pp != data_ov078_021270bc || *(unsigned char *)(self + 0x499) == 1) {
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(self, self + 0x110, 0);
    }

    if ((char *)c->pp == data_ov078_0212703c || (char *)c->pp == data_ov078_021270fc) {
        if (_ZNK12WithMeshClsn8IsOnWallEv(self + 0x110) != 0
            || _ZNK12WithMeshClsn10IsOnGroundEv(self + 0x110) == 0
            || (*(int *)(self + 0x4d8) - 0x28000) > *(int *)(self + 0x60)) {
            func_ov078_02125c48(self, data_ov078_021270bc);
        }
    }

    {
        Vector3 v;
        v.x = data_ov078_02126e00.x;
        v.y = data_ov078_02126e00.y;
        v.z = data_ov078_02126e00.z;
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(self + 0x33c, &v);
    }
    {
        Vector3 v;
        v.x = data_ov078_02126e00.x;
        v.y = data_ov078_02126e00.y;
        v.z = data_ov078_02126e00.z;
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(self + 0x37c, &v);
    }
    _ZN12CylinderClsn5ClearEv(self + 0x33c);
    _ZN12CylinderClsn6UpdateEv(self + 0x33c);
    _ZN12CylinderClsn5ClearEv(self + 0x37c);
    _ZN12CylinderClsn6UpdateEv(self + 0x37c);

    func_ov078_02125de0(self);
    func_ov078_02125c98(self);
    return 1;
}

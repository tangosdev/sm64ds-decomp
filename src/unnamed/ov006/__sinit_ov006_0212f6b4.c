/* Runtime asset handles resolved through the overlay 0 file table. */
enum sinit_ov006_0212f6b4_AssetId {
    ASSET_MARIO_MODEL_MG_BMD = 0x0216
};

extern void func_02017a24(void*, int);
extern void func_020731dc(void*, void*, void*);
extern void _ZN13SharedFilePtr9ConstructEj(void*, int);
extern void func_02017a0c(void);
extern void SharedFilePtr_Destruct_Anim(void);
extern void *_ZN7Vector3D1Ev(void *object);

extern int sTrmpMarioModelFile[];
extern int data_ov006_021406ac[];
extern int sTrmpMarioJumpAnimFile[];
extern int data_ov006_021406c4[];
extern int sTrmpMarioDoubleJump1AnimFile[];
extern int data_ov006_021406d0[];
extern int sTrmpMarioWallJumpAnimFile[];
extern int data_ov006_021406e8[];
extern int sTrmpMarioHangUpAnimFile[];
extern int data_ov006_021406f4[];
extern int sTrmpMarioRollJumpEndAnimFile[];
extern int data_ov006_0214070c[];
extern int sTrmpMarioWalkAnimFile[];
extern int data_ov006_02140718[];
extern int sTrmpMarioDoubleJump2AnimFile[];
extern int data_ov006_02140724[];
extern int sTrmpMarioJumpEndAnimFile[];
extern int data_ov006_02140730[];
extern int sTrmpMarioTurnAnimFile[];
extern int data_ov006_0214073c[];
extern int sTrmpMarioTurnEndAnimFile[];
extern int data_ov006_02140748[];
extern int sTrmpMarioRollJumpAnimFile[];
extern int data_ov006_02140754[];
extern int sTrmpMarioStruggleAnimFile[];
extern int data_ov006_02140760[];
extern int sTrmpMarioSdownAnimFile[];
extern int data_ov006_0214076c[];

struct P2 { int a, b; };
extern struct P2 sTrmpRowBaseHeights;
extern struct P2 sTrmpRowHeights;
extern int data_ov006_0212e000;
extern int sTrmp3DFallSpeed;
extern int data_ov006_0212e020;
extern int sTrmp3DGravityStep;
extern int data_ov006_0212dffc;
extern int sTrmp3DJumpOffSpeed;
extern int data_ov006_0212e008;
extern int sTrmp3DBounceScale;
extern int data_ov006_0212e004;
extern int sTrmp3DHardBounceScale;

extern int data_ov006_0212e010;
extern int sTrmp2FallSpeed;
extern int data_ov006_0212e00c;
extern int sTrmp2GravityStep;
extern int data_ov006_0212e018;
extern int data_ov006_0214053c;
extern int data_ov006_0212e014;
extern int sTrmpSpeedScale;
extern int data_ov006_0212dff8;
extern int sTrmp2RiseSpeedMax;
extern int data_ov006_0212e01c;
extern int sTrmp2DriftScale;

typedef struct { unsigned int x, y, z; } V3;
extern V3 sTrmpGoalSlots;
extern V3 data_ov006_02140784;
extern V3 data_ov006_02140790;
extern V3 data_ov006_0214079c;
extern V3 data_ov006_021407a8;
extern V3 data_ov006_021407b4;
extern int data_ov006_02140640[];
extern int data_ov006_0214064c[];
extern int data_ov006_02140658[];
extern int data_ov006_02140664[];
extern int data_ov006_02140670[];
extern int data_ov006_0214067c[];

extern V3 data_ov006_021407c0;
extern V3 data_ov006_021407cc;
extern V3 data_ov006_021407d8;
extern V3 data_ov006_021407e4;
extern V3 data_ov006_021407f0;
extern V3 data_ov006_021407fc;
extern int data_ov006_02140688[];
extern int data_ov006_02140694[];
extern int data_ov006_021406a0[];
extern int data_ov006_021406b8[];
extern int data_ov006_021406dc[];
extern int data_ov006_02140700[];

void __sinit_ov006_0212f6b4(void)
{
    func_02017a24(sTrmpMarioModelFile, ASSET_MARIO_MODEL_MG_BMD);
    func_020731dc(sTrmpMarioModelFile, (void *)&func_02017a0c, data_ov006_021406ac);
    _ZN13SharedFilePtr9ConstructEj(sTrmpMarioJumpAnimFile, 0x8078);
    func_020731dc(sTrmpMarioJumpAnimFile, (void *)&SharedFilePtr_Destruct_Anim,
                  data_ov006_021406c4);
    _ZN13SharedFilePtr9ConstructEj(sTrmpMarioDoubleJump1AnimFile, 0x801d);
    func_020731dc(sTrmpMarioDoubleJump1AnimFile, (void *)&SharedFilePtr_Destruct_Anim,
                  data_ov006_021406d0);
    _ZN13SharedFilePtr9ConstructEj(sTrmpMarioWallJumpAnimFile, 0x80be);
    func_020731dc(sTrmpMarioWallJumpAnimFile, (void *)&SharedFilePtr_Destruct_Anim,
                  data_ov006_021406e8);
    _ZN13SharedFilePtr9ConstructEj(sTrmpMarioHangUpAnimFile, 0x8071);
    func_020731dc(sTrmpMarioHangUpAnimFile, (void *)&SharedFilePtr_Destruct_Anim,
                  data_ov006_021406f4);
    _ZN13SharedFilePtr9ConstructEj(sTrmpMarioRollJumpEndAnimFile, 0x808f);
    func_020731dc(sTrmpMarioRollJumpEndAnimFile, (void *)&SharedFilePtr_Destruct_Anim,
                  data_ov006_0214070c);
    _ZN13SharedFilePtr9ConstructEj(sTrmpMarioWalkAnimFile, 0x80b4);
    func_020731dc(sTrmpMarioWalkAnimFile, (void *)&SharedFilePtr_Destruct_Anim,
                  data_ov006_02140718);
    _ZN13SharedFilePtr9ConstructEj(sTrmpMarioDoubleJump2AnimFile, 0x801e);
    func_020731dc(sTrmpMarioDoubleJump2AnimFile, (void *)&SharedFilePtr_Destruct_Anim,
                  data_ov006_02140724);
    _ZN13SharedFilePtr9ConstructEj(sTrmpMarioJumpEndAnimFile, 0x8077);
    func_020731dc(sTrmpMarioJumpEndAnimFile, (void *)&SharedFilePtr_Destruct_Anim,
                  data_ov006_02140730);
    _ZN13SharedFilePtr9ConstructEj(sTrmpMarioTurnAnimFile, 0x80ac);
    func_020731dc(sTrmpMarioTurnAnimFile, (void *)&SharedFilePtr_Destruct_Anim,
                  data_ov006_0214073c);
    _ZN13SharedFilePtr9ConstructEj(sTrmpMarioTurnEndAnimFile, 0x80ab);
    func_020731dc(sTrmpMarioTurnEndAnimFile, (void *)&SharedFilePtr_Destruct_Anim,
                  data_ov006_02140748);
    _ZN13SharedFilePtr9ConstructEj(sTrmpMarioRollJumpAnimFile, 0x808e);
    func_020731dc(sTrmpMarioRollJumpAnimFile, (void *)&SharedFilePtr_Destruct_Anim,
                  data_ov006_02140754);
    _ZN13SharedFilePtr9ConstructEj(sTrmpMarioStruggleAnimFile, 0x80a4);
    func_020731dc(sTrmpMarioStruggleAnimFile, (void *)&SharedFilePtr_Destruct_Anim,
                  data_ov006_02140760);
    _ZN13SharedFilePtr9ConstructEj(sTrmpMarioSdownAnimFile, 0x8092);
    func_020731dc(sTrmpMarioSdownAnimFile, (void *)&SharedFilePtr_Destruct_Anim,
                  data_ov006_0214076c);

    sTrmpRowHeights.a = sTrmpRowBaseHeights.a;
    sTrmpRowHeights.b = sTrmpRowBaseHeights.b;
    sTrmp3DFallSpeed = data_ov006_0212e000;
    sTrmp3DGravityStep = data_ov006_0212e020;
    sTrmp3DJumpOffSpeed = data_ov006_0212dffc;
    sTrmp3DBounceScale = data_ov006_0212e008;
    sTrmp3DHardBounceScale = data_ov006_0212e004;

    sTrmpGoalSlots.x = (unsigned int)-0x50000;
    sTrmpGoalSlots.y = 0x100000;
    sTrmpGoalSlots.z = 0;
    func_020731dc(&sTrmpGoalSlots, (void *)&_ZN7Vector3D1Ev, data_ov006_02140640);

    {
        unsigned int *p = (unsigned int*)&data_ov006_02140784;
        p[0] = (unsigned int)-0x30000;
        p[1] = 0x100000;
        p[2] = 0;
        func_020731dc(p, (void*)&_ZN7Vector3D1Ev, data_ov006_0214064c);
    }

    {
        unsigned int *p = (unsigned int*)&data_ov006_02140790;
        p[0] = 0x10000;
        p[1] = 0x100000;
        p[2] = 0;
        func_020731dc(p, (void*)&_ZN7Vector3D1Ev, data_ov006_02140658);
    }

    {
        unsigned int *p = (unsigned int*)&data_ov006_0214079c;
        p[0] = 0x10000;
        p[1] = 0x100000;
        p[2] = 0;
        func_020731dc(p, (void*)&_ZN7Vector3D1Ev, data_ov006_02140664);
    }

    {
        unsigned int *p = (unsigned int*)&data_ov006_021407a8;
        p[0] = 0x30000;
        p[1] = 0x100000;
        p[2] = 0;
        func_020731dc(p, (void*)&_ZN7Vector3D1Ev, data_ov006_02140670);
    }

    {
        unsigned int *p = (unsigned int*)&data_ov006_021407b4;
        p[0] = 0x50000;
        p[1] = 0x100000;
        p[2] = 0;
        func_020731dc(p, (void*)&_ZN7Vector3D1Ev, data_ov006_0214067c);
    }

    sTrmp2FallSpeed = data_ov006_0212e010;
    sTrmp2GravityStep = data_ov006_0212e00c;
    data_ov006_0214053c = data_ov006_0212e018;
    sTrmpSpeedScale = data_ov006_0212e014;
    sTrmp2RiseSpeedMax = data_ov006_0212dff8;
    sTrmp2DriftScale = data_ov006_0212e01c;

    data_ov006_021407c0.x = (unsigned int)-0x50000;
    data_ov006_021407c0.y = 0x100000;
    data_ov006_021407c0.z = 0;
    func_020731dc(&data_ov006_021407c0, (void*)&_ZN7Vector3D1Ev, data_ov006_02140688);

    {
        unsigned int *p = (unsigned int*)&data_ov006_021407cc;
        p[0] = (unsigned int)-0x30000;
        p[1] = 0x100000;
        p[2] = 0;
        func_020731dc(p, (void*)&_ZN7Vector3D1Ev, data_ov006_02140694);
    }

    {
        unsigned int *p = (unsigned int*)&data_ov006_021407d8;
        p[0] = 0x10000;
        p[1] = 0x100000;
        p[2] = 0;
        func_020731dc(p, (void*)&_ZN7Vector3D1Ev, data_ov006_021406a0);
    }

    {
        unsigned int *p = (unsigned int*)&data_ov006_021407e4;
        p[0] = 0x10000;
        p[1] = 0x100000;
        p[2] = 0;
        func_020731dc(p, (void*)&_ZN7Vector3D1Ev, data_ov006_021406b8);
    }

    {
        unsigned int *p = (unsigned int*)&data_ov006_021407f0;
        p[0] = 0x30000;
        p[1] = 0x100000;
        p[2] = 0;
        func_020731dc(p, (void*)&_ZN7Vector3D1Ev, data_ov006_021406dc);
    }

    {
        unsigned int *p = (unsigned int*)&data_ov006_021407fc;
        p[0] = 0x50000;
        p[1] = 0x100000;
        p[2] = 0;
        func_020731dc(p, (void*)&_ZN7Vector3D1Ev, data_ov006_02140700);
    }
}

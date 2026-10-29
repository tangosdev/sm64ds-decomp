//cpp
/* ov006 static initializer at 0x0212f6b4 (0x5c8): the Trampoline minigames'
 * Mario resources and tuning, all read by d_mg_trmpln2_mario.cpp.
 *
 * It loads Mario's model handle and the thirteen animation handles, each
 * registered for destruction; copies the trampoline row heights and the
 * 2D/3D speed constants into their .bss working copies; and builds the two
 * banks of six goal slots, each Vector3 registered with ~Vector3().
 *
 * Not folded: its owner is d_mg_trmpln2_mario.cpp (.init and .bss follow that
 * TU's .text), but compiler emission needs every .bss object in
 * 0x02140538..0x02140808 defined there in retail order, and the goal slots
 * only match through unsigned int stores, not through a Vector3 constructor.
 */
#include "types.h"
#include "SharedFilePtr.h"

/* Runtime asset handles resolved through the overlay 0 file table. */
enum TrmpMarioAssetId { ASSET_MARIO_MODEL_MG_BMD = 0x0216 };

extern "C" {
/* local extern: no project header declares these arm9 helpers. */
void func_02017a24(void *, int);
void func_020731dc(void *, void *, void **);
void func_02017a0c(void);
int SharedFilePtr_Destruct_Anim(int);
/* local extern: C++ cannot take a destructor's address, so the registrations
   name ~Vector3() by its linker name. */
void *_ZN7Vector3D1Ev(void *object);
}

/* Mario's model and animation handles, each followed by the
   destructor-registration node (data_ov006_*) the initializer passes. */
extern int sTrmpMarioModelFile[];
extern void *data_ov006_021406ac[3];
extern SharedFilePtr sTrmpMarioJumpAnimFile;
extern void *data_ov006_021406c4[3];
extern SharedFilePtr sTrmpMarioDoubleJump1AnimFile;
extern void *data_ov006_021406d0[3];
extern SharedFilePtr sTrmpMarioWallJumpAnimFile;
extern void *data_ov006_021406e8[3];
extern SharedFilePtr sTrmpMarioHangUpAnimFile;
extern void *data_ov006_021406f4[3];
extern SharedFilePtr sTrmpMarioRollJumpEndAnimFile;
extern void *data_ov006_0214070c[3];
extern SharedFilePtr sTrmpMarioWalkAnimFile;
extern void *data_ov006_02140718[3];
extern SharedFilePtr sTrmpMarioDoubleJump2AnimFile;
extern void *data_ov006_02140724[3];
extern SharedFilePtr sTrmpMarioJumpEndAnimFile;
extern void *data_ov006_02140730[3];
extern SharedFilePtr sTrmpMarioTurnAnimFile;
extern void *data_ov006_0214073c[3];
extern SharedFilePtr sTrmpMarioTurnEndAnimFile;
extern void *data_ov006_02140748[3];
extern SharedFilePtr sTrmpMarioRollJumpAnimFile;
extern void *data_ov006_02140754[3];
extern SharedFilePtr sTrmpMarioStruggleAnimFile;
extern void *data_ov006_02140760[3];
extern SharedFilePtr sTrmpMarioSdownAnimFile;
extern void *data_ov006_0214076c[3];

/* Tuning: each .bss working copy takes its .data default. */
/* The two trampoline row heights, then the speed constants. */
extern int sTrmpRowBaseHeights[];
extern int sTrmpRowHeights[];
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

/* The first bank of goal slots (sTrmpGoalSlots) and its registration nodes. */
extern Vector3 sTrmpGoalSlots;
extern Vector3 data_ov006_02140784;
extern Vector3 data_ov006_02140790;
extern Vector3 data_ov006_0214079c;
extern Vector3 data_ov006_021407a8;
extern Vector3 data_ov006_021407b4;
extern void *data_ov006_02140640[3];
extern void *data_ov006_0214064c[3];
extern void *data_ov006_02140658[3];
extern void *data_ov006_02140664[3];
extern void *data_ov006_02140670[3];
extern void *data_ov006_0214067c[3];

/* The second bank of goal slots and its registration nodes. */
extern Vector3 data_ov006_021407c0;
extern Vector3 data_ov006_021407cc;
extern Vector3 data_ov006_021407d8;
extern Vector3 data_ov006_021407e4;
extern Vector3 data_ov006_021407f0;
extern Vector3 data_ov006_021407fc;
extern void *data_ov006_02140688[3];
extern void *data_ov006_02140694[3];
extern void *data_ov006_021406a0[3];
extern void *data_ov006_021406b8[3];
extern void *data_ov006_021406dc[3];
extern void *data_ov006_02140700[3];

extern "C" void __sinit_ov006_0212f6b4(void) {
    func_02017a24(sTrmpMarioModelFile, ASSET_MARIO_MODEL_MG_BMD);
    func_020731dc(sTrmpMarioModelFile, (void *)&func_02017a0c, data_ov006_021406ac);
    sTrmpMarioJumpAnimFile.Construct(0x8078);
    func_020731dc(&sTrmpMarioJumpAnimFile, (void *)SharedFilePtr_Destruct_Anim,
                  data_ov006_021406c4);
    sTrmpMarioDoubleJump1AnimFile.Construct(0x801d);
    func_020731dc(&sTrmpMarioDoubleJump1AnimFile, (void *)SharedFilePtr_Destruct_Anim,
                  data_ov006_021406d0);
    sTrmpMarioWallJumpAnimFile.Construct(0x80be);
    func_020731dc(&sTrmpMarioWallJumpAnimFile, (void *)SharedFilePtr_Destruct_Anim,
                  data_ov006_021406e8);
    sTrmpMarioHangUpAnimFile.Construct(0x8071);
    func_020731dc(&sTrmpMarioHangUpAnimFile, (void *)SharedFilePtr_Destruct_Anim,
                  data_ov006_021406f4);
    sTrmpMarioRollJumpEndAnimFile.Construct(0x808f);
    func_020731dc(&sTrmpMarioRollJumpEndAnimFile, (void *)SharedFilePtr_Destruct_Anim,
                  data_ov006_0214070c);
    sTrmpMarioWalkAnimFile.Construct(0x80b4);
    func_020731dc(&sTrmpMarioWalkAnimFile, (void *)SharedFilePtr_Destruct_Anim,
                  data_ov006_02140718);
    sTrmpMarioDoubleJump2AnimFile.Construct(0x801e);
    func_020731dc(&sTrmpMarioDoubleJump2AnimFile, (void *)SharedFilePtr_Destruct_Anim,
                  data_ov006_02140724);
    sTrmpMarioJumpEndAnimFile.Construct(0x8077);
    func_020731dc(&sTrmpMarioJumpEndAnimFile, (void *)SharedFilePtr_Destruct_Anim,
                  data_ov006_02140730);
    sTrmpMarioTurnAnimFile.Construct(0x80ac);
    func_020731dc(&sTrmpMarioTurnAnimFile, (void *)SharedFilePtr_Destruct_Anim,
                  data_ov006_0214073c);
    sTrmpMarioTurnEndAnimFile.Construct(0x80ab);
    func_020731dc(&sTrmpMarioTurnEndAnimFile, (void *)SharedFilePtr_Destruct_Anim,
                  data_ov006_02140748);
    sTrmpMarioRollJumpAnimFile.Construct(0x808e);
    func_020731dc(&sTrmpMarioRollJumpAnimFile, (void *)SharedFilePtr_Destruct_Anim,
                  data_ov006_02140754);
    sTrmpMarioStruggleAnimFile.Construct(0x80a4);
    func_020731dc(&sTrmpMarioStruggleAnimFile, (void *)SharedFilePtr_Destruct_Anim,
                  data_ov006_02140760);
    sTrmpMarioSdownAnimFile.Construct(0x8092);
    func_020731dc(&sTrmpMarioSdownAnimFile, (void *)SharedFilePtr_Destruct_Anim,
                  data_ov006_0214076c);

    sTrmpRowHeights[0] = sTrmpRowBaseHeights[0];
    sTrmpRowHeights[1] = sTrmpRowBaseHeights[1];
    sTrmp3DFallSpeed = data_ov006_0212e000;
    sTrmp3DGravityStep = data_ov006_0212e020;
    sTrmp3DJumpOffSpeed = data_ov006_0212dffc;
    sTrmp3DBounceScale = data_ov006_0212e008;
    sTrmp3DHardBounceScale = data_ov006_0212e004;

    sTrmpGoalSlots.x = -0x50000;
    sTrmpGoalSlots.y = 0x100000;
    sTrmpGoalSlots.z = 0;
    func_020731dc(&sTrmpGoalSlots, (void *)&_ZN7Vector3D1Ev, data_ov006_02140640);

    /* The remaining slots are written through an unsigned int*: plain
       .x/.y/.z stores (as above) compile to different code and lose the match. */
    {
        unsigned int *p = (unsigned int *)&data_ov006_02140784;
        p[0] = (unsigned int)-0x30000;
        p[1] = 0x100000;
        p[2] = 0;
        func_020731dc(p, (void *)&_ZN7Vector3D1Ev, data_ov006_0214064c);
    }

    {
        unsigned int *p = (unsigned int *)&data_ov006_02140790;
        p[0] = 0x10000;
        p[1] = 0x100000;
        p[2] = 0;
        func_020731dc(p, (void *)&_ZN7Vector3D1Ev, data_ov006_02140658);
    }

    {
        unsigned int *p = (unsigned int *)&data_ov006_0214079c;
        p[0] = 0x10000;
        p[1] = 0x100000;
        p[2] = 0;
        func_020731dc(p, (void *)&_ZN7Vector3D1Ev, data_ov006_02140664);
    }

    {
        unsigned int *p = (unsigned int *)&data_ov006_021407a8;
        p[0] = 0x30000;
        p[1] = 0x100000;
        p[2] = 0;
        func_020731dc(p, (void *)&_ZN7Vector3D1Ev, data_ov006_02140670);
    }

    {
        unsigned int *p = (unsigned int *)&data_ov006_021407b4;
        p[0] = 0x50000;
        p[1] = 0x100000;
        p[2] = 0;
        func_020731dc(p, (void *)&_ZN7Vector3D1Ev, data_ov006_0214067c);
    }

    sTrmp2FallSpeed = data_ov006_0212e010;
    sTrmp2GravityStep = data_ov006_0212e00c;
    data_ov006_0214053c = data_ov006_0212e018;
    sTrmpSpeedScale = data_ov006_0212e014;
    sTrmp2RiseSpeedMax = data_ov006_0212dff8;
    sTrmp2DriftScale = data_ov006_0212e01c;

    data_ov006_021407c0.x = -0x50000;
    data_ov006_021407c0.y = 0x100000;
    data_ov006_021407c0.z = 0;
    func_020731dc(&data_ov006_021407c0, (void *)&_ZN7Vector3D1Ev, data_ov006_02140688);

    {
        unsigned int *p = (unsigned int *)&data_ov006_021407cc;
        p[0] = (unsigned int)-0x30000;
        p[1] = 0x100000;
        p[2] = 0;
        func_020731dc(p, (void *)&_ZN7Vector3D1Ev, data_ov006_02140694);
    }

    {
        unsigned int *p = (unsigned int *)&data_ov006_021407d8;
        p[0] = 0x10000;
        p[1] = 0x100000;
        p[2] = 0;
        func_020731dc(p, (void *)&_ZN7Vector3D1Ev, data_ov006_021406a0);
    }

    {
        unsigned int *p = (unsigned int *)&data_ov006_021407e4;
        p[0] = 0x10000;
        p[1] = 0x100000;
        p[2] = 0;
        func_020731dc(p, (void *)&_ZN7Vector3D1Ev, data_ov006_021406b8);
    }

    {
        unsigned int *p = (unsigned int *)&data_ov006_021407f0;
        p[0] = 0x30000;
        p[1] = 0x100000;
        p[2] = 0;
        func_020731dc(p, (void *)&_ZN7Vector3D1Ev, data_ov006_021406dc);
    }

    {
        unsigned int *p = (unsigned int *)&data_ov006_021407fc;
        p[0] = 0x50000;
        p[1] = 0x100000;
        p[2] = 0;
        func_020731dc(p, (void *)&_ZN7Vector3D1Ev, data_ov006_02140700);
    }
}

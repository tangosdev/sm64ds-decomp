//cpp
/* Static initializer for the monkey (daMky_c, d_a_mky.cpp): the model and
 * ten animation handles and the four Vector3 carry offsets, each registered
 * with __register_global_object (func_020731dc) on a 12-byte node, then the
 * 11-entry state table data_ov030_02115e0c (daMky_c::StateFunctions:
 * an enter and an update pointer-to-member each), filled from the constants at
 * 0x02115ac8..0x02115b78.
 *
 * Not folded: for mwcc to emit this from d_a_mky.cpp, that TU would have to
 * define the 22 constants as member-pointer literals and claim the .data
 * run they sit in, which also holds two profiles, the 11-entry animation
 * table and the vtable. */
#include "SharedFilePtr.h"

/* Runtime asset handles resolved through the overlay 0 file table. */
enum sinit_ov030_02114924_AssetId {
    ASSET_MONKEY_BMD = 0x03aa,
    ASSET_MONKEY_HOLD_BCA = 0x03ab,
    ASSET_MONKEY_JUMP_ED_BCA = 0x03ac,
    ASSET_MONKEY_JUMP_ST_BCA = 0x03ad,
    ASSET_MONKEY_RUN_BCA = 0x03ae,
    ASSET_MONKEY_RUN_ED_BCA = 0x03af,
    ASSET_MONKEY_RUN_ST_BCA = 0x03b0,
    ASSET_MONKEY_WAIT1_BCA = 0x03b1,
    ASSET_MONKEY_WAIT2_BCA = 0x03b2,
    ASSET_MONKEY_WAIT3_BCA = 0x03b3,
    ASSET_MONKEY_WAIT4_BCA = 0x03b4
};

/* local extern: SharedFilePtr.cpp defines these veneers with C linkage, and
   SharedFilePtr.h, the only header for that file, does not declare them. */
extern "C" {
void *func_02017acc(void *, unsigned int); /* model handle constructor */
int func_02017ab4(int);                    /* model handle destructor */
int SharedFilePtr_Destruct_Anim(int);
void func_020731dc(void *, void *, void **); /* __register_global_object */
/* local extern: C++ cannot take a destructor's address, so the
   registrations name ~Vector3() by its linker name. */
void *_ZN7Vector3D1Ev(void *);
}

/* The handles, in the order this initializer builds them. */
extern SharedFilePtr data_ov030_02115d00;
extern SharedFilePtr gMonkeyHoldAnimationFile;
extern SharedFilePtr gMonkeyJumpStAnimationFile;
extern SharedFilePtr gMonkeyJumpEdAnimationFile;
extern SharedFilePtr gMonkeyRunStAnimationFile;
extern SharedFilePtr gMonkeyRunAnimationFile;
extern SharedFilePtr gMonkeyRunEdAnimationFile;
extern SharedFilePtr data_ov030_02115cf0;
extern SharedFilePtr gMonkeyWait2AnimationFile;
extern SharedFilePtr gMonkeyWait3AnimationFile;
extern SharedFilePtr gMonkeyWait4AnimationFile;

/* The four carry offsets d_a_mky.cpp indexes as one Vector3[4]. */
extern Vector3 data_ov030_02115ddc;
extern Vector3 data_ov030_02115de8;
extern Vector3 data_ov030_02115df4;
extern Vector3 data_ov030_02115e00;

/* The registration nodes: the handles', then the offsets'. */
extern void *data_ov030_02115d28[3];
extern void *data_ov030_02115d34[3];
extern void *data_ov030_02115d40[3];
extern void *data_ov030_02115d64[3];
extern void *data_ov030_02115d70[3];
extern void *data_ov030_02115d88[3];
extern void *data_ov030_02115d94[3];
extern void *data_ov030_02115dac[3];
extern void *data_ov030_02115db8[3];
extern void *data_ov030_02115dc4[3];
extern void *data_ov030_02115dd0[3];
extern void *data_ov030_02115d58[3];
extern void *data_ov030_02115d4c[3];
extern void *data_ov030_02115d7c[3];
extern void *data_ov030_02115da0[3];

/* An 8-byte pointer-to-member constant, copied as a unit. Under C++ a
   struct of two scalar fields is assigned field by field, which reorders
   the loads; an array member keeps the retail schedule. */
struct PmfWords {
    int w[2];
};

extern PmfWords data_ov030_02115e0c[22];

/* Its constants, in the order this initializer copies them. */
extern PmfWords data_ov030_02115b68;
extern PmfWords data_ov030_02115b60;
extern PmfWords data_ov030_02115af0;
extern PmfWords data_ov030_02115b50;
extern PmfWords data_ov030_02115b00;
extern PmfWords data_ov030_02115b40;
extern PmfWords data_ov030_02115b48;
extern PmfWords data_ov030_02115b18;
extern PmfWords data_ov030_02115b10;
extern PmfWords data_ov030_02115ad8;
extern PmfWords data_ov030_02115b58;
extern PmfWords data_ov030_02115b20;
extern PmfWords data_ov030_02115ae0;
extern PmfWords data_ov030_02115b28;
extern PmfWords data_ov030_02115ae8;
extern PmfWords data_ov030_02115b30;
extern PmfWords data_ov030_02115ac8;
extern PmfWords data_ov030_02115ad0;
extern PmfWords data_ov030_02115b08;
extern PmfWords data_ov030_02115b70;
extern PmfWords data_ov030_02115af8;
extern PmfWords data_ov030_02115b38;

extern "C" void __sinit_ov030_02114924(void) {
    func_02017acc(&data_ov030_02115d00, ASSET_MONKEY_BMD);
    func_020731dc(&data_ov030_02115d00, (void *)func_02017ab4, data_ov030_02115d28);
    gMonkeyHoldAnimationFile.Construct(ASSET_MONKEY_HOLD_BCA);
    func_020731dc(&gMonkeyHoldAnimationFile, (void *)SharedFilePtr_Destruct_Anim,
                  data_ov030_02115d34);
    gMonkeyJumpStAnimationFile.Construct(ASSET_MONKEY_JUMP_ST_BCA);
    func_020731dc(&gMonkeyJumpStAnimationFile, (void *)SharedFilePtr_Destruct_Anim,
                  data_ov030_02115d40);
    gMonkeyJumpEdAnimationFile.Construct(ASSET_MONKEY_JUMP_ED_BCA);
    func_020731dc(&gMonkeyJumpEdAnimationFile, (void *)SharedFilePtr_Destruct_Anim,
                  data_ov030_02115d64);
    gMonkeyRunStAnimationFile.Construct(ASSET_MONKEY_RUN_ST_BCA);
    func_020731dc(&gMonkeyRunStAnimationFile, (void *)SharedFilePtr_Destruct_Anim,
                  data_ov030_02115d70);
    gMonkeyRunAnimationFile.Construct(ASSET_MONKEY_RUN_BCA);
    func_020731dc(&gMonkeyRunAnimationFile, (void *)SharedFilePtr_Destruct_Anim,
                  data_ov030_02115d88);
    gMonkeyRunEdAnimationFile.Construct(ASSET_MONKEY_RUN_ED_BCA);
    func_020731dc(&gMonkeyRunEdAnimationFile, (void *)SharedFilePtr_Destruct_Anim,
                  data_ov030_02115d94);
    data_ov030_02115cf0.Construct(ASSET_MONKEY_WAIT1_BCA);
    func_020731dc(&data_ov030_02115cf0, (void *)SharedFilePtr_Destruct_Anim, data_ov030_02115dac);
    gMonkeyWait2AnimationFile.Construct(ASSET_MONKEY_WAIT2_BCA);
    func_020731dc(&gMonkeyWait2AnimationFile, (void *)SharedFilePtr_Destruct_Anim,
                  data_ov030_02115db8);
    gMonkeyWait3AnimationFile.Construct(ASSET_MONKEY_WAIT3_BCA);
    func_020731dc(&gMonkeyWait3AnimationFile, (void *)SharedFilePtr_Destruct_Anim,
                  data_ov030_02115dc4);
    gMonkeyWait4AnimationFile.Construct(ASSET_MONKEY_WAIT4_BCA);
    func_020731dc(&gMonkeyWait4AnimationFile, (void *)SharedFilePtr_Destruct_Anim,
                  data_ov030_02115dd0);

    data_ov030_02115e0c[0] = data_ov030_02115b68;
    data_ov030_02115e0c[1] = data_ov030_02115b60;
    data_ov030_02115e0c[2] = data_ov030_02115af0;
    data_ov030_02115e0c[3] = data_ov030_02115b50;
    data_ov030_02115e0c[4] = data_ov030_02115b00;
    data_ov030_02115e0c[5] = data_ov030_02115b40;
    data_ov030_02115e0c[6] = data_ov030_02115b48;
    data_ov030_02115e0c[7] = data_ov030_02115b18;
    data_ov030_02115e0c[8] = data_ov030_02115b10;
    data_ov030_02115e0c[9] = data_ov030_02115ad8;
    data_ov030_02115e0c[10] = data_ov030_02115b58;
    data_ov030_02115e0c[11] = data_ov030_02115b20;
    data_ov030_02115e0c[12] = data_ov030_02115ae0;
    data_ov030_02115e0c[13] = data_ov030_02115b28;
    data_ov030_02115e0c[14] = data_ov030_02115ae8;
    data_ov030_02115e0c[15] = data_ov030_02115b30;
    data_ov030_02115e0c[16] = data_ov030_02115ac8;
    data_ov030_02115e0c[17] = data_ov030_02115ad0;
    data_ov030_02115e0c[18] = data_ov030_02115b08;
    data_ov030_02115e0c[19] = data_ov030_02115b70;
    data_ov030_02115e0c[20] = data_ov030_02115af8;
    data_ov030_02115e0c[21] = data_ov030_02115b38;

    data_ov030_02115ddc.x = 0x1c000;
    data_ov030_02115ddc.y = -0x42000;
    data_ov030_02115ddc.z = 0x18000;
    func_020731dc(&data_ov030_02115ddc, (void *)&_ZN7Vector3D1Ev, data_ov030_02115d58);
    /* The other three store through an unsigned int view. Through the
       Vector3 fields, mwcc stores z ahead of y (measured). */
    {
        unsigned int *p = (unsigned int *)&data_ov030_02115de8;
        p[0] = 0x1c000;
        p[1] = (unsigned int)-0x28000;
        p[2] = 0x28000;
        func_020731dc(p, (void *)&_ZN7Vector3D1Ev, data_ov030_02115d4c);
    }
    {
        unsigned int *p = (unsigned int *)&data_ov030_02115df4;
        p[0] = 0x2b000;
        p[1] = (unsigned int)-0x42000;
        p[2] = 0x18000;
        func_020731dc(p, (void *)&_ZN7Vector3D1Ev, data_ov030_02115d7c);
    }
    {
        unsigned int *p = (unsigned int *)&data_ov030_02115e00;
        p[0] = 0x30000;
        p[1] = (unsigned int)-0x1e000;
        p[2] = 0x1e000;
        func_020731dc(p, (void *)&_ZN7Vector3D1Ev, data_ov030_02115da0);
    }
}

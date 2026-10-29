//cpp
/* Static initializer for the ending demo (daDemo_c): the Peach, letter,
 * pipe and cloud file handles, each registered with
 * __register_global_object (func_020731dc) on a 12-byte node, then the
 * ten pointer-to-member state tables d_a_demo.cpp dispatches through
 * (0x02110e24..0x0211104c), filled from the constants at
 * 0x0210b650..0x0210b8b8.
 *
 * Not folded: the owner is d_a_demo.cpp, whose initializer this is (it
 * sits between bc_switch's and d_map's, in the demo's .text slot). For mwcc
 * to emit it, d_a_demo.cpp would have to define the 69 constants as
 * member-pointer literals, which means claiming the demo's whole .data run
 * they are scattered through. */
#include "SharedFilePtr.h"

/* Runtime asset handles resolved through the overlay 0 file table. */
enum sinit_ov002_02107370_AssetId {
    ASSET_PEACH_ED_ENTER_WAIT_BCA = 0x03d6,
    ASSET_PEACH_ED_ENTER_WAIT_BTP = 0x03d7,
    ASSET_PEACH_ED_KISS_BCA = 0x03d8,
    ASSET_PEACH_ED_KISS_BTP = 0x03d9,
    ASSET_PEACH_ED_LANDING_BCA = 0x03da,
    ASSET_PEACH_ED_LANDING_BTP = 0x03db,
    ASSET_PEACH_ED_LOOKUP_BCA = 0x03dc,
    ASSET_PEACH_ED_LOOKUP_BTP = 0x03dd,
    ASSET_PEACH_ED_LOOKUP_TO_WAIT_BCA = 0x03de,
    ASSET_PEACH_ED_LOOKUP_TO_WAIT_BTP = 0x03df,
    ASSET_PEACH_ED_LOOK_ABOUT_BCA = 0x03e0,
    ASSET_PEACH_ED_LOOK_ABOUT_BTP = 0x03e1,
    ASSET_PEACH_ED_NOTICE_MARIO_BCA = 0x03e2,
    ASSET_PEACH_ED_NOTICE_MARIO_BTP = 0x03e3,
    ASSET_PEACH_ED_NOTICE_YOSHI_BCA = 0x03e4,
    ASSET_PEACH_ED_NOTICE_YOSHI_BTP = 0x03e5,
    ASSET_PEACH_ED_STEP_UP_BCA = 0x03e6,
    ASSET_PEACH_ED_STEP_UP_BTP = 0x03e7,
    ASSET_PEACH_ED_TURN_BCA = 0x03e8,
    ASSET_PEACH_ED_TURN_BTP = 0x03e9,
    ASSET_PEACH_ED_WAIT_BCA = 0x03ea,
    ASSET_PEACH_ED_WAIT_BTP = 0x03eb,
    ASSET_PEACH_ED_WALK_BCA = 0x03ec,
    ASSET_PEACH_ED_WAVE_BCA = 0x03ed,
    ASSET_PEACH_ED_WAVE_BTP = 0x03ee,
    ASSET_PEACH_HIGH_BMD = 0x03ef,
    ASSET_PEACH_LOW_BMD = 0x03f3,
    ASSET_PEACH_OP_WAIT_BCA = 0x03f4,
    ASSET_PEACH_OP_WINK_BTP = 0x03f5,
    ASSET_PEACH_WAIT_BTP = 0x03f8,
    ASSET_PEACH_WALK_BTP = 0x03fa,
    ASSET_OBJ_DOKAN_BMD = 0x0461,
    ASSET_OBJ_KUMO_BMD = 0x0475,
    ASSET_PEACH_LETTER_US_BMD = 0x049b
};

/* local extern: SharedFilePtr.cpp defines these veneers with C linkage, and
   SharedFilePtr.h, the only header for that file, does not declare them. */
extern "C" {
void *func_02017acc(void *, unsigned int); /* model handle constructor */
int func_02017ab4(int);                    /* model handle destructor */
int SharedFilePtr_Destruct_Anim(int);
void *SharedFilePtr_Construct_TexSeq(void *, unsigned int);
int SharedFilePtr_Destruct_TexSeq(int);
void func_020731dc(void *, void *, void **); /* __register_global_object */
}

/* The handles, in the order this initializer builds them, and their nodes. */
extern SharedFilePtr data_ov002_02110b50;
extern SharedFilePtr data_ov002_02110b98;
extern SharedFilePtr data_ov002_02110c18;
extern SharedFilePtr data_ov002_02110bf8;
extern SharedFilePtr data_ov002_02110c10;
extern SharedFilePtr data_ov002_02110bd0;
extern SharedFilePtr data_ov002_02110b28;
extern SharedFilePtr data_ov002_02110bf0;
extern SharedFilePtr data_ov002_02110b68;
extern SharedFilePtr data_ov002_02110ba0;
extern SharedFilePtr data_ov002_02110bb0;
extern SharedFilePtr data_ov002_02110b48;
extern SharedFilePtr data_ov002_02110b30;
extern SharedFilePtr data_ov002_02110be8;
extern SharedFilePtr data_ov002_02110c08;
extern SharedFilePtr data_ov002_02110c00;
extern SharedFilePtr data_ov002_02110bd8;
extern SharedFilePtr data_ov002_02110b60;
extern SharedFilePtr data_ov002_02110be0;
extern SharedFilePtr data_ov002_02110b90;
extern SharedFilePtr data_ov002_02110bc0;
extern SharedFilePtr data_ov002_02110b80;
extern SharedFilePtr data_ov002_02110b88;
extern SharedFilePtr data_ov002_02110b38;
extern SharedFilePtr data_ov002_02110ba8;
extern SharedFilePtr data_ov002_02110b20;
extern SharedFilePtr data_ov002_02110b40;
extern SharedFilePtr data_ov002_02110b58;
extern SharedFilePtr data_ov002_02110bb8;
extern SharedFilePtr data_ov002_02110b10;
extern SharedFilePtr data_ov002_02110bc8;
extern SharedFilePtr data_ov002_02110b18;
extern SharedFilePtr data_ov002_02110b70;
extern SharedFilePtr data_ov002_02110b78;

extern void *data_ov002_02110dc4[3];
extern void *data_ov002_02110c2c[3];
extern void *data_ov002_02110c38[3];
extern void *data_ov002_02110c44[3];
extern void *data_ov002_02110c50[3];
extern void *data_ov002_02110c5c[3];
extern void *data_ov002_02110c68[3];
extern void *data_ov002_02110c74[3];
extern void *data_ov002_02110c80[3];
extern void *data_ov002_02110c8c[3];
extern void *data_ov002_02110c98[3];
extern void *data_ov002_02110ca4[3];
extern void *data_ov002_02110cb0[3];
extern void *data_ov002_02110cbc[3];
extern void *data_ov002_02110cc8[3];
extern void *data_ov002_02110cd4[3];
extern void *data_ov002_02110ce0[3];
extern void *data_ov002_02110cec[3];
extern void *data_ov002_02110cf8[3];
extern void *data_ov002_02110d04[3];
extern void *data_ov002_02110d10[3];
extern void *data_ov002_02110d1c[3];
extern void *data_ov002_02110dd0[3];
extern void *data_ov002_02110d28[3];
extern void *data_ov002_02110d34[3];
extern void *data_ov002_02110d40[3];
extern void *data_ov002_02110d4c[3];
extern void *data_ov002_02110d58[3];
extern void *data_ov002_02110d64[3];
extern void *data_ov002_02110d70[3];
extern void *data_ov002_02110d7c[3];
extern void *data_ov002_02110d88[3];
extern void *data_ov002_02110d94[3];
extern void *data_ov002_02110da0[3];

/* An 8-byte pointer-to-member constant, copied as a unit. Under C++ a
   struct of two scalar fields is assigned field by field, which reorders
   the loads; an array member keeps the retail schedule. */
struct PmfWords {
    int w[2];
};

/* The state tables, in address order. */
extern PmfWords data_ov002_02110e24[2];
extern PmfWords data_ov002_02110e34[3];
extern PmfWords data_ov002_02110e4c[3];
extern PmfWords data_ov002_02110e64[3];
extern PmfWords data_ov002_02110e7c[4];
extern PmfWords data_ov002_02110e9c[4];
extern PmfWords data_ov002_02110ebc[6];
extern PmfWords data_ov002_02110eec[9];
extern PmfWords data_ov002_02110f34[13];
extern PmfWords data_ov002_02110f9c[22];

/* Their constants, in the order this initializer copies them. */
extern PmfWords data_ov002_0210b718;
extern PmfWords data_ov002_0210b690;
extern PmfWords data_ov002_0210b6a0;
extern PmfWords data_ov002_0210b850;
extern PmfWords data_ov002_0210b7f0;
extern PmfWords data_ov002_0210b898;
extern PmfWords data_ov002_0210b688;
extern PmfWords data_ov002_0210b698;
extern PmfWords data_ov002_0210b830;
extern PmfWords data_ov002_0210b880;
extern PmfWords data_ov002_0210b8a0;
extern PmfWords data_ov002_0210b840;
extern PmfWords data_ov002_0210b860;
extern PmfWords data_ov002_0210b8b8;
extern PmfWords data_ov002_0210b660;
extern PmfWords data_ov002_0210b858;
extern PmfWords data_ov002_0210b670;
extern PmfWords data_ov002_0210b7e0;
extern PmfWords data_ov002_0210b828;
extern PmfWords data_ov002_0210b8a8;
extern PmfWords data_ov002_0210b808;
extern PmfWords data_ov002_0210b838;
extern PmfWords data_ov002_0210b7f8;
extern PmfWords data_ov002_0210b6b8;
extern PmfWords data_ov002_0210b7e8;
extern PmfWords data_ov002_0210b680;
extern PmfWords data_ov002_0210b720;
extern PmfWords data_ov002_0210b6c8;
extern PmfWords data_ov002_0210b6d0;
extern PmfWords data_ov002_0210b650;
extern PmfWords data_ov002_0210b7c8;
extern PmfWords data_ov002_0210b818;
extern PmfWords data_ov002_0210b820;
extern PmfWords data_ov002_0210b7c0;
extern PmfWords data_ov002_0210b7b8;
extern PmfWords data_ov002_0210b668;
extern PmfWords data_ov002_0210b7a8;
extern PmfWords data_ov002_0210b848;
extern PmfWords data_ov002_0210b700;
extern PmfWords data_ov002_0210b788;
extern PmfWords data_ov002_0210b780;
extern PmfWords data_ov002_0210b778;
extern PmfWords data_ov002_0210b758;
extern PmfWords data_ov002_0210b710;
extern PmfWords data_ov002_0210b748;
extern PmfWords data_ov002_0210b740;
extern PmfWords data_ov002_0210b738;
extern PmfWords data_ov002_0210b730;
extern PmfWords data_ov002_0210b8b0;
extern PmfWords data_ov002_0210b728;
extern PmfWords data_ov002_0210b890;
extern PmfWords data_ov002_0210b810;
extern PmfWords data_ov002_0210b6d8;
extern PmfWords data_ov002_0210b800;
extern PmfWords data_ov002_0210b7d8;
extern PmfWords data_ov002_0210b7d0;
extern PmfWords data_ov002_0210b6c0;
extern PmfWords data_ov002_0210b6b0;
extern PmfWords data_ov002_0210b7b0;
extern PmfWords data_ov002_0210b7a0;
extern PmfWords data_ov002_0210b798;
extern PmfWords data_ov002_0210b770;
extern PmfWords data_ov002_0210b760;
extern PmfWords data_ov002_0210b790;
extern PmfWords data_ov002_0210b878;
extern PmfWords data_ov002_0210b870;
extern PmfWords data_ov002_0210b6f8;
extern PmfWords data_ov002_0210b6f0;
extern PmfWords data_ov002_0210b6e0;

extern "C" void __sinit_ov002_02107370(void) {
    func_02017acc(&data_ov002_02110b50, ASSET_PEACH_LETTER_US_BMD);
    func_020731dc(&data_ov002_02110b50, (void *)func_02017ab4, data_ov002_02110dc4);
    func_02017acc(&data_ov002_02110b98, ASSET_PEACH_HIGH_BMD);
    func_020731dc(&data_ov002_02110b98, (void *)func_02017ab4, data_ov002_02110c2c);
    func_02017acc(&data_ov002_02110c18, ASSET_PEACH_LOW_BMD);
    func_020731dc(&data_ov002_02110c18, (void *)func_02017ab4, data_ov002_02110c38);
    data_ov002_02110bf8.Construct(ASSET_PEACH_OP_WAIT_BCA);
    func_020731dc(&data_ov002_02110bf8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_02110c44);
    data_ov002_02110c10.Construct(ASSET_PEACH_ED_ENTER_WAIT_BCA);
    func_020731dc(&data_ov002_02110c10, (void *)SharedFilePtr_Destruct_Anim, data_ov002_02110c50);
    data_ov002_02110bd0.Construct(ASSET_PEACH_ED_LANDING_BCA);
    func_020731dc(&data_ov002_02110bd0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_02110c5c);
    data_ov002_02110b28.Construct(ASSET_PEACH_ED_WAIT_BCA);
    func_020731dc(&data_ov002_02110b28, (void *)SharedFilePtr_Destruct_Anim, data_ov002_02110c68);
    data_ov002_02110bf0.Construct(ASSET_PEACH_ED_LOOK_ABOUT_BCA);
    func_020731dc(&data_ov002_02110bf0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_02110c74);
    data_ov002_02110b68.Construct(ASSET_PEACH_ED_NOTICE_MARIO_BCA);
    func_020731dc(&data_ov002_02110b68, (void *)SharedFilePtr_Destruct_Anim, data_ov002_02110c80);
    data_ov002_02110ba0.Construct(ASSET_PEACH_ED_NOTICE_YOSHI_BCA);
    func_020731dc(&data_ov002_02110ba0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_02110c8c);
    data_ov002_02110bb0.Construct(ASSET_PEACH_ED_LOOKUP_BCA);
    func_020731dc(&data_ov002_02110bb0, (void *)SharedFilePtr_Destruct_Anim, data_ov002_02110c98);
    data_ov002_02110b48.Construct(ASSET_PEACH_ED_LOOKUP_TO_WAIT_BCA);
    func_020731dc(&data_ov002_02110b48, (void *)SharedFilePtr_Destruct_Anim, data_ov002_02110ca4);
    data_ov002_02110b30.Construct(ASSET_PEACH_ED_STEP_UP_BCA);
    func_020731dc(&data_ov002_02110b30, (void *)SharedFilePtr_Destruct_Anim, data_ov002_02110cb0);
    data_ov002_02110be8.Construct(ASSET_PEACH_ED_KISS_BCA);
    func_020731dc(&data_ov002_02110be8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_02110cbc);
    data_ov002_02110c08.Construct(ASSET_PEACH_ED_TURN_BCA);
    func_020731dc(&data_ov002_02110c08, (void *)SharedFilePtr_Destruct_Anim, data_ov002_02110cc8);
    data_ov002_02110c00.Construct(ASSET_PEACH_ED_WALK_BCA);
    func_020731dc(&data_ov002_02110c00, (void *)SharedFilePtr_Destruct_Anim, data_ov002_02110cd4);
    data_ov002_02110bd8.Construct(ASSET_PEACH_ED_WAVE_BCA);
    func_020731dc(&data_ov002_02110bd8, (void *)SharedFilePtr_Destruct_Anim, data_ov002_02110ce0);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110b60, ASSET_PEACH_ED_ENTER_WAIT_BTP);
    func_020731dc(&data_ov002_02110b60, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_02110cec);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110be0, ASSET_PEACH_ED_KISS_BTP);
    func_020731dc(&data_ov002_02110be0, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_02110cf8);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110b90, ASSET_PEACH_ED_LANDING_BTP);
    func_020731dc(&data_ov002_02110b90, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_02110d04);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110bc0, ASSET_PEACH_ED_LOOKUP_BTP);
    func_020731dc(&data_ov002_02110bc0, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_02110d10);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110b80, ASSET_PEACH_ED_LOOKUP_TO_WAIT_BTP);
    func_020731dc(&data_ov002_02110b80, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_02110d1c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110b88, ASSET_PEACH_ED_LOOK_ABOUT_BTP);
    func_020731dc(&data_ov002_02110b88, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_02110dd0);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110b38, ASSET_PEACH_ED_NOTICE_MARIO_BTP);
    func_020731dc(&data_ov002_02110b38, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_02110d28);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110ba8, ASSET_PEACH_ED_NOTICE_YOSHI_BTP);
    func_020731dc(&data_ov002_02110ba8, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_02110d34);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110b20, ASSET_PEACH_ED_STEP_UP_BTP);
    func_020731dc(&data_ov002_02110b20, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_02110d40);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110b40, ASSET_PEACH_ED_TURN_BTP);
    func_020731dc(&data_ov002_02110b40, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_02110d4c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110b58, ASSET_PEACH_ED_WAIT_BTP);
    func_020731dc(&data_ov002_02110b58, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_02110d58);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110bb8, ASSET_PEACH_ED_WAVE_BTP);
    func_020731dc(&data_ov002_02110bb8, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_02110d64);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110b10, ASSET_PEACH_OP_WINK_BTP);
    func_020731dc(&data_ov002_02110b10, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_02110d70);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110bc8, ASSET_PEACH_WAIT_BTP);
    func_020731dc(&data_ov002_02110bc8, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_02110d7c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110b18, ASSET_PEACH_WALK_BTP);
    func_020731dc(&data_ov002_02110b18, (void *)SharedFilePtr_Destruct_TexSeq, data_ov002_02110d88);
    func_02017acc(&data_ov002_02110b70, ASSET_OBJ_DOKAN_BMD);
    func_020731dc(&data_ov002_02110b70, (void *)func_02017ab4, data_ov002_02110d94);
    func_02017acc(&data_ov002_02110b78, ASSET_OBJ_KUMO_BMD);
    func_020731dc(&data_ov002_02110b78, (void *)func_02017ab4, data_ov002_02110da0);

    data_ov002_02110f9c[0] = data_ov002_0210b718;
    data_ov002_02110f9c[1] = data_ov002_0210b690;
    data_ov002_02110f9c[2] = data_ov002_0210b6a0;
    data_ov002_02110f9c[3] = data_ov002_0210b850;
    data_ov002_02110f9c[4] = data_ov002_0210b7f0;
    data_ov002_02110f9c[5] = data_ov002_0210b898;
    data_ov002_02110f9c[6] = data_ov002_0210b688;
    data_ov002_02110f9c[7] = data_ov002_0210b698;
    data_ov002_02110f9c[8] = data_ov002_0210b830;
    data_ov002_02110f9c[9] = data_ov002_0210b880;
    data_ov002_02110f9c[10] = data_ov002_0210b8a0;
    data_ov002_02110f9c[11] = data_ov002_0210b840;
    data_ov002_02110f9c[12] = data_ov002_0210b860;
    data_ov002_02110f9c[13] = data_ov002_0210b8b8;
    data_ov002_02110f9c[14] = data_ov002_0210b660;
    data_ov002_02110f9c[15] = data_ov002_0210b858;
    data_ov002_02110f9c[16] = data_ov002_0210b670;
    data_ov002_02110f9c[17] = data_ov002_0210b7e0;
    data_ov002_02110f9c[18] = data_ov002_0210b828;
    data_ov002_02110f9c[19] = data_ov002_0210b8a8;
    data_ov002_02110f9c[20] = data_ov002_0210b808;
    data_ov002_02110f9c[21] = data_ov002_0210b838;

    data_ov002_02110f34[0] = data_ov002_0210b7f8;
    data_ov002_02110f34[1] = data_ov002_0210b6b8;
    data_ov002_02110f34[2] = data_ov002_0210b7e8;
    data_ov002_02110f34[3] = data_ov002_0210b680;
    data_ov002_02110f34[4] = data_ov002_0210b720;
    data_ov002_02110f34[5] = data_ov002_0210b6c8;
    data_ov002_02110f34[6] = data_ov002_0210b6d0;
    data_ov002_02110f34[7] = data_ov002_0210b650;
    data_ov002_02110f34[8] = data_ov002_0210b7c8;
    data_ov002_02110f34[9] = data_ov002_0210b818;
    data_ov002_02110f34[10] = data_ov002_0210b820;
    data_ov002_02110f34[11] = data_ov002_0210b7c0;
    data_ov002_02110f34[12] = data_ov002_0210b7b8;

    data_ov002_02110e7c[0] = data_ov002_0210b668;
    data_ov002_02110e7c[1] = data_ov002_0210b7a8;
    data_ov002_02110e7c[2] = data_ov002_0210b848;
    data_ov002_02110e7c[3] = data_ov002_0210b700;

    data_ov002_02110e64[0] = data_ov002_0210b788;
    data_ov002_02110e64[1] = data_ov002_0210b780;
    data_ov002_02110e64[2] = data_ov002_0210b778;

    data_ov002_02110eec[0] = data_ov002_0210b758;
    data_ov002_02110eec[1] = data_ov002_0210b710;
    data_ov002_02110eec[2] = data_ov002_0210b748;
    data_ov002_02110eec[3] = data_ov002_0210b740;
    data_ov002_02110eec[4] = data_ov002_0210b738;
    data_ov002_02110eec[5] = data_ov002_0210b730;
    data_ov002_02110eec[6] = data_ov002_0210b8b0;
    data_ov002_02110eec[7] = data_ov002_0210b728;
    data_ov002_02110eec[8] = data_ov002_0210b890;

    data_ov002_02110e34[0] = data_ov002_0210b810;
    data_ov002_02110e34[1] = data_ov002_0210b6d8;
    data_ov002_02110e34[2] = data_ov002_0210b800;

    data_ov002_02110e4c[0] = data_ov002_0210b7d8;
    data_ov002_02110e4c[1] = data_ov002_0210b7d0;
    data_ov002_02110e4c[2] = data_ov002_0210b6c0;

    data_ov002_02110e9c[0] = data_ov002_0210b6b0;
    data_ov002_02110e9c[1] = data_ov002_0210b7b0;
    data_ov002_02110e9c[2] = data_ov002_0210b7a0;
    data_ov002_02110e9c[3] = data_ov002_0210b798;

    data_ov002_02110e24[0] = data_ov002_0210b770;
    data_ov002_02110e24[1] = data_ov002_0210b760;

    data_ov002_02110ebc[0] = data_ov002_0210b790;
    data_ov002_02110ebc[1] = data_ov002_0210b878;
    data_ov002_02110ebc[2] = data_ov002_0210b870;
    data_ov002_02110ebc[3] = data_ov002_0210b6f8;
    data_ov002_02110ebc[4] = data_ov002_0210b6f0;
    data_ov002_02110ebc[5] = data_ov002_0210b6e0;
}

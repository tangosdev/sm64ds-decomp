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

extern int func_02017acc(void*, int);
extern void func_020731dc(void*, void*, void**);
extern int _ZN13SharedFilePtr9ConstructEj(void*, int);
extern int SharedFilePtr_Construct_TexSeq(void*, int);
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Anim(void);
extern void SharedFilePtr_Destruct_TexSeq(void);

extern void* data_ov002_02110dc4;
extern void* data_ov002_02110c2c;
extern void* data_ov002_02110c38;
extern void* data_ov002_02110c44;
extern void* data_ov002_02110c50;
extern void* data_ov002_02110c5c;
extern void* data_ov002_02110c68;
extern void* data_ov002_02110c74;
extern void* data_ov002_02110c80;
extern void* data_ov002_02110c8c;
extern void* data_ov002_02110c98;
extern void* data_ov002_02110ca4;
extern void* data_ov002_02110cb0;
extern void* data_ov002_02110cbc;
extern void* data_ov002_02110cc8;
extern void* data_ov002_02110cd4;
extern void* data_ov002_02110ce0;
extern void* data_ov002_02110cec;
extern void* data_ov002_02110cf8;
extern void* data_ov002_02110d04;
extern void* data_ov002_02110d10;
extern void* data_ov002_02110d1c;
extern void* data_ov002_02110dd0;
extern void* data_ov002_02110d28;
extern void* data_ov002_02110d34;
extern void* data_ov002_02110d40;
extern void* data_ov002_02110d4c;
extern void* data_ov002_02110d58;
extern void* data_ov002_02110d64;
extern void* data_ov002_02110d70;
extern void* data_ov002_02110d7c;
extern void* data_ov002_02110d88;
extern void* data_ov002_02110d94;
extern void* data_ov002_02110da0;

extern void* data_ov002_02110b50;
extern void* data_ov002_02110b98;
extern void* data_ov002_02110c18;
extern void* data_ov002_02110bf8;
extern void* data_ov002_02110c10;
extern void* data_ov002_02110bd0;
extern void* data_ov002_02110b28;
extern void* data_ov002_02110bf0;
extern void* data_ov002_02110b68;
extern void* data_ov002_02110ba0;
extern void* data_ov002_02110bb0;
extern void* data_ov002_02110b48;
extern void* data_ov002_02110b30;
extern void* data_ov002_02110be8;
extern void* data_ov002_02110c08;
extern void* data_ov002_02110c00;
extern void* data_ov002_02110bd8;
extern void* data_ov002_02110b60;
extern void* data_ov002_02110be0;
extern void* data_ov002_02110b90;
extern void* data_ov002_02110bc0;
extern void* data_ov002_02110b80;
extern void* data_ov002_02110b88;
extern void* data_ov002_02110b38;
extern void* data_ov002_02110ba8;
extern void* data_ov002_02110b20;
extern void* data_ov002_02110b40;
extern void* data_ov002_02110b58;
extern void* data_ov002_02110bb8;
extern void* data_ov002_02110b10;
extern void* data_ov002_02110bc8;
extern void* data_ov002_02110b18;
extern void* data_ov002_02110b70;
extern void* data_ov002_02110b78;

typedef struct { int a, b; } S8;
typedef struct { S8 x, y; } S16;

extern S16 data_ov002_02110f9c[11];
extern S16 data_ov002_02110f34[7];
extern S16 data_ov002_02110e7c[2];
extern S16 data_ov002_02110e64[2];
extern S16 data_ov002_02110eec[5];
extern S16 data_ov002_02110e34[2];
extern S16 data_ov002_02110e4c[2];
extern S16 data_ov002_02110e9c[2];
extern S16 data_ov002_02110e24[1];
extern S16 data_ov002_02110ebc[3];

extern S8 data_ov002_0210b718;
extern S8 data_ov002_0210b690;
extern S8 data_ov002_0210b6a0;
extern S8 data_ov002_0210b850;
extern S8 data_ov002_0210b7f0;
extern S8 data_ov002_0210b898;
extern S8 data_ov002_0210b688;
extern S8 data_ov002_0210b698;
extern S8 data_ov002_0210b830;
extern S8 data_ov002_0210b880;
extern S8 data_ov002_0210b8a0;
extern S8 data_ov002_0210b840;
extern S8 data_ov002_0210b860;
extern S8 data_ov002_0210b8b8;
extern S8 data_ov002_0210b660;
extern S8 data_ov002_0210b858;
extern S8 data_ov002_0210b670;
extern S8 data_ov002_0210b7e0;
extern S8 data_ov002_0210b828;
extern S8 data_ov002_0210b8a8;
extern S8 data_ov002_0210b808;
extern S8 data_ov002_0210b838;
extern S8 data_ov002_0210b7f8;
extern S8 data_ov002_0210b6b8;
extern S8 data_ov002_0210b7e8;
extern S8 data_ov002_0210b680;
extern S8 data_ov002_0210b720;
extern S8 data_ov002_0210b6c8;
extern S8 data_ov002_0210b6d0;
extern S8 data_ov002_0210b650;
extern S8 data_ov002_0210b7c8;
extern S8 data_ov002_0210b818;
extern S8 data_ov002_0210b820;
extern S8 data_ov002_0210b7c0;
extern S8 data_ov002_0210b7b8;
extern S8 data_ov002_0210b668;
extern S8 data_ov002_0210b7a8;
extern S8 data_ov002_0210b848;
extern S8 data_ov002_0210b700;
extern S8 data_ov002_0210b788;
extern S8 data_ov002_0210b780;
extern S8 data_ov002_0210b778;
extern S8 data_ov002_0210b758;
extern S8 data_ov002_0210b710;
extern S8 data_ov002_0210b748;
extern S8 data_ov002_0210b740;
extern S8 data_ov002_0210b738;
extern S8 data_ov002_0210b730;
extern S8 data_ov002_0210b8b0;
extern S8 data_ov002_0210b728;
extern S8 data_ov002_0210b890;
extern S8 data_ov002_0210b810;
extern S8 data_ov002_0210b6d8;
extern S8 data_ov002_0210b800;
extern S8 data_ov002_0210b7d8;
extern S8 data_ov002_0210b7d0;
extern S8 data_ov002_0210b6c0;
extern S8 data_ov002_0210b6b0;
extern S8 data_ov002_0210b7b0;
extern S8 data_ov002_0210b7a0;
extern S8 data_ov002_0210b798;
extern S8 data_ov002_0210b770;
extern S8 data_ov002_0210b760;
extern S8 data_ov002_0210b790;
extern S8 data_ov002_0210b878;
extern S8 data_ov002_0210b870;
extern S8 data_ov002_0210b6f8;
extern S8 data_ov002_0210b6f0;
extern S8 data_ov002_0210b6e0;

void __sinit_ov002_02107370(void)
{
    func_02017acc(&data_ov002_02110b50, ASSET_PEACH_LETTER_US_BMD);
    func_020731dc(&data_ov002_02110b50, (void*)&func_02017ab4, (void**)&data_ov002_02110dc4);
    func_02017acc(&data_ov002_02110b98, ASSET_PEACH_HIGH_BMD);
    func_020731dc(&data_ov002_02110b98, (void*)&func_02017ab4, (void**)&data_ov002_02110c2c);
    func_02017acc(&data_ov002_02110c18, ASSET_PEACH_LOW_BMD);
    func_020731dc(&data_ov002_02110c18, (void*)&func_02017ab4, (void**)&data_ov002_02110c38);
    _ZN13SharedFilePtr9ConstructEj(&data_ov002_02110bf8, ASSET_PEACH_OP_WAIT_BCA);
    func_020731dc(&data_ov002_02110bf8, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov002_02110c44);
    _ZN13SharedFilePtr9ConstructEj(&data_ov002_02110c10, ASSET_PEACH_ED_ENTER_WAIT_BCA);
    func_020731dc(&data_ov002_02110c10, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov002_02110c50);
    _ZN13SharedFilePtr9ConstructEj(&data_ov002_02110bd0, ASSET_PEACH_ED_LANDING_BCA);
    func_020731dc(&data_ov002_02110bd0, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov002_02110c5c);
    _ZN13SharedFilePtr9ConstructEj(&data_ov002_02110b28, ASSET_PEACH_ED_WAIT_BCA);
    func_020731dc(&data_ov002_02110b28, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov002_02110c68);
    _ZN13SharedFilePtr9ConstructEj(&data_ov002_02110bf0, ASSET_PEACH_ED_LOOK_ABOUT_BCA);
    func_020731dc(&data_ov002_02110bf0, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov002_02110c74);
    _ZN13SharedFilePtr9ConstructEj(&data_ov002_02110b68, ASSET_PEACH_ED_NOTICE_MARIO_BCA);
    func_020731dc(&data_ov002_02110b68, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov002_02110c80);
    _ZN13SharedFilePtr9ConstructEj(&data_ov002_02110ba0, ASSET_PEACH_ED_NOTICE_YOSHI_BCA);
    func_020731dc(&data_ov002_02110ba0, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov002_02110c8c);
    _ZN13SharedFilePtr9ConstructEj(&data_ov002_02110bb0, ASSET_PEACH_ED_LOOKUP_BCA);
    func_020731dc(&data_ov002_02110bb0, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov002_02110c98);
    _ZN13SharedFilePtr9ConstructEj(&data_ov002_02110b48, ASSET_PEACH_ED_LOOKUP_TO_WAIT_BCA);
    func_020731dc(&data_ov002_02110b48, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov002_02110ca4);
    _ZN13SharedFilePtr9ConstructEj(&data_ov002_02110b30, ASSET_PEACH_ED_STEP_UP_BCA);
    func_020731dc(&data_ov002_02110b30, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov002_02110cb0);
    _ZN13SharedFilePtr9ConstructEj(&data_ov002_02110be8, ASSET_PEACH_ED_KISS_BCA);
    func_020731dc(&data_ov002_02110be8, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov002_02110cbc);
    _ZN13SharedFilePtr9ConstructEj(&data_ov002_02110c08, ASSET_PEACH_ED_TURN_BCA);
    func_020731dc(&data_ov002_02110c08, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov002_02110cc8);
    _ZN13SharedFilePtr9ConstructEj(&data_ov002_02110c00, ASSET_PEACH_ED_WALK_BCA);
    func_020731dc(&data_ov002_02110c00, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov002_02110cd4);
    _ZN13SharedFilePtr9ConstructEj(&data_ov002_02110bd8, ASSET_PEACH_ED_WAVE_BCA);
    func_020731dc(&data_ov002_02110bd8, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov002_02110ce0);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110b60, ASSET_PEACH_ED_ENTER_WAIT_BTP);
    func_020731dc(&data_ov002_02110b60, (void*)&SharedFilePtr_Destruct_TexSeq, (void**)&data_ov002_02110cec);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110be0, ASSET_PEACH_ED_KISS_BTP);
    func_020731dc(&data_ov002_02110be0, (void*)&SharedFilePtr_Destruct_TexSeq, (void**)&data_ov002_02110cf8);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110b90, ASSET_PEACH_ED_LANDING_BTP);
    func_020731dc(&data_ov002_02110b90, (void*)&SharedFilePtr_Destruct_TexSeq, (void**)&data_ov002_02110d04);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110bc0, ASSET_PEACH_ED_LOOKUP_BTP);
    func_020731dc(&data_ov002_02110bc0, (void*)&SharedFilePtr_Destruct_TexSeq, (void**)&data_ov002_02110d10);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110b80, ASSET_PEACH_ED_LOOKUP_TO_WAIT_BTP);
    func_020731dc(&data_ov002_02110b80, (void*)&SharedFilePtr_Destruct_TexSeq, (void**)&data_ov002_02110d1c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110b88, ASSET_PEACH_ED_LOOK_ABOUT_BTP);
    func_020731dc(&data_ov002_02110b88, (void*)&SharedFilePtr_Destruct_TexSeq, (void**)&data_ov002_02110dd0);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110b38, ASSET_PEACH_ED_NOTICE_MARIO_BTP);
    func_020731dc(&data_ov002_02110b38, (void*)&SharedFilePtr_Destruct_TexSeq, (void**)&data_ov002_02110d28);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110ba8, ASSET_PEACH_ED_NOTICE_YOSHI_BTP);
    func_020731dc(&data_ov002_02110ba8, (void*)&SharedFilePtr_Destruct_TexSeq, (void**)&data_ov002_02110d34);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110b20, ASSET_PEACH_ED_STEP_UP_BTP);
    func_020731dc(&data_ov002_02110b20, (void*)&SharedFilePtr_Destruct_TexSeq, (void**)&data_ov002_02110d40);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110b40, ASSET_PEACH_ED_TURN_BTP);
    func_020731dc(&data_ov002_02110b40, (void*)&SharedFilePtr_Destruct_TexSeq, (void**)&data_ov002_02110d4c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110b58, ASSET_PEACH_ED_WAIT_BTP);
    func_020731dc(&data_ov002_02110b58, (void*)&SharedFilePtr_Destruct_TexSeq, (void**)&data_ov002_02110d58);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110bb8, ASSET_PEACH_ED_WAVE_BTP);
    func_020731dc(&data_ov002_02110bb8, (void*)&SharedFilePtr_Destruct_TexSeq, (void**)&data_ov002_02110d64);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110b10, ASSET_PEACH_OP_WINK_BTP);
    func_020731dc(&data_ov002_02110b10, (void*)&SharedFilePtr_Destruct_TexSeq, (void**)&data_ov002_02110d70);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110bc8, ASSET_PEACH_WAIT_BTP);
    func_020731dc(&data_ov002_02110bc8, (void*)&SharedFilePtr_Destruct_TexSeq, (void**)&data_ov002_02110d7c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_02110b18, ASSET_PEACH_WALK_BTP);
    func_020731dc(&data_ov002_02110b18, (void*)&SharedFilePtr_Destruct_TexSeq, (void**)&data_ov002_02110d88);
    func_02017acc(&data_ov002_02110b70, ASSET_OBJ_DOKAN_BMD);
    func_020731dc(&data_ov002_02110b70, (void*)&func_02017ab4, (void**)&data_ov002_02110d94);
    func_02017acc(&data_ov002_02110b78, ASSET_OBJ_KUMO_BMD);
    func_020731dc(&data_ov002_02110b78, (void*)&func_02017ab4, (void**)&data_ov002_02110da0);

    data_ov002_02110f9c[0].x = data_ov002_0210b718;
    data_ov002_02110f9c[0].y = data_ov002_0210b690;
    data_ov002_02110f9c[1].x = data_ov002_0210b6a0;
    data_ov002_02110f9c[1].y = data_ov002_0210b850;
    data_ov002_02110f9c[2].x = data_ov002_0210b7f0;
    data_ov002_02110f9c[2].y = data_ov002_0210b898;
    data_ov002_02110f9c[3].x = data_ov002_0210b688;
    data_ov002_02110f9c[3].y = data_ov002_0210b698;
    data_ov002_02110f9c[4].x = data_ov002_0210b830;
    data_ov002_02110f9c[4].y = data_ov002_0210b880;
    data_ov002_02110f9c[5].x = data_ov002_0210b8a0;
    data_ov002_02110f9c[5].y = data_ov002_0210b840;
    data_ov002_02110f9c[6].x = data_ov002_0210b860;
    data_ov002_02110f9c[6].y = data_ov002_0210b8b8;
    data_ov002_02110f9c[7].x = data_ov002_0210b660;
    data_ov002_02110f9c[7].y = data_ov002_0210b858;
    data_ov002_02110f9c[8].x = data_ov002_0210b670;
    data_ov002_02110f9c[8].y = data_ov002_0210b7e0;
    data_ov002_02110f9c[9].x = data_ov002_0210b828;
    data_ov002_02110f9c[9].y = data_ov002_0210b8a8;
    data_ov002_02110f9c[10].x = data_ov002_0210b808;
    data_ov002_02110f9c[10].y = data_ov002_0210b838;

    data_ov002_02110f34[0].x = data_ov002_0210b7f8;
    data_ov002_02110f34[0].y = data_ov002_0210b6b8;
    data_ov002_02110f34[1].x = data_ov002_0210b7e8;
    data_ov002_02110f34[1].y = data_ov002_0210b680;
    data_ov002_02110f34[2].x = data_ov002_0210b720;
    data_ov002_02110f34[2].y = data_ov002_0210b6c8;
    data_ov002_02110f34[3].x = data_ov002_0210b6d0;
    data_ov002_02110f34[3].y = data_ov002_0210b650;
    data_ov002_02110f34[4].x = data_ov002_0210b7c8;
    data_ov002_02110f34[4].y = data_ov002_0210b818;
    data_ov002_02110f34[5].x = data_ov002_0210b820;
    data_ov002_02110f34[5].y = data_ov002_0210b7c0;
    data_ov002_02110f34[6].x = data_ov002_0210b7b8;

    data_ov002_02110e7c[0].x = data_ov002_0210b668;
    data_ov002_02110e7c[0].y = data_ov002_0210b7a8;
    data_ov002_02110e7c[1].x = data_ov002_0210b848;
    data_ov002_02110e7c[1].y = data_ov002_0210b700;

    data_ov002_02110e64[0].x = data_ov002_0210b788;
    data_ov002_02110e64[0].y = data_ov002_0210b780;
    data_ov002_02110e64[1].x = data_ov002_0210b778;

    data_ov002_02110eec[0].x = data_ov002_0210b758;
    data_ov002_02110eec[0].y = data_ov002_0210b710;
    data_ov002_02110eec[1].x = data_ov002_0210b748;
    data_ov002_02110eec[1].y = data_ov002_0210b740;
    data_ov002_02110eec[2].x = data_ov002_0210b738;
    data_ov002_02110eec[2].y = data_ov002_0210b730;
    data_ov002_02110eec[3].x = data_ov002_0210b8b0;
    data_ov002_02110eec[3].y = data_ov002_0210b728;
    data_ov002_02110eec[4].x = data_ov002_0210b890;

    data_ov002_02110e34[0].x = data_ov002_0210b810;
    data_ov002_02110e34[0].y = data_ov002_0210b6d8;
    data_ov002_02110e34[1].x = data_ov002_0210b800;

    data_ov002_02110e4c[0].x = data_ov002_0210b7d8;
    data_ov002_02110e4c[0].y = data_ov002_0210b7d0;
    data_ov002_02110e4c[1].x = data_ov002_0210b6c0;

    data_ov002_02110e9c[0].x = data_ov002_0210b6b0;
    data_ov002_02110e9c[0].y = data_ov002_0210b7b0;
    data_ov002_02110e9c[1].x = data_ov002_0210b7a0;
    data_ov002_02110e9c[1].y = data_ov002_0210b798;

    data_ov002_02110e24[0].x = data_ov002_0210b770;
    data_ov002_02110e24[0].y = data_ov002_0210b760;

    data_ov002_02110ebc[0].x = data_ov002_0210b790;
    data_ov002_02110ebc[0].y = data_ov002_0210b878;
    data_ov002_02110ebc[1].x = data_ov002_0210b870;
    data_ov002_02110ebc[1].y = data_ov002_0210b6f8;
    data_ov002_02110ebc[2].x = data_ov002_0210b6f0;
    data_ov002_02110ebc[2].y = data_ov002_0210b6e0;
}

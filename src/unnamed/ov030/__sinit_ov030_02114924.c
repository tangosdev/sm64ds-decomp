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

extern int func_02017acc(void*, int);
extern void func_020731dc(void*, void*, void**);
extern int _ZN13SharedFilePtr9ConstructEj(void*, int);
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Anim(void);
extern void *_ZN7Vector3D1Ev(void *object);

extern int data_ov030_02115d00[];
extern unsigned int data_ov030_02115d28[];
extern int gMonkeyHoldAnimationFile[];
extern unsigned int data_ov030_02115d34[];
extern int gMonkeyJumpStAnimationFile[];
extern unsigned int data_ov030_02115d40[];
extern int gMonkeyJumpEdAnimationFile[];
extern unsigned int data_ov030_02115d64[];
extern int gMonkeyRunStAnimationFile[];
extern unsigned int data_ov030_02115d70[];
extern int gMonkeyRunAnimationFile[];
extern unsigned int data_ov030_02115d88[];
extern int gMonkeyRunEdAnimationFile[];
extern unsigned int data_ov030_02115d94[];
extern int data_ov030_02115cf0[];
extern unsigned int data_ov030_02115dac[];
extern int gMonkeyWait2AnimationFile[];
extern unsigned int data_ov030_02115db8[];
extern int gMonkeyWait3AnimationFile[];
extern unsigned int data_ov030_02115dc4[];
extern int gMonkeyWait4AnimationFile[];
extern unsigned int data_ov030_02115dd0[];

typedef struct { int a, b; } S8;
typedef struct { S8 lo, hi; } S16;

extern S8 data_ov030_02115b68;
extern S8 data_ov030_02115b60;
extern S8 data_ov030_02115af0;
extern S8 data_ov030_02115b50;
extern S8 data_ov030_02115b00;
extern S8 data_ov030_02115b40;
extern S8 data_ov030_02115b48;
extern S8 data_ov030_02115b18;
extern S8 data_ov030_02115b10;
extern S8 data_ov030_02115ad8;
extern S8 data_ov030_02115b58;
extern S8 data_ov030_02115b20;
extern S8 data_ov030_02115ae0;
extern S8 data_ov030_02115b28;
extern S8 data_ov030_02115ae8;
extern S8 data_ov030_02115b30;
extern S8 data_ov030_02115ac8;
extern S8 data_ov030_02115ad0;
extern S8 data_ov030_02115b08;
extern S8 data_ov030_02115b70;
extern S8 data_ov030_02115af8;
extern S8 data_ov030_02115b38;

extern char data_ov030_02115e0c[];

extern char data_ov030_02115ddc[];
extern unsigned int data_ov030_02115d58[];
extern char data_ov030_02115de8[];
extern unsigned int data_ov030_02115d4c[];
extern char data_ov030_02115df4[];
extern unsigned int data_ov030_02115d7c[];
extern char data_ov030_02115e00[];
extern unsigned int data_ov030_02115da0[];

void __sinit_ov030_02114924(void)
{
    func_02017acc(&data_ov030_02115d00, ASSET_MONKEY_BMD);
    func_020731dc(&data_ov030_02115d00, (void*)&func_02017ab4, (void**)&data_ov030_02115d28);
    _ZN13SharedFilePtr9ConstructEj(&gMonkeyHoldAnimationFile, ASSET_MONKEY_HOLD_BCA);
    func_020731dc(&gMonkeyHoldAnimationFile, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov030_02115d34);
    _ZN13SharedFilePtr9ConstructEj(&gMonkeyJumpStAnimationFile, ASSET_MONKEY_JUMP_ST_BCA);
    func_020731dc(&gMonkeyJumpStAnimationFile, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov030_02115d40);
    _ZN13SharedFilePtr9ConstructEj(&gMonkeyJumpEdAnimationFile, ASSET_MONKEY_JUMP_ED_BCA);
    func_020731dc(&gMonkeyJumpEdAnimationFile, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov030_02115d64);
    _ZN13SharedFilePtr9ConstructEj(&gMonkeyRunStAnimationFile, ASSET_MONKEY_RUN_ST_BCA);
    func_020731dc(&gMonkeyRunStAnimationFile, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov030_02115d70);
    _ZN13SharedFilePtr9ConstructEj(&gMonkeyRunAnimationFile, ASSET_MONKEY_RUN_BCA);
    func_020731dc(&gMonkeyRunAnimationFile, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov030_02115d88);
    _ZN13SharedFilePtr9ConstructEj(&gMonkeyRunEdAnimationFile, ASSET_MONKEY_RUN_ED_BCA);
    func_020731dc(&gMonkeyRunEdAnimationFile, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov030_02115d94);
    _ZN13SharedFilePtr9ConstructEj(&data_ov030_02115cf0, ASSET_MONKEY_WAIT1_BCA);
    func_020731dc(&data_ov030_02115cf0, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov030_02115dac);
    _ZN13SharedFilePtr9ConstructEj(&gMonkeyWait2AnimationFile, ASSET_MONKEY_WAIT2_BCA);
    func_020731dc(&gMonkeyWait2AnimationFile, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov030_02115db8);
    _ZN13SharedFilePtr9ConstructEj(&gMonkeyWait3AnimationFile, ASSET_MONKEY_WAIT3_BCA);
    func_020731dc(&gMonkeyWait3AnimationFile, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov030_02115dc4);
    _ZN13SharedFilePtr9ConstructEj(&gMonkeyWait4AnimationFile, ASSET_MONKEY_WAIT4_BCA);
    func_020731dc(&gMonkeyWait4AnimationFile, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov030_02115dd0);

    ((S16 *)data_ov030_02115e0c)[0].lo = data_ov030_02115b68;
    ((S16 *)data_ov030_02115e0c)[0].hi = data_ov030_02115b60;
    ((S16 *)data_ov030_02115e0c)[1].lo = data_ov030_02115af0;
    ((S16 *)data_ov030_02115e0c)[1].hi = data_ov030_02115b50;
    ((S16 *)data_ov030_02115e0c)[2].lo = data_ov030_02115b00;
    ((S16 *)data_ov030_02115e0c)[2].hi = data_ov030_02115b40;
    ((S16 *)data_ov030_02115e0c)[3].lo = data_ov030_02115b48;
    ((S16 *)data_ov030_02115e0c)[3].hi = data_ov030_02115b18;
    ((S16 *)data_ov030_02115e0c)[4].lo = data_ov030_02115b10;
    ((S16 *)data_ov030_02115e0c)[4].hi = data_ov030_02115ad8;
    ((S16 *)data_ov030_02115e0c)[5].lo = data_ov030_02115b58;
    ((S16 *)data_ov030_02115e0c)[5].hi = data_ov030_02115b20;
    ((S16 *)data_ov030_02115e0c)[6].lo = data_ov030_02115ae0;
    ((S16 *)data_ov030_02115e0c)[6].hi = data_ov030_02115b28;
    ((S16 *)data_ov030_02115e0c)[7].lo = data_ov030_02115ae8;
    ((S16 *)data_ov030_02115e0c)[7].hi = data_ov030_02115b30;
    ((S16 *)data_ov030_02115e0c)[8].lo = data_ov030_02115ac8;
    ((S16 *)data_ov030_02115e0c)[8].hi = data_ov030_02115ad0;
    ((S16 *)data_ov030_02115e0c)[9].lo = data_ov030_02115b08;
    ((S16 *)data_ov030_02115e0c)[9].hi = data_ov030_02115b70;
    ((S16 *)data_ov030_02115e0c)[10].lo = data_ov030_02115af8;
    ((S16 *)data_ov030_02115e0c)[10].hi = data_ov030_02115b38;

    *(unsigned int*)&data_ov030_02115ddc = 0x1c000;
    *(unsigned int*)((char*)&data_ov030_02115ddc + 4) = (unsigned int)-0x42000;
    *(unsigned int*)((char*)&data_ov030_02115ddc + 8) = 0x18000;
    func_020731dc(&data_ov030_02115ddc, (void*)&_ZN7Vector3D1Ev, (void**)&data_ov030_02115d58);
    {
        unsigned int *p = (unsigned int*)&data_ov030_02115de8;
        p[0] = 0x1c000;
        p[1] = (unsigned int)-0x28000;
        p[2] = 0x28000;
        func_020731dc(p, (void*)&_ZN7Vector3D1Ev, (void**)&data_ov030_02115d4c);
    }
    {
        unsigned int *p = (unsigned int*)&data_ov030_02115df4;
        p[0] = 0x2b000;
        p[1] = (unsigned int)-0x42000;
        p[2] = 0x18000;
        func_020731dc(p, (void*)&_ZN7Vector3D1Ev, (void**)&data_ov030_02115d7c);
    }
    {
        unsigned int *p = (unsigned int*)&data_ov030_02115e00;
        p[0] = 0x30000;
        p[1] = (unsigned int)-0x1e000;
        p[2] = 0x1e000;
        func_020731dc(p, (void*)&_ZN7Vector3D1Ev, (void**)&data_ov030_02115da0);
    }
}

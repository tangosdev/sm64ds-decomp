/* Runtime asset handles resolved through the overlay 0 file table. */
enum sinit_ov085_0212f3a0_AssetId {
    ASSET_PEACH_JUMP_BCA = 0x03f0,
    ASSET_PEACH_JUMP_END_BCA = 0x03f1,
    ASSET_PEACH_JUMP_LOOP_BCA = 0x03f2,
    ASSET_PEACH_LOW_BMD = 0x03f3,
    ASSET_PEACH_OP_WAIT_BCA = 0x03f4,
    ASSET_PEACH_RUN_BCA = 0x03f6,
    ASSET_PEACH_WAIT_BCA = 0x03f7,
    ASSET_PEACH_WALK_BCA = 0x03f9
};

extern int func_02017acc();
extern void func_020731dc();
extern int _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Anim(void);

extern char data_ov085_021304f4[];
extern char data_ov085_02130538[];
extern char data_ov085_021304d4[];
extern char data_ov085_02130544[];
extern char data_ov085_021304c4[];
extern char data_ov085_02130550[];
extern char data_ov085_021304e4[];
extern char data_ov085_021304fc[];
extern char data_ov085_021304ec[];
extern char data_ov085_02130508[];
extern char gPeachJumpEndAnimationFile[];
extern char data_ov085_02130514[];
extern char gPeachJumpLoopAnimationFile[];
extern char data_ov085_02130520[];
extern char data_ov085_021304bc[];
extern char data_ov085_0213052c[];

struct P2 { int a, b; };
extern struct P2 data_ov085_0213055c[];
extern struct P2 data_ov085_0212ff7c;
extern struct P2 data_ov085_0212ff64;
extern struct P2 data_ov085_0212ff44;
extern struct P2 data_ov085_0212ff6c;
extern struct P2 data_ov085_0212ff4c;
extern struct P2 data_ov085_0212ff74;
extern struct P2 data_ov085_0212ff54;
extern struct P2 data_ov085_0212ff5c;
extern struct P2 data_ov085_0212ff3c;
extern struct P2 data_ov085_0212ff34;

void __sinit_ov085_0212f3a0(void)
{
    func_02017acc(data_ov085_021304f4, ASSET_PEACH_LOW_BMD);
    func_020731dc(data_ov085_021304f4, func_02017ab4, data_ov085_02130538);
    _ZN13SharedFilePtr9ConstructEj(data_ov085_021304d4, ASSET_PEACH_WAIT_BCA);
    func_020731dc(data_ov085_021304d4, SharedFilePtr_Destruct_Anim, data_ov085_02130544);
    _ZN13SharedFilePtr9ConstructEj(data_ov085_021304c4, ASSET_PEACH_WALK_BCA);
    func_020731dc(data_ov085_021304c4, SharedFilePtr_Destruct_Anim, data_ov085_02130550);
    _ZN13SharedFilePtr9ConstructEj(data_ov085_021304e4, ASSET_PEACH_RUN_BCA);
    func_020731dc(data_ov085_021304e4, SharedFilePtr_Destruct_Anim, data_ov085_021304fc);
    _ZN13SharedFilePtr9ConstructEj(data_ov085_021304ec, ASSET_PEACH_JUMP_BCA);
    func_020731dc(data_ov085_021304ec, SharedFilePtr_Destruct_Anim, data_ov085_02130508);
    _ZN13SharedFilePtr9ConstructEj(gPeachJumpEndAnimationFile, ASSET_PEACH_JUMP_END_BCA);
    func_020731dc(gPeachJumpEndAnimationFile, SharedFilePtr_Destruct_Anim, data_ov085_02130514);
    _ZN13SharedFilePtr9ConstructEj(gPeachJumpLoopAnimationFile, ASSET_PEACH_JUMP_LOOP_BCA);
    func_020731dc(gPeachJumpLoopAnimationFile, SharedFilePtr_Destruct_Anim, data_ov085_02130520);
    _ZN13SharedFilePtr9ConstructEj(data_ov085_021304bc, ASSET_PEACH_OP_WAIT_BCA);
    func_020731dc(data_ov085_021304bc, SharedFilePtr_Destruct_Anim, data_ov085_0213052c);

    data_ov085_0213055c[0] = data_ov085_0212ff7c;
    data_ov085_0213055c[1] = data_ov085_0212ff64;
    data_ov085_0213055c[2] = data_ov085_0212ff44;
    data_ov085_0213055c[3] = data_ov085_0212ff6c;
    data_ov085_0213055c[4] = data_ov085_0212ff4c;
    data_ov085_0213055c[5] = data_ov085_0212ff74;
    data_ov085_0213055c[6] = data_ov085_0212ff54;
    data_ov085_0213055c[7] = data_ov085_0212ff5c;
    data_ov085_0213055c[8] = data_ov085_0212ff3c;
    data_ov085_0213055c[9] = data_ov085_0212ff34;
}

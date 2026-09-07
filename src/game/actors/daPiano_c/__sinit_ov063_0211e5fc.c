#include "MadPianoResources.h"

extern void func_02017acc();
extern void func_020731dc();
extern void _ZN13SharedFilePtr9ConstructEj();
extern void func_02017b4c();
extern int func_02017ab4[];
extern int SharedFilePtr_Destruct_Clsn[];
extern int SharedFilePtr_Destruct_Anim[];
extern SharedFilePtrLayout gPianoModelFile;
extern int data_ov063_0211ef98[];
extern SharedFilePtrLayout gPianoCollisionFile;
extern int data_ov063_0211efa4[];
extern SharedFilePtrLayout gPianoAttackAnimationFile;
extern int data_ov063_0211efb0[];

struct Pair { int a, b; };
extern struct Pair data_ov063_0211ecd8;
extern struct Pair data_ov063_0211ece8;
extern struct Pair data_ov063_0211ece0;
extern struct Pair data_ov063_0211ecf0;
struct Quad { struct Pair p0, p1, p2, p3; };
extern struct Quad data_ov063_0211efbc;

void __sinit_ov063_0211e5fc(void)
{
    func_02017acc(&gPianoModelFile, MAD_PIANO_MODEL_ASSET);
    func_020731dc(&gPianoModelFile, func_02017ab4, data_ov063_0211ef98);
    func_02017b4c(&gPianoCollisionFile, MAD_PIANO_COLLISION_ASSET);
    func_020731dc(&gPianoCollisionFile, SharedFilePtr_Destruct_Clsn, data_ov063_0211efa4);
    _ZN13SharedFilePtr9ConstructEj(&gPianoAttackAnimationFile, MAD_PIANO_ATTACK_ANIMATION_ASSET);
    func_020731dc(&gPianoAttackAnimationFile, SharedFilePtr_Destruct_Anim, data_ov063_0211efb0);
    data_ov063_0211efbc.p0 = data_ov063_0211ecd8;
    data_ov063_0211efbc.p1 = data_ov063_0211ece8;
    data_ov063_0211efbc.p2 = data_ov063_0211ece0;
    data_ov063_0211efbc.p3 = data_ov063_0211ecf0;
}

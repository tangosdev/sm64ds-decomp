/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class daObjFl_Puzzle_c: 5 matched functions, 14 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef DAOBJFL_PUZZLE_C_H
#define DAOBJFL_PUZZLE_C_H
#include "dBgActor_c.h"

/* mState: index into the pointer-to-member table at 0x0211c904, which the ROM
 * builds in __sinit_ov064_0211b1d4 in this order. The scripts in the per-type
 * tables hold these values (1 mostly, 2..5 for moves) ended by -1. */
enum {
    PIECE_STATE_INIT = 0,       /* StateInit: bind to the FL_COIN manager, or destroy self */
    PIECE_STATE_WAIT = 1,       /* StateWait: idle until the step timer runs out */
    PIECE_STATE_MOVE_NEG_X = 2, /* StateMoveNegX: shake, then slide -X */
    PIECE_STATE_MOVE_POS_X = 3, /* StateMovePosX: shake, then slide +X */
    PIECE_STATE_MOVE_NEG_Z = 4, /* StateMoveNegZ: shake, then slide -Z */
    PIECE_STATE_MOVE_POS_Z = 5  /* StateMovePosZ: shake, then slide +Z */
};

/* Lethal Lava Land puzzle piece (FL_PUZZLE). Each piece plays a byte script
 * of states, one step per run of mStepTimer up to 0x18, under an FL_COIN manager.
 *
 * _ZTI16daObjFl_Puzzle_c is at ov064 0x0211bfa4 and the name bytes at
 * 0x0211bfc4 say daObjFl_Puzzle_c. The vtable at 0x0211c25c stores that
 * typeinfo. The sole base is dBgActor_c: the destructor restores
 * _ZTV10dBgActor_c, destroys the moving-mesh member at 0x124 and dExtSimpleModel_c at
 * 0x0d4, then chains to dActor_c::~dActor_c.
 */
struct daObjFl_Puzzle_c : dBgActor_c {
    /* Unique ID of the FL_COIN manager (daObjFl_Coin_c) this piece bound to in
     * PIECE_STATE_INIT. */
    u32 mCoinMgrId; /* 0x320 */
    /* This type's step script: a byte array of PIECE_STATE_* values ended by -1,
     * taken from the table at 0x0211c198 by mType. Stored as a plain word. */
    s32 mStepScript; /* 0x324 */
    u8 mStepIndex;   /* 0x328 -- next entry of the script */
    u8 pad_329[0x3];
    s32 unk_32c; /* 0x32c -- only zeroed, in this TU */
    /* Vertical offset added to mPosY for the model and collision matrices:
     * 0 on even mStepTimer values and -6 units (-0x6000) on odd ones while timer
     * values 0..0x13 of a move step run, 0 otherwise. */
    s32 mShakeOffsetY; /* 0x330 */
    u16 mStepTimer;    /* 0x334 -- ticks into the current step; counts only while the
                             manager is gone or has no live coins */
    u8 mState;         /* 0x336 -- PIECE_STATE_* */
    u8 mType;          /* 0x337 -- param1 & 0xf: which model and script */
    /* Set to 1 by func_ov064_0211929c (the collision callback registered in
     * InitResources, via the func_ov064_021192bc thunk) when the other actor's
     * ID is 0xbf (PLAYER). Never cleared here. While set, the step driver
     * writes FLCOIN_FLAG_PLAYER_TOUCHED into the manager's flags each frame. */
    u8 mHadClsn;      /* 0x338 */
    u8 mStepActive;   /* 0x339 -- 1 while a step runs; the script
                             advances only when it is 0 */
    u8 mCanSpawnCoin; /* 0x33a -- 1 until the piece first attempts its coin spawn */
    /* Inline is load-bearing. The forcing calls in src/game/actors/d_a_obj_fl_coin.cpp
     * emit D1 then D0. An out-of-line body emits D2, D0, D1. */
    virtual ~daObjFl_Puzzle_c() {}

    /* Overrides of fBase_c's resource/behavior/render slots. */
    int InitResources();
    int CleanupResources();
    int Behavior();
    int Render();

    /* State-table targets and the helpers they call. Not virtual: the table
     * at 0x0211c904 is a pointer-to-member array, not extra vtable slots. */
    void SpawnCoin();
    void StateMovePosZ();
    void StateMoveNegZ();
    void StateMovePosX();
    void StateMoveNegX();
    void StateWait();
    void StateInit();
    void ShakeThenSlide(int dx, int dz, int slideTicks);
    void AdvanceStepScript();
    void UpdateClsnMatrix();
    void UpdateModelMatrix();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjFl_Puzzle_c_size_must_be_0x33c[sizeof(struct daObjFl_Puzzle_c) == 0x33c ? 1 : -1];
#endif

#endif

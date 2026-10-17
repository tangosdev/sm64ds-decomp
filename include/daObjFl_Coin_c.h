#ifndef DAOBJFL_COIN_C_H
#define DAOBJFL_COIN_C_H

#include "types.h"
#include "dActor_c.h"

/* Lethal Lava Land coin-puzzle manager (FL_COIN).
 *
 * _ZTI14daObjFl_Coin_c is at ov064 0x0211bf98 and the name bytes at
 * 0x0211bfb0 say daObjFl_Coin_c. The vtable at 0x0211c1d8 stores that
 * typeinfo, and daObjFl_Coin_c_classInit writes the same vtable after
 * operator new(0xd8). Three byte fields sit at 0xd4. daObjFl_Puzzle_c pieces
 * (src/game/actors/d_a_obj_fl_coin.cpp) find the manager by actor ID, write the
 * first and third and read the second; daCoin_c reads and decrements the
 * third.
 */
/* mPieceFlags bits. A piece whose touch latch (mHadClsn) is set writes
 * FLCOIN_FLAG_PLAYER_TOUCHED over the whole byte every frame, which also
 * clears FLCOIN_FLAG_SCRIPT_WRAPPED. A piece ORs in FLCOIN_FLAG_SCRIPT_WRAPPED
 * when it takes the last entry of its step script (the byte after it is -1)
 * and rewinds the script. */
enum {
    FLCOIN_FLAG_PLAYER_TOUCHED = 1,
    FLCOIN_FLAG_SCRIPT_WRAPPED = 2
};

struct daObjFl_Coin_c : dActor_c {
    u8 pad_0d0[4];
    /* Bits a piece raises (see the FLCOIN_FLAG_* enum above). Behavior moves to
     * phase 1 on a frame where the byte is exactly PLAYER_TOUCHED |
     * SCRIPT_WRAPPED and the player is near. */
    u8 mPieceFlags;   /* 0x0d4 */
    /* 0 until Behavior sees both flag bits with the player within 1000 units,
     * then 1. A piece spawns its coin once it reads 1. */
    u8 mPhase;        /* 0x0d5 */
    /* Count of coins spawned by pieces and not yet gone: a piece's coin spawn
     * adds one, and daCoin_c's destructor subtracts one while it is nonzero.
     * Pieces keep their step timer stopped while it is nonzero. */
    u8 mLiveCoinCount; /* 0x0d6 */
    u8 pad_0d7;

    /* Inline so this TU emits D1 then D0. An out-of-line body emits D2, D0, D1,
     * and the cartridge's run is D1 at 0x02118bec then D0 at 0x02118c10. */
    virtual ~daObjFl_Coin_c() {}

    virtual s32 InitResources(); /* slot 0 */
    virtual s32 Behavior();      /* slot 6 */
};

#ifndef SM64DS_PLATFORM_PC
typedef char daObjFl_Coin_c_size_must_be_0xd8[sizeof(daObjFl_Coin_c) == 0xd8 ? 1 : -1];
#endif

#endif /* DAOBJFL_COIN_C_H */

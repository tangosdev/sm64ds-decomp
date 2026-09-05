#ifndef DASTARGATE_C_H
#define DASTARGATE_C_H

#include "CommonModel.h"
#include "dActor_c.h"

struct Player;

/* Six-byte configuration selected by param1. The member spellings are
   lineage-supported; its stride and every access are ROM-proven. */
struct daStarGateInfo {
    s8 numStars;
    s8 saveFlag;
    s16 notEnoughStarsMsgID;
    s16 notMarioMsgID;
};

/* daStarGate_c_classInit allocates 0x118 bytes, constructs dActor_c, and constructs a
 * CommonModel at 0xd4. D1 destroys that model before chaining to dActor_c.
 * Render writes the model matrix at 0xe0..0x10f, while Behavior reads the
 * state pointer at 0x110; together those uses close the derived layout.
 *
 * SM64DS RTTI names the implementation daStarGate_c. The reconstructed
 * factory daStarGate_c_classInit (historical alias
 * StarDoor_Spawn) constructs it for the STAR_GATE
 * registry profile.
 */
struct daStarGate_c : dActor_c {
    /* The exact nested type spelling is lineage-supported rather than ROM
       text. Its layout is ROM-proven: two 8-byte CodeWarrior PTMFs. */
    struct State {
        typedef bool (daStarGate_c::*StateFunc)(Player *player);

        StateFunc init;
        StateFunc main;
    };

    u8          pad_0d0[0x4];
    CommonModel mModel;          /* 0x0d4 */
    State      *mState;          /* 0x110 */
    u8          mCloseDoorTimer; /* 0x114 */
    u8          pad_115[0x3];

    /* These state names come from later source lineage. Their addresses,
       contents, and use as five separate state descriptors are ROM-proven. */
    static State ST_OPEN_CLOSE;
    static State ST_STAY_CLOSED;
    static State ST_WAIT;
    static State ST_TALKING_TO_PLAYER;
    static State ST_UNLOCKING;

    virtual ~daStarGate_c();

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnPendingDestroy();

    /* The English method names and pointer parameter spellings below are
       lineage-supported reconstructions. The class ownership, argument count,
       PTMF ABI, and call graph are proven by the ROM. */
    bool ChangeState(State *nextState, Player *player);
    Player *CalculateRelativePlayerPos();
    bool IsInFrontOfPlayer(Player *player);
    void TryOpenDoor(Player *player);

    bool St_OpenClose_Init(Player *player);
    bool St_OpenClose_Main(Player *player);
    bool St_StayClosed_Main(Player *player);
    bool St_Wait_Main(Player *player);
    bool St_TalkingToPlayer_Init(Player *player);
    bool St_TalkingToPlayer_Main(Player *player);
    bool St_Unlocking_Init(Player *player);
    bool St_Unlocking_Main(Player *player);
};

typedef char daStarGate_c_State_size_must_be_0x10[
    sizeof(daStarGate_c::State) == 0x10 ? 1 : -1];
typedef char daStarGate_c_size_must_be_0x118[
    sizeof(daStarGate_c) == 0x118 ? 1 : -1];
typedef char daStarGateInfo_size_must_be_0x6[
    sizeof(daStarGateInfo) == 0x6 ? 1 : -1];

#endif /* DASTARGATE_C_H */

#ifndef DSCMGBOMROOM_C_H
#define DSCMGBOMROOM_C_H
#include "dScMgBase_c.h"

/* Bob-omb sorting minigame (dScMgBomroom_c : dScMgBase_c). Fields below
 * 0x4660 belong to the base and stay raw; the bombs themselves start there.
 *
 * The name is the ROM's RTTI name, and rtti_extract.py confirms it is a
 * leaf (no RTTI record names it as a base). dScMgBomroom_c_classInit (alias
 * MgSortOrSplode_Spawn) installs its vtable for the MG_BOMROOM profile.
 */
/* One bomb: 0x70 records at +0x4660, stride 0x40. grabX/grabY are the
   stylus offset at the grab; speed is the Fix12 step along angle. */
struct dScMgBomroom_Bomb {
    s32 x;          /* +0x00 -- Fix12 */
    s32 y;          /* +0x04 -- Fix12 */
    s32 grabX;      /* +0x08 -- stylus x minus bomb x, Fix12 */
    s32 grabY;      /* +0x0c -- stylus y minus bomb y, Fix12 */
    s32 speed;      /* +0x10 -- Fix12 per-frame step; scaled by sin/cos of
                       angle while roaming, added directly when
                       walking to its slot */
    s32 unk_14;     /* +0x14 */
    s32 unk_18;     /* +0x18 */
    s32 prevX;      /* +0x1c -- position before this frame's move */
    s32 prevY;      /* +0x20 */
    s32 sound;      /* +0x24 -- sound handle; func_02012468 takes it back
                       and returns it each frame */
    s32 dragSound;  /* +0x28 -- sound 0x1e5 handle while the bomb is dragged */
    u16 angle;      /* +0x2c -- 0..0xffff, sine-table index >> 4 */
    u16 counter;    /* +0x2e -- frame counter, see Bomb_Animate */
    u16 marchDelay; /* +0x30 -- frames before it walks to its line-up slot (8 per bomb) */
    u16 fuse;       /* +0x32 -- fuse; at 0x40 the bomb flashes, at 0 it explodes */
    u8 type;        /* +0x34 -- anim row; picks frame time/count and sfx */
    u8 frame;       /* +0x35 */
    u8 color;       /* +0x36 -- 0/1; also picks which pen box it bounces in */
    u8 state;       /* +0x37 -- 5 = settled; indexes sBomroomBombStateTable */
    u8 active;      /* +0x38 */
    u8 visible;     /* +0x39 -- DrawBombs skips the bomb while 0 */
    u8 pen;         /* +0x3a -- which blast marked it; 0 = was loose */
    u8 substate;    /* +0x3b -- indexes sBomroomBombMarchTable */
    u8 lineSlot;    /* +0x3c -- line-up slot: row = slot / 10, column = slot % 10 */
    u8 exploded;    /* +0x3d -- set when the bomb goes off */
    u8 overPen;     /* +0x3e -- held over its own pen; sound 0x1e4 on entry */
    u8 pad_3f;
};

/* A 0x10-byte per-slot record: the scene keeps three runs of them between
   +0x6260 and +0x62cf, and +0x0c/+0x0d are always the PMF-table index and
   the dispatch gate. */
struct dScMgBomroom_Slot {
    s32 x;            /* +0x00 -- Fix12 */
    s32 y;            /* +0x04 -- Fix12 */
    u16 timer;        /* +0x08 -- frame counter, see Marker_UpdateAll/020d61dc */
    u8  unk_0a[0x2];
    u8  state;        /* +0x0c -- PMF-table index (markers: animating flag) */
    u8  gate;         /* +0x0d -- dispatch gate (markers: drawn/active) */
    u8  idx;          /* +0x0e -- sprite/sub index */
    u8 level;         /* +0x0f -- see PenGate_StateOpening */
};
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgBomroom_Bomb_size_must_be_0x40[sizeof(struct dScMgBomroom_Bomb) == 0x40 ? 1 : -1];
typedef char dScMgBomroom_Slot_size_must_be_0x10[sizeof(struct dScMgBomroom_Slot) == 0x10 ? 1 : -1];
#endif

struct dScMgBomroom_c : dScMgBase_c {
    virtual ~dScMgBomroom_c();
    virtual s32 InitResources();  /* slot 0 */
    virtual s32 Behavior();       /* slot 6 */
    virtual s32 Render();         /* slot 9 */
    virtual void StartRound(int arg);               /* slot 18 */

    /* scene helpers -- the PMF targets of the seven sinit state tables
       (data_ov006_02141660/680/6c0/6e0/708/730 and the raw {fptr,adj} pairs at
       sBomroomRoundEndTable) plus the per-frame and round bookkeeping they call. */
    void DrawRoundOver();
    void SetRoundOver();
    void UpdateBg2Lift();
    void ResetBg2Lift();
    void Marker_DrawAll();
    void Marker_UpdateAll();
    void Marker_Show(int idx);
    void Marker_HideAll();
    void TopDoor_Close();
    void TopDoor_Open();
    void TopDoor_Draw();
    void TopDoor_StateClosing(int index);
    void TopDoor_StateOpen();
    void TopDoor_StateOpening(int index);
    void TopDoor_StateShut(int index);
    void TopDoor_Update();
    void TopDoor_Show();
    void TopDoor_Hide();
    void PenGate_DrawAll();
    void PenGate_StateClosing(int index);
    void PenGate_StateHold(int index);
    void PenGate_StateOpening(int index);
    void PenGate_StateShut(int index);
    void PenGate_UpdateAll();
    void PenGate_ShowAll();
    void PenGate_Open(int index);
    void PenGate_HideAll();
    void HatchPanel_DrawAll();
    void HatchPanel_StateRevert(int index);
    void HatchPanel_StateHeld();
    void HatchPanel_StateAnimate(int index);
    void HatchPanel_StateIdle(int index);
    void HatchPanel_UpdateAll();
    void HatchPanel_ShowAll();
    void HatchPanel_HideAll();
    void HatchPanel_Activate(int index);
    void DrawRemaining();
    void CountSortedBombs();
    void OnBombExploded(int picked);
    void Bomb_Animate(int index);
    void Bomb_CheckSorted(int index);
    void Bomb_CheckWrongPen(int index);
    void Bomb_CheckGrab(int index);
    void Bomb_PushApart(int index);
    void DrawBombs();
    void CheckColorComplete();
    void BombMarch_LinedUp();
    void BombMarch_ToLine(int index);
    void BombMarch_Offscreen();
    void BombMarch_Exit(int index);
    void BombMarch_ToGate(int index);
    void Bomb_StateMarch(int index);
    void Bomb_StateInPen(int idx);
    void Bomb_StateExploding(int i);
    void Bomb_StateChainDelay(int idx);
    void Bomb_StateHeld(int idx);
    void Bomb_StateWalk(int idx);
    void Bomb_StateWaiting(int i);
    void UpdateBombs();
    void SpawnBombs();
    void HideExplodedBombs();
    void ClearBombs();
    void RoundEnd_Finish();
    void RoundEnd_Tally();
    int AnyBombMarching();
    void RoundEnd_WaitLinedUp();
    void RoundEnd_StartMarch();
    void Round_StateGameOver();
    void Round_StateEnd();
    void Round_StatePlay();
    void Round_StateStart();
    void Round_StateSetup();
    void ResetRound();

    dScMgBomroom_Bomb mBombs[0x70]; /* 0x4660 -- 0x70 x 0x40 */
    /* the 0x10-byte sprite records of the room's animated fixtures (seen in
       melonDS with each draw stubbed; notes/minigames/runtime-checks.md):
         mHatchPanels @0x6260 -- the green panels either side of the bomb hatch
                                 (sBomroomHatchPanelStateTable)
         mPenGates    @0x6280 -- the gates at each pen's outer edge, indexed by
                                 bomb colour (sBomroomPenGateStateTable)
         mTopDoor     @0x62a0 -- the top screen's emblem door
                                 (sBomroomTopDoorStateTable)
         mMarkers     @0x62b0 -- two HUD sprite markers */
    dScMgBomroom_Slot mHatchPanels[2]; /* 0x6260 */
    dScMgBomroom_Slot mPenGates[2];    /* 0x6280 */
    dScMgBomroom_Slot mTopDoor;        /* 0x62a0 */
    dScMgBomroom_Slot mMarkers[2];     /* 0x62b0 */
    s32 mSceneState;    /* 0x62d0 -- round state; 3 once a picked bomb has gone off */
    s32 mSubState;      /* 0x62d4 -- index into the sBomroomRoundEndTable {fptr,adj} pairs */
    s32 mSpawnCount;    /* 0x62d8 -- bombs dropped so far; picks the spawn step */
    s32 mBg2Y;          /* 0x62dc -- BG2 scroll offset, Fix12 */
    u16 mTallyDelay;    /* 0x62e0 -- 0x10 frames between mStateTimer and the tally */
    u16 mSpawnDelay;    /* 0x62e2 -- frames until the next spawn step */
    u16 unk_62e4;       /* 0x62e4 */
    u16 unk_62e6;       /* 0x62e6 */
    u16 mStateTimer;    /* 0x62e8 -- per-state countdown; 0x60 after the first blast */
    u16 mTallyTick;     /* 0x62ea -- counts 0..3; one sorted bomb is tallied every 4th frame */
    u8 pad_62ec[0x2];   /* 0x62ec */
    u16 mTallyCount;    /* 0x62ee -- bombs tallied so far; added to the score */
    u16 mDispatchDelay; /* 0x62f0 -- delay waited out before state dispatch; 0x60 after a blast */
    u16 mBg2Hold;       /* 0x62f2 -- frames BG2 stays raised (state 2) */
    u8 mBg2State;       /* 0x62f4 -- BG2: 0 idle, 1 raising, 2 holding/lowering */
    u8 mWinColor;       /* 0x62f5 -- color that filled its pen */
    u8 mHeldColor;      /* 0x62f6 -- held bomb's color, 0xff = nothing held */
    u8 unk_62f7;        /* 0x62f7 */
    u8 mBlastPen;       /* 0x62f8 -- first blast's pen + 1; 0 = none yet */
    u8 mShowTally;      /* 0x62f9 -- DrawRemaining draws mTallyCount while set */
    u8 mRoundOver;      /* 0x62fa -- gates the round-over sprite */
    u8 mFusesFrozen;    /* 0x62fb -- set when a pen fills; the fuses stop */
    u8 mSpawnHatchFlip; /* 0x62fc -- SpawnBombs toggles bit 0 on each wave of
                           spawn steps 7, 8 and 11: clear sends the wave from
                           hatch 0 (y 0), set from hatch 1 (y 0xb8); step 8
                           also adds the bit to the bomb count. ResetRound
                           clears it. */
    u8 pad_62fd[0x3];   /* 0x62fd -- tail padding to the 0x6300 allocation */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgBomroom_c_size_must_be_0x6300[sizeof(struct dScMgBomroom_c) == 0x6300 ? 1 : -1];
#endif

#endif

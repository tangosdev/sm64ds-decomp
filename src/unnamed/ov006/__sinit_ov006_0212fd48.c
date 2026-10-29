/* ov006 static initializer at 0x0212fd48 (0x368): fills dScMgBomroom_c's
 * seven pointer-to-member state tables (round, round end, bomb, bomb march,
 * hatch panel, pen gate, top door) from the pointer-to-member constants
 * the compiler laid down in .data (pmf_dScMgBomroom_c_*).
 *
 * Not folded: its owner is d_s_mg_bomroom.cpp, but that TU claims neither
 * the constants' .data run (0x0213b9xx) nor the tables' .bss (from
 * 0x021416e0), and emitting this from real pointer-to-member table
 * definitions needs both claimed first.
 */

/* A pointer to member function as the ABI stores it: function and this
   adjustment. */
struct Pmf {
    int fn, adj;
};
struct Dst4 {
    struct Pmf p0, p1, p2, p3;
};
struct Dst5 {
    struct Pmf p0, p1, p2, p3, p4;
};
struct Dst7 {
    struct Pmf p0, p1, p2, p3, p4, p5, p6;
};

extern struct Pmf pmf_dScMgBomroom_c_Round_StateSetup;
extern struct Pmf pmf_dScMgBomroom_c_Round_StateStart;
extern struct Pmf pmf_dScMgBomroom_c_Round_StatePlay;
extern struct Pmf pmf_dScMgBomroom_c_Round_StateEnd;
extern struct Pmf pmf_dScMgBomroom_c_Round_StateGameOver;
extern struct Pmf pmf_dScMgBomroom_c_RoundEnd_StartMarch;
extern struct Pmf pmf_dScMgBomroom_c_RoundEnd_WaitLinedUp;
extern struct Pmf pmf_dScMgBomroom_c_RoundEnd_Tally;
extern struct Pmf pmf_dScMgBomroom_c_RoundEnd_Finish;
extern struct Pmf pmf_dScMgBomroom_c_Bomb_StateWaiting;
extern struct Pmf pmf_dScMgBomroom_c_Bomb_StateWalk;
extern struct Pmf pmf_dScMgBomroom_c_Bomb_StateHeld;
extern struct Pmf pmf_dScMgBomroom_c_Bomb_StateChainDelay;
extern struct Pmf pmf_dScMgBomroom_c_Bomb_StateExploding;
extern struct Pmf pmf_dScMgBomroom_c_Bomb_StateInPen;
extern struct Pmf pmf_dScMgBomroom_c_Bomb_StateMarch;
extern struct Pmf pmf_dScMgBomroom_c_BombMarch_ToGate;
extern struct Pmf pmf_dScMgBomroom_c_BombMarch_Exit;
extern struct Pmf pmf_dScMgBomroom_c_BombMarch_Offscreen;
extern struct Pmf pmf_dScMgBomroom_c_BombMarch_ToLine;
extern struct Pmf pmf_dScMgBomroom_c_BombMarch_LinedUp;
extern struct Pmf pmf_dScMgBomroom_c_HatchPanel_StateIdle;
extern struct Pmf pmf_dScMgBomroom_c_HatchPanel_StateAnimate;
extern struct Pmf pmf_dScMgBomroom_c_HatchPanel_StateHeld;
extern struct Pmf pmf_dScMgBomroom_c_HatchPanel_StateRevert;
extern struct Pmf pmf_dScMgBomroom_c_PenGate_StateShut;
extern struct Pmf pmf_dScMgBomroom_c_PenGate_StateOpening;
extern struct Pmf pmf_dScMgBomroom_c_PenGate_StateHold;
extern struct Pmf pmf_dScMgBomroom_c_PenGate_StateClosing;
extern struct Pmf pmf_dScMgBomroom_c_TopDoor_StateShut;
extern struct Pmf pmf_dScMgBomroom_c_TopDoor_StateOpening;
extern struct Pmf pmf_dScMgBomroom_c_TopDoor_StateOpen;
extern struct Pmf pmf_dScMgBomroom_c_TopDoor_StateClosing;

extern struct Dst5 sBomroomRoundStateTable;
extern struct Dst4 sBomroomRoundEndTable;
extern struct Dst7 sBomroomBombStateTable;
extern struct Dst5 sBomroomBombMarchTable;
extern struct Dst4 sBomroomHatchPanelStateTable;
extern struct Dst4 sBomroomPenGateStateTable;
extern struct Dst4 sBomroomTopDoorStateTable;

void __sinit_ov006_0212fd48(void) {
    sBomroomRoundStateTable.p0 = pmf_dScMgBomroom_c_Round_StateSetup;
    sBomroomRoundStateTable.p1 = pmf_dScMgBomroom_c_Round_StateStart;
    sBomroomRoundStateTable.p2 = pmf_dScMgBomroom_c_Round_StatePlay;
    sBomroomRoundStateTable.p3 = pmf_dScMgBomroom_c_Round_StateEnd;
    sBomroomRoundStateTable.p4 = pmf_dScMgBomroom_c_Round_StateGameOver;

    sBomroomRoundEndTable.p0 = pmf_dScMgBomroom_c_RoundEnd_StartMarch;
    sBomroomRoundEndTable.p1 = pmf_dScMgBomroom_c_RoundEnd_WaitLinedUp;
    sBomroomRoundEndTable.p2 = pmf_dScMgBomroom_c_RoundEnd_Tally;
    sBomroomRoundEndTable.p3 = pmf_dScMgBomroom_c_RoundEnd_Finish;

    sBomroomBombStateTable.p0 = pmf_dScMgBomroom_c_Bomb_StateWaiting;
    sBomroomBombStateTable.p1 = pmf_dScMgBomroom_c_Bomb_StateWalk;
    sBomroomBombStateTable.p2 = pmf_dScMgBomroom_c_Bomb_StateHeld;
    sBomroomBombStateTable.p3 = pmf_dScMgBomroom_c_Bomb_StateChainDelay;
    sBomroomBombStateTable.p4 = pmf_dScMgBomroom_c_Bomb_StateExploding;
    sBomroomBombStateTable.p5 = pmf_dScMgBomroom_c_Bomb_StateInPen;
    sBomroomBombStateTable.p6 = pmf_dScMgBomroom_c_Bomb_StateMarch;

    sBomroomBombMarchTable.p0 = pmf_dScMgBomroom_c_BombMarch_ToGate;
    sBomroomBombMarchTable.p1 = pmf_dScMgBomroom_c_BombMarch_Exit;
    sBomroomBombMarchTable.p2 = pmf_dScMgBomroom_c_BombMarch_Offscreen;
    sBomroomBombMarchTable.p3 = pmf_dScMgBomroom_c_BombMarch_ToLine;
    sBomroomBombMarchTable.p4 = pmf_dScMgBomroom_c_BombMarch_LinedUp;

    sBomroomHatchPanelStateTable.p0 = pmf_dScMgBomroom_c_HatchPanel_StateIdle;
    sBomroomHatchPanelStateTable.p1 = pmf_dScMgBomroom_c_HatchPanel_StateAnimate;
    sBomroomHatchPanelStateTable.p2 = pmf_dScMgBomroom_c_HatchPanel_StateHeld;
    sBomroomHatchPanelStateTable.p3 = pmf_dScMgBomroom_c_HatchPanel_StateRevert;

    sBomroomPenGateStateTable.p0 = pmf_dScMgBomroom_c_PenGate_StateShut;
    sBomroomPenGateStateTable.p1 = pmf_dScMgBomroom_c_PenGate_StateOpening;
    sBomroomPenGateStateTable.p2 = pmf_dScMgBomroom_c_PenGate_StateHold;
    sBomroomPenGateStateTable.p3 = pmf_dScMgBomroom_c_PenGate_StateClosing;

    sBomroomTopDoorStateTable.p0 = pmf_dScMgBomroom_c_TopDoor_StateShut;
    sBomroomTopDoorStateTable.p1 = pmf_dScMgBomroom_c_TopDoor_StateOpening;
    sBomroomTopDoorStateTable.p2 = pmf_dScMgBomroom_c_TopDoor_StateOpen;
    sBomroomTopDoorStateTable.p3 = pmf_dScMgBomroom_c_TopDoor_StateClosing;
}

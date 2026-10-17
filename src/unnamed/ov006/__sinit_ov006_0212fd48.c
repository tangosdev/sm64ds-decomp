struct Pair { int a, b; };
struct Dst4 { struct Pair p0, p1, p2, p3; };
struct Dst5 { struct Pair p0, p1, p2, p3, p4; };
struct Dst7 { struct Pair p0, p1, p2, p3, p4, p5, p6; };

extern struct Pair pmf_dScMgBomroom_c_Round_StateSetup;
extern struct Pair pmf_dScMgBomroom_c_Round_StateStart;
extern struct Pair pmf_dScMgBomroom_c_Round_StatePlay;
extern struct Pair pmf_dScMgBomroom_c_Round_StateEnd;
extern struct Pair pmf_dScMgBomroom_c_Round_StateGameOver;
extern struct Pair pmf_dScMgBomroom_c_RoundEnd_StartMarch;
extern struct Pair pmf_dScMgBomroom_c_RoundEnd_WaitLinedUp;
extern struct Pair pmf_dScMgBomroom_c_RoundEnd_Tally;
extern struct Pair pmf_dScMgBomroom_c_RoundEnd_Finish;
extern struct Pair pmf_dScMgBomroom_c_Bomb_StateWaiting;
extern struct Pair pmf_dScMgBomroom_c_Bomb_StateWalk;
extern struct Pair pmf_dScMgBomroom_c_Bomb_StateHeld;
extern struct Pair pmf_dScMgBomroom_c_Bomb_StateChainDelay;
extern struct Pair pmf_dScMgBomroom_c_Bomb_StateExploding;
extern struct Pair pmf_dScMgBomroom_c_Bomb_StateInPen;
extern struct Pair pmf_dScMgBomroom_c_Bomb_StateMarch;
extern struct Pair pmf_dScMgBomroom_c_BombMarch_ToGate;
extern struct Pair pmf_dScMgBomroom_c_BombMarch_Exit;
extern struct Pair pmf_dScMgBomroom_c_BombMarch_Offscreen;
extern struct Pair pmf_dScMgBomroom_c_BombMarch_ToLine;
extern struct Pair pmf_dScMgBomroom_c_BombMarch_LinedUp;
extern struct Pair pmf_dScMgBomroom_c_HatchPanel_StateIdle;
extern struct Pair pmf_dScMgBomroom_c_HatchPanel_StateAnimate;
extern struct Pair pmf_dScMgBomroom_c_HatchPanel_StateHeld;
extern struct Pair pmf_dScMgBomroom_c_HatchPanel_StateRevert;
extern struct Pair pmf_dScMgBomroom_c_PenGate_StateShut;
extern struct Pair pmf_dScMgBomroom_c_PenGate_StateOpening;
extern struct Pair pmf_dScMgBomroom_c_PenGate_StateHold;
extern struct Pair pmf_dScMgBomroom_c_PenGate_StateClosing;
extern struct Pair pmf_dScMgBomroom_c_TopDoor_StateShut;
extern struct Pair pmf_dScMgBomroom_c_TopDoor_StateOpening;
extern struct Pair pmf_dScMgBomroom_c_TopDoor_StateOpen;
extern struct Pair pmf_dScMgBomroom_c_TopDoor_StateClosing;

extern struct Dst5 sBomroomRoundStateTable;
extern struct Dst4 sBomroomRoundEndTable;
extern struct Dst7 sBomroomBombStateTable;
extern struct Dst5 sBomroomBombMarchTable;
extern struct Dst4 sBomroomHatchPanelStateTable;
extern struct Dst4 sBomroomPenGateStateTable;
extern struct Dst4 sBomroomTopDoorStateTable;

void __sinit_ov006_0212fd48(void)
{
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

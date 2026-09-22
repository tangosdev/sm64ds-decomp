// THE mwcc POINTER-TO-MEMBER WALL, ov006 STATE-TABLE HALF: the nineteen
// tables run link100 lane SEAT4 seated, census batch B4 OV006-PMF-SWEEP.
//
// READ port/slice_seat4.txt FIRST. It carries the per-class universe proof
// (every cell of every table below filled from slot 0 with no gap, every ROM
// pair {code, 0}, every source record read exactly once and only by its own
// class sinit, every table reachable from exactly two places -- its sinit and
// its ONE dispatcher), the emitted side of each dispatcher read off that TU's
// own /FAsc listing, and the reason each face has the calling convention it
// has.
//
// WHAT THIS FILE IS. Each table is filled at overlay load by the class's own
// __sinit_ov006_* with the ROM's eight-byte {code, adjust} pairs. The matched
// dispatcher reads cell[j] and transfers to the code word with the receiver in
// ecx -- and that code word is a DS address, not a host one. So after the sinit
// has run, and only after every mounted cell has been compared against the
// ROM's own word and a zero adjustment, the code word of each cell is replaced
// by the address of a FACE defined here: a small host function with the calling
// convention the dispatcher's own emitted code needs, forwarding to the state
// body's matched C symbol. THE ADJUSTMENT WORD IS NEVER TOUCHED. Every measured
// pair in every one of these tables has a zero adjustment, so the dispatcher's
// `add ecx, this` leaves the receiver exactly where the ROM leaves it.
//
// THREE CALLING CONVENTIONS, EACH MEASURED RATHER THAN ASSUMED.
//   F1  arity one. `push i / mov ecx,[tab+j*8+4] / mov eax,[tab+j*8] /
//       add ecx,this / call eax`, with NO `add esp,4` after it, so the one
//       pushed word is callee-popped: __fastcall(self, dead edx, int).
//   F0  arity zero. The same without the push: __fastcall(self, dead edx).
//       Two of these dispatchers (_ZN13dScMgMemory_c11StateResultEv and _020f71c8) emit
//       `jmp eax` rather than `call eax` because the dispatch is their last
//       statement -- the tail-jump shape lane PMFB8 recorded. A zero-argument
//       __fastcall face is correct under both: it takes its receiver from ecx,
//       pops nothing, and returns to whoever the jump left on the stack.
//   C0  _ZN15dScMgCurling2_c8BehaviorEv OPEN-CODES the decode in plain ints and transfers
//       with `push obj / call eax`, and the caller cleans up (the `add esp,12`
//       that covers this call and the two ordinary ones after it). So table
//       data_ov006_02141a18 takes __cdecl(void *self) faces. A __fastcall face
//       there would pop the argument the caller also pops.
//
// THE SEAT ORDER IS NOT NEW. Every table is seated from inside the per-class
// installer hal/scene_mg.cpp already calls at overlay load, one call added to
// each, so nothing in the boot order moves and no new call site is needed.
//
// NO FACE IS INVENTED. Every state address in every cell already had a written
// host switch case in that class's port/unmatched file, naming the symbol and the
// argument list its own src TU defines; the faces below keep those exactly,
// casts included.

#include <cstdio>
#include <cstdlib>

/* The eight-byte mwcc member pointer, in the only spelling true on both
   machines: two words, no member-pointer type anywhere. */
struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the nineteen mount tables, re-typed to the ROM's eight-byte pair */
extern MgPmf data_ov006_02141708[];
extern MgPmf data_ov006_021417c8[];
extern MgPmf data_ov006_021417e8[];
extern MgPmf data_ov006_021418c0[];
extern MgPmf data_ov006_021419a0[];
extern MgPmf data_ov006_021419b8[];
extern MgPmf data_ov006_021419f8[];
extern MgPmf data_ov006_02141a18[];
extern MgPmf data_ov006_02141f1c[];
extern MgPmf data_ov006_02141fac[];
extern MgPmf data_ov006_021421ec[];
extern MgPmf data_ov006_0214221c[];
extern MgPmf data_ov006_021422bc[];
extern MgPmf data_ov006_02142304[];
extern MgPmf data_ov006_021423c0[];
extern MgPmf data_ov006_02142440[];
extern MgPmf data_ov006_02143020[];
extern MgPmf data_ov006_02143050[];
extern MgPmf data_ov006_02143070[];

/* the state bodies, each with the parameter list ITS OWN src TU defines --
   taken from that class's host switch, which is where it was settled */
void func_ov006_020dc294(void);
void func_ov006_020dc1c4(void *thiz, int idx);
void func_ov006_020dc154(char *base, int idx);
void func_ov006_020dbf7c(void *sb, int i);
void func_ov006_020dc6d0(int o, int idx);
void func_ov006_020dc5c4(char *c, int i);
void func_ov006_020dc4c8(char *c, int i);
void func_ov006_020dc4b0(char *c, int i);
void func_ov006_020dc414(char *base, int idx);
void func_ov006_020d7a84(char *c, int i);
void func_ov006_020d795c(char *o, int i);
void func_ov006_020d7958(void);
void func_ov006_020d777c(char *self, int idx);
void func_ov006_020d7778(void);
void func_ov006_020e1264(char *c, int idx);
void func_ov006_020e1214(char *base, int idx);
void func_ov006_020e0d84(char *c, int i);
void _ZN15dScMgCurling2_c12StepYRestartEi(char *c, int i);
void _ZN15dScMgCurling2_c11StepYRampUpEi(char *c, int i);
void _ZN15dScMgCurling2_c13StepYRampDownEi(char *c, int i);
void _ZN15dScMgCurling2_c13StepXPickFastEi(char *c, int i);
void _ZN15dScMgCurling2_c16StepXPushPosFastEi(char *c, int i);
void _ZN15dScMgCurling2_c16StepXPushNegFastEi(char *c, int i);
void _ZN15dScMgCurling2_c15StepXSettleFastEi(char *c, int i);
void _ZN15dScMgCurling2_c9StepXPickEi(char *c, int i);
void _ZN15dScMgCurling2_c12StepXPushPosEi(char *c, int i);
void _ZN15dScMgCurling2_c12StepXPushNegEi(char *c, int i);
void _ZN15dScMgCurling2_c11StepXSettleEi(char *c, int i);
void func_ov006_020e64e4(int c);
void func_ov006_020e6354(char *c);
void func_ov006_020e628c(char *c);
void func_ov006_020e61c4(char *c);
void func_ov006_020e61c0(void);
void func_ov006_020e8928(char *o, int idx);
void func_ov006_020e8830(char *base, int idx);
void func_ov006_020e97b0(char *c);
void func_ov006_020e96f4(char *thiz);
void func_ov006_020e968c(char *c);
void func_ov006_020e9670(void *t);
void _ZN12dScMgLuigi_c8IrisHoldEi(char *c, int i);
void _ZN12dScMgLuigi_c8IrisGrowEi(char *c, int i);
void _ZN12dScMgLuigi_c8IrisStopEi(char *c, int i);
void _ZN12dScMgLuigi_c18CurtainStateBounceEi(char *c, int i);
void _ZN12dScMgLuigi_c16CurtainStateHoldEi(char *c, int i);
void _ZN12dScMgLuigi_c16CurtainStateFastEi(char *c, int i);
void _ZN13dScMgMemory_c10ResultWaitEv(char *self);
void _ZN13dScMgMemory_c12ResultRewardEv(char *c);
void _ZN13dScMgMemory_c15ResultTurnCardsEv(char *c);
void _ZN13dScMgMemory_c12ResultFinishEv(void *c);
void _ZN13dScMgMemory_c10RoundStartEv(char *c);
void _ZN13dScMgMemory_c13RoundDealEasyEv(unsigned char *c);
void _ZN13dScMgMemory_c15RoundDealNormalEv(char *c);
void _ZN13dScMgMemory_c13RoundDealHardEv(unsigned char *c);
void *_ZN13dScMgMemory_c13RoundWaitDealEv(char *c);
void _ZN13dScMgMemory_c11RoundRevealEv(char *c);
void _ZN14dScMgMemory2_c10ResultWaitEv(char *c);
void _ZN14dScMgMemory2_c12ResultRewardEv(char *c);
void _ZN14dScMgMemory2_c15ResultTurnCardsEv(char *c);
void _ZN14dScMgMemory2_c12ResultFinishEv(char *p);
void _ZN14dScMgMemory2_c10RoundStartEv(char *c);
void _ZN14dScMgMemory2_c13RoundDealEasyEv(char *thiz);
void _ZN14dScMgMemory2_c15RoundDealNormalEv(char *thiz);
void _ZN14dScMgMemory2_c13RoundDealHardEv(char *c);
void _ZN14dScMgMemory2_c15RoundDealFourthEv(char *thiz);
void _ZN14dScMgMemory2_c14RoundShowCardsEv(char *c);
void _ZN14dScMgMemory2_c14RoundHideCardsEv(char *o);
void _ZN14dScMgMemory2_c15RoundReadyCardsEv(char *c);
void *_ZN14dScMgMemory2_c13RoundWaitDealEv(char *c);
void _ZN14dScMgMemory2_c11RoundRevealEv(char *c);
void func_ov006_02129894(char *c, int i);
void func_ov006_021297c0(unsigned char *base, int idx);
void func_ov006_0212972c(char *c, int idx);
void func_ov006_0212a110(char *c, int idx);
void func_ov006_0212a000(char *base, int index);
void func_ov006_02129eec(char *c, int idx);
void func_ov006_02129e28(char *c, int i);
void func_ov006_02129cb0(char *o, int i);
void func_ov006_02129b74(char *base, int index);
void func_ov006_02129a34(char *base, int i);
void func_ov006_0212992c(char *self, int idx);

/* called from the per-class installers in port/unmatched/Mg*_StateDispatch.cpp */
void port_pmf_seat4_coin(void);
void port_pmf_seat4_bomroom(void);
void port_pmf_seat4_curling(void);
void port_pmf_seat4_curling2(void);
void port_pmf_seat4_esp3d(void);
void port_pmf_seat4_luigi(void);
void port_pmf_seat4_memory1(void);
void port_pmf_seat4_memory2(void);
void port_pmf_seat4_snowball(void);

}  /* extern "C" */

// ---- the dispatch counters ------------------------------------------------
//
// One per cell, so a counted run can say which cells actually routed live
// rather than that the build merely linked. The report below writes them out
// when SM64DS_SEAT4_LOG names a file, and does nothing at all otherwise.

static unsigned g_h_02141708[5];
static unsigned g_h_021417c8[4];
static unsigned g_h_021417e8[5];
static unsigned g_h_021418c0[3];
static unsigned g_h_021419a0[3];
static unsigned g_h_021419b8[4];
static unsigned g_h_021419f8[4];
static unsigned g_h_02141a18[5];
static unsigned g_h_02141f1c[2];
static unsigned g_h_02141fac[4];
static unsigned g_h_021421ec[3];
static unsigned g_h_0214221c[3];
static unsigned g_h_021422bc[4];
static unsigned g_h_02142304[6];
static unsigned g_h_021423c0[4];
static unsigned g_h_02142440[10];
static unsigned g_h_02143020[3];
static unsigned g_h_02143050[4];
static unsigned g_h_02143070[4];

// ---- dScMgCoin_c -------------------------------------------------
/* data_ov006_021417c8, 4 cell(s), arity one */
static void __fastcall s4_021417c8_0(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021417c8[0];
    func_ov006_020dc294();
}
static void __fastcall s4_021417c8_1(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021417c8[1];
    func_ov006_020dc1c4(self_c, arg);
}
static void __fastcall s4_021417c8_2(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021417c8[2];
    func_ov006_020dc154(self_c, arg);
}
static void __fastcall s4_021417c8_3(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021417c8[3];
    func_ov006_020dbf7c(self_c, arg);
}

/* data_ov006_021417e8, 5 cell(s), arity one */
static void __fastcall s4_021417e8_0(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021417e8[0];
    func_ov006_020dc6d0((int)(size_t)self_c, arg);
}
static void __fastcall s4_021417e8_1(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021417e8[1];
    func_ov006_020dc5c4(self_c, arg);
}
static void __fastcall s4_021417e8_2(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021417e8[2];
    func_ov006_020dc4c8(self_c, arg);
}
static void __fastcall s4_021417e8_3(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021417e8[3];
    func_ov006_020dc4b0(self_c, arg);
}
static void __fastcall s4_021417e8_4(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021417e8[4];
    func_ov006_020dc414(self_c, arg);
}

// ---- dScMgBomroom_c ----------------------------------------------
/* data_ov006_02141708, 5 cell(s), arity one */
static void __fastcall s4_02141708_0(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02141708[0];
    func_ov006_020d7a84(self_c, arg);
}
static void __fastcall s4_02141708_1(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02141708[1];
    func_ov006_020d795c(self_c, arg);
}
static void __fastcall s4_02141708_2(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02141708[2];
    func_ov006_020d7958();
}
static void __fastcall s4_02141708_3(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02141708[3];
    func_ov006_020d777c(self_c, arg);
}
static void __fastcall s4_02141708_4(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02141708[4];
    func_ov006_020d7778();
}

// ---- dScMgCurling_c ----------------------------------------------
/* data_ov006_021418c0, 3 cell(s), arity one */
static void __fastcall s4_021418c0_0(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021418c0[0];
    func_ov006_020e1264(self_c, arg);
}
static void __fastcall s4_021418c0_1(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021418c0[1];
    func_ov006_020e1214(self_c, arg);
}
static void __fastcall s4_021418c0_2(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021418c0[2];
    func_ov006_020e0d84(self_c, arg);
}

// ---- dScMgCurling2_c ---------------------------------------------
/* data_ov006_021419a0, 3 cell(s), arity one */
static void __fastcall s4_021419a0_0(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021419a0[0];
    _ZN15dScMgCurling2_c12StepYRestartEi((char *)self_c, arg);
}
static void __fastcall s4_021419a0_1(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021419a0[1];
    _ZN15dScMgCurling2_c11StepYRampUpEi((char *)self_c, arg);
}
static void __fastcall s4_021419a0_2(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021419a0[2];
    _ZN15dScMgCurling2_c13StepYRampDownEi((char *)self_c, arg);
}

/* data_ov006_021419b8, 4 cell(s), arity one */
static void __fastcall s4_021419b8_0(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021419b8[0];
    _ZN15dScMgCurling2_c13StepXPickFastEi((char *)self_c, arg);
}
static void __fastcall s4_021419b8_1(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021419b8[1];
    _ZN15dScMgCurling2_c16StepXPushPosFastEi((char *)self_c, arg);
}
static void __fastcall s4_021419b8_2(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021419b8[2];
    _ZN15dScMgCurling2_c16StepXPushNegFastEi((char *)self_c, arg);
}
static void __fastcall s4_021419b8_3(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021419b8[3];
    _ZN15dScMgCurling2_c15StepXSettleFastEi((char *)self_c, arg);
}

/* data_ov006_021419f8, 4 cell(s), arity one */
static void __fastcall s4_021419f8_0(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021419f8[0];
    _ZN15dScMgCurling2_c9StepXPickEi((char *)self_c, arg);
}
static void __fastcall s4_021419f8_1(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021419f8[1];
    _ZN15dScMgCurling2_c12StepXPushPosEi((char *)self_c, arg);
}
static void __fastcall s4_021419f8_2(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021419f8[2];
    _ZN15dScMgCurling2_c12StepXPushNegEi((char *)self_c, arg);
}
static void __fastcall s4_021419f8_3(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021419f8[3];
    _ZN15dScMgCurling2_c11StepXSettleEi((char *)self_c, arg);
}

/* data_ov006_02141a18, 5 cell(s), open-coded, cdecl */
static void s4_02141a18_0(void *self)
{
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02141a18[0];
    func_ov006_020e64e4((int)(size_t)self_c);
}
static void s4_02141a18_1(void *self)
{
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02141a18[1];
    func_ov006_020e6354((char *)self_c);
}
static void s4_02141a18_2(void *self)
{
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02141a18[2];
    func_ov006_020e628c((char *)self_c);
}
static void s4_02141a18_3(void *self)
{
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02141a18[3];
    func_ov006_020e61c4((char *)self_c);
}
static void s4_02141a18_4(void *self)
{
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02141a18[4];
    func_ov006_020e61c0();
}

// ---- dScMg3DEsp_c ------------------------------------------------
/* data_ov006_02141f1c, 2 cell(s), arity one */
static void __fastcall s4_02141f1c_0(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02141f1c[0];
    func_ov006_020e8928(self_c, arg);
}
static void __fastcall s4_02141f1c_1(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02141f1c[1];
    func_ov006_020e8830(self_c, arg);
}

/* data_ov006_02141fac, 4 cell(s), arity zero */
static void __fastcall s4_02141fac_0(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02141fac[0];
    func_ov006_020e97b0(self_c);
}
static void __fastcall s4_02141fac_1(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02141fac[1];
    func_ov006_020e96f4(self_c);
}
static void __fastcall s4_02141fac_2(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02141fac[2];
    func_ov006_020e968c(self_c);
}
static void __fastcall s4_02141fac_3(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02141fac[3];
    func_ov006_020e9670(self_c);
}

// ---- dScMgLuigi_c ------------------------------------------------
/* data_ov006_021421ec, 3 cell(s), arity one */
static void __fastcall s4_021421ec_0(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021421ec[0];
    _ZN12dScMgLuigi_c8IrisHoldEi(self_c, arg);
}
static void __fastcall s4_021421ec_1(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021421ec[1];
    _ZN12dScMgLuigi_c8IrisGrowEi(self_c, arg);
}
static void __fastcall s4_021421ec_2(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021421ec[2];
    _ZN12dScMgLuigi_c8IrisStopEi(self_c, arg);
}

/* data_ov006_0214221c, 3 cell(s), arity one */
static void __fastcall s4_0214221c_0(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_0214221c[0];
    _ZN12dScMgLuigi_c18CurtainStateBounceEi(self_c, arg);
}
static void __fastcall s4_0214221c_1(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_0214221c[1];
    _ZN12dScMgLuigi_c16CurtainStateHoldEi(self_c, arg);
}
static void __fastcall s4_0214221c_2(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_0214221c[2];
    _ZN12dScMgLuigi_c16CurtainStateFastEi(self_c, arg);
}

// ---- dScMgMemory1_c ----------------------------------------------
/* data_ov006_021422bc, 4 cell(s), arity zero */
static void __fastcall s4_021422bc_0(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021422bc[0];
    _ZN13dScMgMemory_c10ResultWaitEv(self_c);
}
static void __fastcall s4_021422bc_1(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021422bc[1];
    _ZN13dScMgMemory_c12ResultRewardEv(self_c);
}
static void __fastcall s4_021422bc_2(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021422bc[2];
    _ZN13dScMgMemory_c15ResultTurnCardsEv(self_c);
}
static void __fastcall s4_021422bc_3(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021422bc[3];
    _ZN13dScMgMemory_c12ResultFinishEv(self_c);
}

/* data_ov006_02142304, 6 cell(s), arity zero */
static void __fastcall s4_02142304_0(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02142304[0];
    _ZN13dScMgMemory_c10RoundStartEv(self_c);
}
static void __fastcall s4_02142304_1(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02142304[1];
    _ZN13dScMgMemory_c13RoundDealEasyEv((unsigned char *)self_c);
}
static void __fastcall s4_02142304_2(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02142304[2];
    _ZN13dScMgMemory_c15RoundDealNormalEv(self_c);
}
static void __fastcall s4_02142304_3(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02142304[3];
    _ZN13dScMgMemory_c13RoundDealHardEv((unsigned char *)self_c);
}
static void __fastcall s4_02142304_4(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02142304[4];
    _ZN13dScMgMemory_c13RoundWaitDealEv(self_c);
}
static void __fastcall s4_02142304_5(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02142304[5];
    _ZN13dScMgMemory_c11RoundRevealEv(self_c);
}

// ---- dScMgMemory2_c ----------------------------------------------
/* data_ov006_021423c0, 4 cell(s), arity zero */
static void __fastcall s4_021423c0_0(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021423c0[0];
    _ZN14dScMgMemory2_c10ResultWaitEv(self_c);
}
static void __fastcall s4_021423c0_1(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021423c0[1];
    _ZN14dScMgMemory2_c12ResultRewardEv(self_c);
}
static void __fastcall s4_021423c0_2(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021423c0[2];
    _ZN14dScMgMemory2_c15ResultTurnCardsEv(self_c);
}
static void __fastcall s4_021423c0_3(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_021423c0[3];
    _ZN14dScMgMemory2_c12ResultFinishEv(self_c);
}

/* data_ov006_02142440, 10 cell(s), arity zero */
static void __fastcall s4_02142440_0(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02142440[0];
    _ZN14dScMgMemory2_c10RoundStartEv(self_c);
}
static void __fastcall s4_02142440_1(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02142440[1];
    _ZN14dScMgMemory2_c13RoundDealEasyEv(self_c);
}
static void __fastcall s4_02142440_2(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02142440[2];
    _ZN14dScMgMemory2_c15RoundDealNormalEv(self_c);
}
static void __fastcall s4_02142440_3(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02142440[3];
    _ZN14dScMgMemory2_c13RoundDealHardEv(self_c);
}
static void __fastcall s4_02142440_4(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02142440[4];
    _ZN14dScMgMemory2_c15RoundDealFourthEv(self_c);
}
static void __fastcall s4_02142440_5(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02142440[5];
    _ZN14dScMgMemory2_c14RoundShowCardsEv(self_c);
}
static void __fastcall s4_02142440_6(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02142440[6];
    _ZN14dScMgMemory2_c14RoundHideCardsEv(self_c);
}
static void __fastcall s4_02142440_7(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02142440[7];
    _ZN14dScMgMemory2_c15RoundReadyCardsEv(self_c);
}
static void __fastcall s4_02142440_8(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02142440[8];
    _ZN14dScMgMemory2_c13RoundWaitDealEv(self_c);
}
static void __fastcall s4_02142440_9(void *self, void *dead_edx)
{
    (void)dead_edx;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02142440[9];
    _ZN14dScMgMemory2_c11RoundRevealEv(self_c);
}

// ---- dScMgSnowball_c ---------------------------------------------
/* data_ov006_02143020, 3 cell(s), arity one */
static void __fastcall s4_02143020_0(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02143020[0];
    func_ov006_02129894(self_c, arg);
}
static void __fastcall s4_02143020_1(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02143020[1];
    func_ov006_021297c0((unsigned char *)self_c, arg);
}
static void __fastcall s4_02143020_2(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02143020[2];
    func_ov006_0212972c(self_c, arg);
}

/* data_ov006_02143050, 4 cell(s), arity one */
static void __fastcall s4_02143050_0(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02143050[0];
    func_ov006_0212a110(self_c, arg);
}
static void __fastcall s4_02143050_1(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02143050[1];
    func_ov006_0212a000(self_c, arg);
}
static void __fastcall s4_02143050_2(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02143050[2];
    func_ov006_02129eec(self_c, arg);
}
static void __fastcall s4_02143050_3(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02143050[3];
    func_ov006_02129e28(self_c, arg);
}

/* data_ov006_02143070, 4 cell(s), arity one */
static void __fastcall s4_02143070_0(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02143070[0];
    func_ov006_02129cb0(self_c, arg);
}
static void __fastcall s4_02143070_1(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02143070[1];
    func_ov006_02129b74(self_c, arg);
}
static void __fastcall s4_02143070_2(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02143070[2];
    func_ov006_02129a34(self_c, arg);
}
static void __fastcall s4_02143070_3(void *self, void *dead_edx, int arg)
{
    (void)dead_edx; (void)arg;
    char *self_c = (char *)self; (void)self_c;
    ++g_h_02143070[3];
    func_ov006_0212992c(self_c, arg);
}

// ---- the counted run ------------------------------------------------------
//
// SM64DS_SEAT4_LOG names a file; a process that installed these tables appends
// one line per TABLE at exit:
//
//   seat4 scene=<id> table=<addr> class=<name> cells=<n> live=<n> routed=<n> hits=<per cell>
//
// A table that never routes writes routed=0, which is that scene's answer for it.

static void s4_report(void)
{
    const char *path = std::getenv("SM64DS_SEAT4_LOG");
    if (!path || !*path) return;
    const char *scene = std::getenv("SM64DS_SCENE");
    std::FILE *f = std::fopen(path, "a");
    if (!f) return;
    struct Row { const char *tab; const char *cls; const unsigned *h; unsigned n; };
    static const Row rows[] = {
        {"02141708", "dScMgBomroom_c", g_h_02141708, 5},
        {"021417c8", "dScMgCoin_c", g_h_021417c8, 4},
        {"021417e8", "dScMgCoin_c", g_h_021417e8, 5},
        {"021418c0", "dScMgCurling_c", g_h_021418c0, 3},
        {"021419a0", "dScMgCurling2_c", g_h_021419a0, 3},
        {"021419b8", "dScMgCurling2_c", g_h_021419b8, 4},
        {"021419f8", "dScMgCurling2_c", g_h_021419f8, 4},
        {"02141a18", "dScMgCurling2_c", g_h_02141a18, 5},
        {"02141f1c", "dScMg3DEsp_c", g_h_02141f1c, 2},
        {"02141fac", "dScMg3DEsp_c", g_h_02141fac, 4},
        {"021421ec", "dScMgLuigi_c", g_h_021421ec, 3},
        {"0214221c", "dScMgLuigi_c", g_h_0214221c, 3},
        {"021422bc", "dScMgMemory1_c", g_h_021422bc, 4},
        {"02142304", "dScMgMemory1_c", g_h_02142304, 6},
        {"021423c0", "dScMgMemory2_c", g_h_021423c0, 4},
        {"02142440", "dScMgMemory2_c", g_h_02142440, 10},
        {"02143020", "dScMgSnowball_c", g_h_02143020, 3},
        {"02143050", "dScMgSnowball_c", g_h_02143050, 4},
        {"02143070", "dScMgSnowball_c", g_h_02143070, 4},
    };
    for (unsigned i = 0; i < sizeof rows / sizeof rows[0]; ++i) {
        unsigned routed = 0, live = 0;
        for (unsigned k = 0; k < rows[i].n; ++k) {
            routed += rows[i].h[k];
            if (rows[i].h[k]) ++live;
        }
        std::fprintf(f, "seat4 scene=%s table=%s class=%s cells=%u live=%u "
                     "routed=%u hits=", scene ? scene : "?", rows[i].tab,
                     rows[i].cls, rows[i].n, live, routed);
        for (unsigned k = 0; k < rows[i].n; ++k)
            std::fprintf(f, "%s%u", k ? "," : "", rows[i].h[k]);
        std::fprintf(f, "\n");
    }
    std::fclose(f);
}

static void s4_arm_report(void)
{
    static int armed;
    if (armed) return;
    armed = 1;
    std::atexit(s4_report);
}

// ---- the boot installers --------------------------------------------------
//
// Each verifies EVERY mounted cell against the ROM's own code word and a zero
// adjustment BEFORE it writes a single face, and aborts loudly on a
// disagreement. A table whose sinit has not run cannot pass that check, so a
// seat in the wrong order is a loud stop and never a quiet wrong address.

struct S4Seat { MgPmf *table; const char *name; unsigned slot; unsigned rom; void *face; };

static void s4_install(const char *cls, const S4Seat *s, unsigned n)
{
    s4_arm_report();
    for (unsigned i = 0; i < n; ++i) {
        MgPmf *p = &s[i].table[s[i].slot];
        if (p->code != s[i].rom || p->adj != 0) {
            std::fprintf(stderr, "FATAL: %s state table %s slot %u: the sinit "
                         "left %08x/%d, the ROM's own pairs say %08x/0 -- "
                         "WRONG BYTES\n", cls, s[i].name, s[i].slot,
                         p->code, p->adj, s[i].rom);
            std::abort();
        }
        p->code = (unsigned)(size_t)s[i].face;
    }
}

extern "C" void port_pmf_seat4_coin(void)
{
    static int done;
    if (done) return;
    done = 1;
    static const S4Seat seats[] = {
        {data_ov006_021417c8, "021417c8", 0, 0x020dc294u, (void *)s4_021417c8_0},
        {data_ov006_021417c8, "021417c8", 1, 0x020dc1c4u, (void *)s4_021417c8_1},
        {data_ov006_021417c8, "021417c8", 2, 0x020dc154u, (void *)s4_021417c8_2},
        {data_ov006_021417c8, "021417c8", 3, 0x020dbf7cu, (void *)s4_021417c8_3},
        {data_ov006_021417e8, "021417e8", 0, 0x020dc6d0u, (void *)s4_021417e8_0},
        {data_ov006_021417e8, "021417e8", 1, 0x020dc5c4u, (void *)s4_021417e8_1},
        {data_ov006_021417e8, "021417e8", 2, 0x020dc4c8u, (void *)s4_021417e8_2},
        {data_ov006_021417e8, "021417e8", 3, 0x020dc4b0u, (void *)s4_021417e8_3},
        {data_ov006_021417e8, "021417e8", 4, 0x020dc414u, (void *)s4_021417e8_4},
    };
    s4_install("dScMgCoin_c", seats, sizeof seats / sizeof seats[0]);
}

extern "C" void port_pmf_seat4_bomroom(void)
{
    static int done;
    if (done) return;
    done = 1;
    static const S4Seat seats[] = {
        {data_ov006_02141708, "02141708", 0, 0x020d7a84u, (void *)s4_02141708_0},
        {data_ov006_02141708, "02141708", 1, 0x020d795cu, (void *)s4_02141708_1},
        {data_ov006_02141708, "02141708", 2, 0x020d7958u, (void *)s4_02141708_2},
        {data_ov006_02141708, "02141708", 3, 0x020d777cu, (void *)s4_02141708_3},
        {data_ov006_02141708, "02141708", 4, 0x020d7778u, (void *)s4_02141708_4},
    };
    s4_install("dScMgBomroom_c", seats, sizeof seats / sizeof seats[0]);
}

extern "C" void port_pmf_seat4_curling(void)
{
    static int done;
    if (done) return;
    done = 1;
    static const S4Seat seats[] = {
        {data_ov006_021418c0, "021418c0", 0, 0x020e1264u, (void *)s4_021418c0_0},
        {data_ov006_021418c0, "021418c0", 1, 0x020e1214u, (void *)s4_021418c0_1},
        {data_ov006_021418c0, "021418c0", 2, 0x020e0d84u, (void *)s4_021418c0_2},
    };
    s4_install("dScMgCurling_c", seats, sizeof seats / sizeof seats[0]);
}

extern "C" void port_pmf_seat4_curling2(void)
{
    static int done;
    if (done) return;
    done = 1;
    static const S4Seat seats[] = {
        {data_ov006_021419a0, "021419a0", 0, 0x020e3db4u, (void *)s4_021419a0_0},
        {data_ov006_021419a0, "021419a0", 1, 0x020e3ce0u, (void *)s4_021419a0_1},
        {data_ov006_021419a0, "021419a0", 2, 0x020e3c4cu, (void *)s4_021419a0_2},
        {data_ov006_021419b8, "021419b8", 0, 0x020e4630u, (void *)s4_021419b8_0},
        {data_ov006_021419b8, "021419b8", 1, 0x020e4520u, (void *)s4_021419b8_1},
        {data_ov006_021419b8, "021419b8", 2, 0x020e440cu, (void *)s4_021419b8_2},
        {data_ov006_021419b8, "021419b8", 3, 0x020e4348u, (void *)s4_021419b8_3},
        {data_ov006_021419f8, "021419f8", 0, 0x020e41d0u, (void *)s4_021419f8_0},
        {data_ov006_021419f8, "021419f8", 1, 0x020e4094u, (void *)s4_021419f8_1},
        {data_ov006_021419f8, "021419f8", 2, 0x020e3f54u, (void *)s4_021419f8_2},
        {data_ov006_021419f8, "021419f8", 3, 0x020e3e4cu, (void *)s4_021419f8_3},
        {data_ov006_02141a18, "02141a18", 0, 0x020e64e4u, (void *)s4_02141a18_0},
        {data_ov006_02141a18, "02141a18", 1, 0x020e6354u, (void *)s4_02141a18_1},
        {data_ov006_02141a18, "02141a18", 2, 0x020e628cu, (void *)s4_02141a18_2},
        {data_ov006_02141a18, "02141a18", 3, 0x020e61c4u, (void *)s4_02141a18_3},
        {data_ov006_02141a18, "02141a18", 4, 0x020e61c0u, (void *)s4_02141a18_4},
    };
    s4_install("dScMgCurling2_c", seats, sizeof seats / sizeof seats[0]);
}

extern "C" void port_pmf_seat4_esp3d(void)
{
    static int done;
    if (done) return;
    done = 1;
    static const S4Seat seats[] = {
        {data_ov006_02141f1c, "02141f1c", 0, 0x020e8928u, (void *)s4_02141f1c_0},
        {data_ov006_02141f1c, "02141f1c", 1, 0x020e8830u, (void *)s4_02141f1c_1},
        {data_ov006_02141fac, "02141fac", 0, 0x020e97b0u, (void *)s4_02141fac_0},
        {data_ov006_02141fac, "02141fac", 1, 0x020e96f4u, (void *)s4_02141fac_1},
        {data_ov006_02141fac, "02141fac", 2, 0x020e968cu, (void *)s4_02141fac_2},
        {data_ov006_02141fac, "02141fac", 3, 0x020e9670u, (void *)s4_02141fac_3},
    };
    s4_install("dScMg3DEsp_c", seats, sizeof seats / sizeof seats[0]);
}

extern "C" void port_pmf_seat4_luigi(void)
{
    static int done;
    if (done) return;
    done = 1;
    static const S4Seat seats[] = {
        {data_ov006_021421ec, "021421ec", 0, 0x020f002cu, (void *)s4_021421ec_0},
        {data_ov006_021421ec, "021421ec", 1, 0x020effb8u, (void *)s4_021421ec_1},
        {data_ov006_021421ec, "021421ec", 2, 0x020eff20u, (void *)s4_021421ec_2},
        {data_ov006_0214221c, "0214221c", 0, 0x020f0a6cu, (void *)s4_0214221c_0},
        {data_ov006_0214221c, "0214221c", 1, 0x020f088cu, (void *)s4_0214221c_1},
        {data_ov006_0214221c, "0214221c", 2, 0x020f06fcu, (void *)s4_0214221c_2},
    };
    s4_install("dScMgLuigi_c", seats, sizeof seats / sizeof seats[0]);
}

extern "C" void port_pmf_seat4_memory1(void)
{
    static int done;
    if (done) return;
    done = 1;
    static const S4Seat seats[] = {
        {data_ov006_021422bc, "021422bc", 0, 0x020f4888u, (void *)s4_021422bc_0},
        {data_ov006_021422bc, "021422bc", 1, 0x020f47d8u, (void *)s4_021422bc_1},
        {data_ov006_021422bc, "021422bc", 2, 0x020f46ecu, (void *)s4_021422bc_2},
        {data_ov006_021422bc, "021422bc", 3, 0x020f456cu, (void *)s4_021422bc_3},
        {data_ov006_02142304, "02142304", 0, 0x020f4c38u, (void *)s4_02142304_0},
        {data_ov006_02142304, "02142304", 1, 0x020f4bbcu, (void *)s4_02142304_1},
        {data_ov006_02142304, "02142304", 2, 0x020f4b30u, (void *)s4_02142304_2},
        {data_ov006_02142304, "02142304", 3, 0x020f4ad4u, (void *)s4_02142304_3},
        {data_ov006_02142304, "02142304", 4, 0x020f4a40u, (void *)s4_02142304_4},
        {data_ov006_02142304, "02142304", 5, 0x020f49acu, (void *)s4_02142304_5},
    };
    s4_install("dScMgMemory1_c", seats, sizeof seats / sizeof seats[0]);
}

extern "C" void port_pmf_seat4_memory2(void)
{
    static int done;
    if (done) return;
    done = 1;
    static const S4Seat seats[] = {
        {data_ov006_021423c0, "021423c0", 0, 0x020f6538u, (void *)s4_021423c0_0},
        {data_ov006_021423c0, "021423c0", 1, 0x020f6488u, (void *)s4_021423c0_1},
        {data_ov006_021423c0, "021423c0", 2, 0x020f639cu, (void *)s4_021423c0_2},
        {data_ov006_021423c0, "021423c0", 3, 0x020f6230u, (void *)s4_021423c0_3},
        {data_ov006_02142440, "02142440", 0, 0x020f6bf0u, (void *)s4_02142440_0},
        {data_ov006_02142440, "02142440", 1, 0x020f6b78u, (void *)s4_02142440_1},
        {data_ov006_02142440, "02142440", 2, 0x020f6b00u, (void *)s4_02142440_2},
        {data_ov006_02142440, "02142440", 3, 0x020f6a78u, (void *)s4_02142440_3},
        {data_ov006_02142440, "02142440", 4, 0x020f6a00u, (void *)s4_02142440_4},
        {data_ov006_02142440, "02142440", 5, 0x020f6904u, (void *)s4_02142440_5},
        {data_ov006_02142440, "02142440", 6, 0x020f6830u, (void *)s4_02142440_6},
        {data_ov006_02142440, "02142440", 7, 0x020f67a0u, (void *)s4_02142440_7},
        {data_ov006_02142440, "02142440", 8, 0x020f670cu, (void *)s4_02142440_8},
        {data_ov006_02142440, "02142440", 9, 0x020f6678u, (void *)s4_02142440_9},
    };
    s4_install("dScMgMemory2_c", seats, sizeof seats / sizeof seats[0]);
}

extern "C" void port_pmf_seat4_snowball(void)
{
    static int done;
    if (done) return;
    done = 1;
    static const S4Seat seats[] = {
        {data_ov006_02143020, "02143020", 0, 0x02129894u, (void *)s4_02143020_0},
        {data_ov006_02143020, "02143020", 1, 0x021297c0u, (void *)s4_02143020_1},
        {data_ov006_02143020, "02143020", 2, 0x0212972cu, (void *)s4_02143020_2},
        {data_ov006_02143050, "02143050", 0, 0x0212a110u, (void *)s4_02143050_0},
        {data_ov006_02143050, "02143050", 1, 0x0212a000u, (void *)s4_02143050_1},
        {data_ov006_02143050, "02143050", 2, 0x02129eecu, (void *)s4_02143050_2},
        {data_ov006_02143050, "02143050", 3, 0x02129e28u, (void *)s4_02143050_3},
        {data_ov006_02143070, "02143070", 0, 0x02129cb0u, (void *)s4_02143070_0},
        {data_ov006_02143070, "02143070", 1, 0x02129b74u, (void *)s4_02143070_1},
        {data_ov006_02143070, "02143070", 2, 0x02129a34u, (void *)s4_02143070_2},
        {data_ov006_02143070, "02143070", 3, 0x0212992cu, (void *)s4_02143070_3},
    };
    s4_install("dScMgSnowball_c", seats, sizeof seats / sizeof seats[0]);
}

// ---- the /alternatename directives -----------------------------------------
//
// Ten C++ decorations that only the matched TUs spell. Each matched dispatcher
// declares its table at NAMESPACE scope with a member-pointer or
// struct-wrapping-a-member-pointer type, so MSVC decorates the reference while
// the ov006 mount defines the plain C name. Every left-hand side below was read
// off that TU's own /FAsc EXTRN list before the link, and NOTHING IN THE TREE
// DEFINES ANY OF THEM (checked against walk_window.map), so none can be
// silently defeated the way port/tools/alternatename_guard.py exists to catch.
// ?func_020beb68@@3PAXA is the third spelling of ov004's data_ov004_020beb68 --
// src/func_ov006_020de440.cpp declares it `extern void *` at namespace scope,
// where MgCoin_Faces.cpp has the plain C name and MgCoin_StateDispatch.cpp the
// `char *` decoration; all three point at the same storage.
#pragma comment(linker, "/alternatename:?data_ov006_021418c0@@3PAP8C@@AEXH@ZA=_data_ov006_021418c0")
#pragma comment(linker, "/alternatename:?data_ov006_021419a0@@3PAP8C@@AEXH@ZA=_data_ov006_021419a0")
#pragma comment(linker, "/alternatename:?data_ov006_021419f8@@3PAP8C@@AEXH@ZA=_data_ov006_021419f8")
#pragma comment(linker, "/alternatename:?data_ov006_02141fac@@3PAUEntry@@A=_data_ov006_02141fac")
#pragma comment(linker, "/alternatename:?data_ov006_02142304@@3PAUEntry@@A=_data_ov006_02142304")
#pragma comment(linker, "/alternatename:?data_ov006_02142440@@3PAUEntry@@A=_data_ov006_02142440")
#pragma comment(linker, "/alternatename:?data_ov006_02143020@@3PAUEntry@@A=_data_ov006_02143020")
#pragma comment(linker, "/alternatename:?data_ov006_02143070@@3PAUEntry@@A=_data_ov006_02143070")
#pragma comment(linker, "/alternatename:?data_ov006_02141840@@3PAP8C@@AEXH@ZA=_data_ov006_02141840")
#pragma comment(linker, "/alternatename:?func_020beb68@@3PAXA=_data_ov004_020beb68")

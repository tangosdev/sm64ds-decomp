/* HOST COPY of SWOOP's (actor 237, ov065, daBasabasa_c) Behavior -- vtable
 * slot 6, ROM body 0x02117b64.
 *
 * Transcribed line for line from src/_ZN5Swoop8BehaviorEv.cpp; the one
 * change is the state dispatch, for the reason Snufit_Behavior.cpp's banner
 * gives (incomplete-class PMF widening; the ROM reads one word at table+8).
 * Swoop keeps its state-pair pointer at +0x420 and its per-frame fn words
 * were seated by port_ov065_states_seat before Swoop's sinit copied them.
 */
extern "C" {
int _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(void *thiz, void *c);
void _ZN5dCc_c5ClearEv(void *thiz);
void _ZN5dCc_c6UpdateEv(void *thiz);
int _ZN12dEnemyBase_c26UpdateKillByInvincibleCharER10dBgCh_ActrR9ModelAnimj(
    void *thiz, void *wm, void *ma, unsigned j);
void _ZN12dEnemyBase_c11UpdateDeathER10dBgCh_Actr(void *thiz, void *wm);
unsigned short DecIfAbove0_Short(unsigned short *p);
void func_02012694(int, void *);
void _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(void *thiz,
                                                       void *clsn);
void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void *thiz, void *wm,
                                              unsigned j);
char *_ZN8dActor_c13ClosestPlayerEv(void *thiz);
void _ZN9Animation7AdvanceEv(void *thiz);
int _ZNK9Animation12WillHitFrameEi(void *thiz, int frame);
int func_ov065_02117994(char *c);
int func_ov065_0211704c(char *c);

extern char data_ov065_0211d6e0[];
extern char data_ov065_0211d6f0[];
}

/* HOST COPY RETIRED, run link100 lane PMFB7 gate 1. src/_ZN5Swoop8BehaviorEv.cpp
   dispatches its own field now: with /vmg /vmm (block R8) MSVC's pointer to
   member IS the ROM's eight-byte {code, adjust} pair, so the widening this
   banner was written for does not happen. The per-frame half of every state
   cell holds a zero-argument __fastcall face; the enter half does not change,
   because the helper that dispatches it tail-jumps. Measurements in
   port/slice_pmfb7.txt and runs/link100/out/PMFB7/. */

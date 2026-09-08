/* HOST COPY of SNUFIT's (actor 236, ov065, daYurei_Mucho_c) Behavior --
 * vtable slot 6, ROM body 0x02116b84.
 *
 * Transcribed line for line from src/_ZN6Snufit8BehaviorEv.cpp; the ONE
 * change is the state dispatch. The matched TU forms its PMF type over
 * `struct Enemy;` while Enemy is still forward-declared, which MSVC widens
 * to the general 16-byte representation: `Holder { char pad[8]; PMF fn; }`
 * then reads 16 bytes at +8 of an 8-byte-stride pair and dispatches through
 * garbage adjustment fields (the gate-173 measurement). The ROM reads ONE
 * word at table+8 -- the pair's per-frame fn -- and calls it with self.
 * The copy reads that word as a plain function pointer; the words are host
 * bodies because port_ov065_states_seat rewrote the SOURCE statics before
 * Snufit's sinit copied them (port/unmatched/Ov065_StateDispatch.cpp).
 *
 * C-named (extern "C" _ZN6Snufit8BehaviorEv) so the vtable thunk in
 * hal/actor_classes_ov065.cpp calls it directly; the matched TU stays out of
 * slice_w5b.txt with the reason recorded there (the honesty rule -- a
 * shadowed TU would sit in the slice looking seated).
 */
extern "C" {
int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(void *thiz, void *c);
void _ZN12CylinderClsn5ClearEv(void *thiz);
void _ZN12CylinderClsn6UpdateEv(void *thiz);
int func_ov065_0211691c(char *c, unsigned *table);
int _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(
    void *thiz, void *wm, void *ma, unsigned j);
int ApproachAngle(short *target, short from, short start, short speed,
                  short max);
void _ZN5Enemy11UpdateDeathER12WithMeshClsn(void *thiz, void *wm);
unsigned short DecIfAbove0_Short(unsigned short *p);
void _Z14ApproachLinearRiii(int *x, int target, int step);
void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void *thiz,
                                                       void *clsn);
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *thiz, void *wm,
                                              unsigned j);
char *_ZN5Actor13ClosestPlayerEv(void *thiz);
void _ZN9Animation7AdvanceEv(void *thiz);
int func_ov065_0211696c(char *c);
void func_ov065_02115ff0(char *c);

extern short data_02082214[];
extern int data_ov065_0211d670[];
extern int data_ov065_0211d650[];
extern int data_ov065_0211d660[];
}

/* HOST COPY RETIRED, run link100 lane PMFB7 gate 1. src/_ZN6Snufit8BehaviorEv.cpp
   dispatches its own field now: with /vmg /vmm (block R8) MSVC's pointer to
   member IS the ROM's eight-byte {code, adjust} pair, so the widening this
   banner was written for does not happen. The per-frame half of every state
   cell holds a zero-argument __fastcall face; the enter half does not change,
   because the helper that dispatches it tail-jumps. Measurements in
   port/slice_pmfb7.txt and runs/link100/out/PMFB7/. */

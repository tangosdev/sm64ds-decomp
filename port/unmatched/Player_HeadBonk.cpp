/* HOST COPY of src/actors/Player.cpp -- the Player's head-bonk check.
 *
 * WHY THIS FILE EXISTS. Vtable slot 28, Actor::OnHitFromUnderneath(Actor&),
 * has TWO dispatch sites in this port and the two matched transcriptions
 * model it with DIFFERENT calling conventions:
 *
 *   src/func_ov002_020eeca8.cpp   models Actor as a C++ class, so MSVC emits
 *                                 __thiscall: this in ecx, the argument
 *                                 pushed, and the CALLEE pops it.
 *   src/actors/Player.cpp   models the vtable as a plain array of
 *                                 function pointers taking self explicitly:
 *                                     (*(void (**)(void *, char *))
 *                                        (*(int *)a + 0x70))(a, self)
 *                                 which is __cdecl -- both arguments pushed
 *                                 and the CALLER pops eight.
 *
 * On ARM both are the same instruction sequence: r0 = a, r1 = self, branch
 * through the slot. The divergence is an artifact of how the two files were
 * written and exists ONLY on the host. It is invisible until something real
 * is seated in slot 28, and then it is unsatisfiable: a thunk that pops four
 * is right for the first site and corrupts the second, a thunk that pops
 * nothing is right for the second and corrupts the first. One word cannot be
 * both.
 *
 * So the outlier is converted rather than the majority. This copy is the
 * matched source above, line for line, with exactly ONE thing changed: the
 * dispatch goes through a C++ virtual so MSVC emits the same __thiscall the
 * other site emits. Every constant, every field offset, every branch and the
 * whole ClsnResult fill are the byte-locked original's. src/ is untouched;
 * slice_gate10.txt carries this file in place of the .cpp, the
 * TexSeq_Caller_ov002_020e5948 pattern.
 *
 * WHAT IT IS IN THE GAME. The player is rising (IsInAir, +0xa8 > 0), a
 * RaycastLine is cast upward from the player's position, and whatever it hits
 * is looked up by its collision id and told OnHitFromUnderneath. It is
 * bonking your head on something: the reported "softlock when trying to open
 * exclamation box in jrb ship from below" arrives here.
 *
 * PROOF (run linkw, lane l1). Both spellings compiled standalone with the port's
 * own cl, /O2 /FAsc, and read off the listing. src/actors/Player.cpp:
 *
 *     ; (*(void (**)(void *, char *))(*(int *)a + 0x70))(a, self);
 *       8b 01      mov  eax, DWORD PTR [ecx]
 *       56         push esi            ; self
 *       51         push ecx            ; a -- the RECEIVER, pushed, not in ecx
 *       8b 40 70   mov  eax, DWORD PTR [eax+112]
 *       ff d0      call eax
 *       83 c4 08   add  esp, 8         ; the caller cleans BOTH words
 *
 * this file:
 *
 *     ; ((HbActor *)a)->OnHitFromUnderneath(self);
 *       8b 10      mov  edx, DWORD PTR [eax]
 *       8b c8      mov  ecx, eax       ; a -- the receiver, in ecx as `this`
 *       56         push esi            ; self
 *       ff 52 70   call DWORD PTR [edx+112]
 *                                      ; no cleanup: the callee pops it
 *
 * The other dispatcher of the SAME slot, src/func_ov002_020eeca8.cpp, is
 * byte-locked too and spells the call `a->m70(arg)` on a struct of twenty-nine
 * virtuals -- a real C++ virtual, so MSVC emits the thiscall form above. Two
 * matched TUs, one vtable word, two conventions: this divergence lives entirely
 * on the host, since on ARM both are `mov r0,a / mov r1,self / ldr pc,[vt+0x70]`.
 * Thirty classes seat slot 28 and every seat is the __fastcall ret-4 shape
 * (hal/actor_classes.cpp:407 `vt[28] = ac_under`, defined at :175 as
 * `static int __fastcall ac_under(void *s, void *, void *o)`), so the cdecl
 * spelling is the outlier and it is the one that gets converted. No MSVC switch
 * reaches the wanted shape: /Gr puts the second argument in edx and pops
 * nothing, /Gz pops eight.
 */
typedef struct { int a, b; } Pair2i;
typedef struct { int x, y, z; } Vector3;
typedef struct {
    void *tag;
    int f04, f08, f0c, f10, f14;
    unsigned short f18, f1a;
    int f1c, f20, f24;
} ClsnResultTmp;

extern "C" {
extern int _ZN6Player7IsInAirEv(void *self);
extern int _ZN6Player7IsStateERNS_5StateE(void *self, void *st);
extern void _ZN6Player11ChangeStateERNS_5StateE(void *self, void *st);
extern void _ZN9dBgCh_LinC1Ev(void *self);
extern void _ZN9dBgCh_LinD1Ev(void *self);
extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void *self, void *a, void *b, void *act);
extern int _ZN9dBgCh_Lin10DetectClsnEv(void *self);
extern void _ZN9dBgCh_Lin10GetClsnPosEv(void *ret, void *self);
extern void func_02035414(void *bgch);
extern void func_02035428(void *bgch);
extern int func_02037e38(unsigned int *p);
extern unsigned _ZNK5dBgPi9GetClsnIDEv(void *self);
extern void *_ZN8dActor_c10FindWithIDEj(unsigned id);
extern void _ZN5dBgPiD1Ev(void *self);

extern char data_ov002_0211001c;
extern int data_02099368;
/* The quarantine net's interaction-receiver latch (func_02043fdc_hostcopy.cpp):
   name the receiver about to be dispatched so a genuine access violation inside
   its slot freezes the RECEIVER, not the player driving the head-bonk. */
void *port_actor_interaction_begin(void *receiver);
void port_actor_interaction_end(void *prev);
}

/* The one changed line's shape. Twenty-eight filler virtuals put the
   twenty-ninth at vtable+0x70, which is slot 28, so the call MSVC emits is
   `call dword ptr [reg+0x70]` with the argument pushed and the callee
   popping it -- byte for byte what func_ov002_020eeca8 emits. Declared and
   never defined: only the call is wanted, and the object is always a real
   actor carrying a real vtable. */
struct HbActor {
    virtual void v0();  virtual void v1();  virtual void v2();  virtual void v3();
    virtual void v4();  virtual void v5();  virtual void v6();  virtual void v7();
    virtual void v8();  virtual void v9();  virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void OnHitFromUnderneath(char *other);   /* vtable + 0x70 */
};

/* PORT_HOST_ABI: cdecl-vs-thiscall vtable dispatch -- slot 28's OTHER byte-locked dispatcher (src/func_ov002_020eeca8.cpp) spells the same call as a C++ virtual, so one vtable word would have to be both conventions at once; this TU emits `push esi / push ecx / call [eax+0x70] / add esp,8` against thirty __fastcall ret-4 seats (hal/actor_classes.cpp:177,407). Listings in the block above. */
/* func_ov002_020cef84 RETIRED (run link100, lane SEAT6, batch B6).
   Slot 28's OTHER dispatcher (src/func_ov002_020eeca8.cpp) spells the
   call as a C++ virtual, so the seats stay thiscall and this site moves.
   The matched TU src/func_ov002_020cef84.cpp is seated in its place: port/tools/hostgen.py's VIRTUAL_CALL table, word 28 __fastcall, with
   the receiver latch carried over.
   Per-row ROM evidence (referrer, RTTI name, kind:function record, the
   dispatch instruction read at its own address) is in port/slice_seat6.txt. */

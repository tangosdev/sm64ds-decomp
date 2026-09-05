/* HOST COPY of src/func_ov002_020bdb50.cpp -- the Player "let go of the held
 * object" path that, for one held-object kind, hands the object to
 * func_ov002_020d5cec.
 *
 * THE CALLING-CONVENTION SEAM:
 *
 * func_ov002_020d5cec takes ONE argument -- the held object -- and dereferences
 * it (its host body does `mov r4, r0` then uses r4 throughout). On the DS the
 * caller leaves that object live in r0 at the call:
 *
 *     020bdbb8: ldr  r0, [r4, #0x360]   ; r0 = *(this+0x360) = the held object
 *     020bdbbc: ldrh r1, [r0, #0xc]     ; (r0 kept)
 *     ...
 *     020bdbd4: bl   func_ov002_020d5cec ; r0 still = the held object
 *
 * so the argument arrives for free in r0. The matched C spells the call with no
 * argument (`func_ov002_020d5cec(void)`); byte-identical on ARM because the
 * object is already in r0 from the immediately preceding load.
 *
 * On the host that zero-argument call pushes nothing; func_ov002_020d5cec's
 * object reads its first parameter from an unwritten stack slot and dereferences
 * garbage. THE FIX passes the held object explicitly -- exactly the value the
 * ROM leaves in r0: *(char **)(c + 0x360), which the surrounding matched code
 * already spells as `obj`.
 *
 * src/func_ov002_020bdb50.cpp is dropped from slice_gate10.txt in favour of this
 * file; the byte-locked source is unchanged.
 */

/* Actor's vtable as far as this file reaches: slot 18 OnYoshiTryEat() and slot
   19 OnTurnIntoEgg(Player &), spelled as VIRTUALS rather than as raw function
   pointers -- the same correction, for the same reason, as the one already made
   in src/_ZN6Player15St_Swallow_MainEv.cpp.

   THE SEAM. On ARM a raw function-pointer call through the vtable and a virtual
   call are the same three instructions, so the byte-locked source spells either
   one. On the HOST they are not the same at all: a raw pointer call compiles
   __cdecl -- every argument pushed, CALLER cleans -- while every seated
   slot-18/19 face in port/hal is the thiscall-shaped __fastcall.

   WHAT THAT COST, MEASURED OFF THE COMPILED OBJECT (dumpbin /disasm of
   build/port/CMakeFiles/walk_window.dir/unmatched/Player_ReleaseHeldDispatch.cpp.obj):

     SLOT 19, the live fault.
       00000073: 8B 01        mov  eax,[ecx]      ; vt
       00000075: 56           push esi            ; c   (the Player)
       00000076: 51           push ecx            ; obj (the receiver)
       00000077: 8B 40 4C     mov  eax,[eax+4Ch]  ; slot 19
       0000007A: FF D0        call eax
       0000007C: 83 C4 08     add  esp,8          ; CALLER cleans 8 => __cdecl
     The receiver is right by ACCIDENT: ecx was loaded with obj at 0x40
     (`mov ecx,[esi+360h]`) and nothing clobbers it before the call, so the
     __fastcall face reads the correct `this` out of ecx purely by luck of
     register allocation. What is wrong is the SECOND parameter: the face reads
     its `Player &` from [esp+4], which is the last push -- obj, not c. That
     half is SILENT, because Actor::OnTurnIntoEgg is a veneer to
     KillAndTrackInDeathTable which ignores the argument.
     THE FAULT IS THE FOUR-BYTE STACK IMBALANCE. The __fastcall face is `ret 4`,
     so the callee pops 4 and the caller pops 8 again; esp runs 4 high, and the
     pop esi / pop ebp / ret at 0x89..0x8B take the wrong words and return
     through a garbage address. Same signature as the swallow fault.

     SLOT 18, latent rather than crashing. The compiler folded the cleanup --
     `push esi` at 0x25 (for func_ov002_020d71a0) and `push ecx` at 0x31 are
     cleaned by a single `add esp,8` at 0x39 -- and the slot-18 face is a
     two-parameter __fastcall with `ret 0`, so the stack balances. ecx again
     happens to hold obj, so the receiver is right too. It is corrected here as
     well, and the reason is worth stating plainly: correct-by-accident through
     the register allocator is not correct. Spelling it as a virtual is what
     makes it actually right rather than currently lucky.

   HOW IT IS REACHED IN ORDINARY SINGLE PLAYER. Yoshi, with an enemy in his
   mouth, picks up a character cap: the cap actor's state machine
   (src/func_ov002_020b74d0.c:51) calls Player::SetNewHatCharacter, whose first
   act is func_ov002_020bdb50(this, 0). Nothing guards it. The versus route
   through St_InYoshiMouth_Init is real but much narrower. Because the fault
   happens inside an actor's own callback, the actor-walk quarantine net turns
   it into a frozen player. Proof: port/tools/yoshi_cap_proof.py. */
struct HeldActor {
    virtual int v00(); virtual int v01(); virtual int v02(); virtual int v03();
    virtual int v04(); virtual int v05(); virtual int v06(); virtual int v07();
    virtual int v08(); virtual int v09(); virtual int v10(); virtual int v11();
    virtual int v12(); virtual int v13(); virtual int v14(); virtual int v15();
    virtual int v16(); virtual int v17();
    virtual int OnYoshiTryEat();               /* slot 18, +0x48 */
    virtual int OnTurnIntoEgg(char* player);   /* slot 19, +0x4c */
};

extern "C" {
extern void func_ov002_020bdc18(char* c);
extern void Player_ReleaseHeldActor(char* c);
extern void func_ov002_020d71a0(char* p);
extern int func_ov002_020d5cec(char* obj);   /* the real one-arg (held obj) shape */
// PORT_HOST_ABI: implicit-register-arg (callee takes the held object *(this+0x360); rode r0 on ARM).
void func_ov002_020bdb50(char* c, int arg) {
    func_ov002_020bdc18(c);
    Player_ReleaseHeldActor(c);
    if (arg != 0) return;
    if (*(int*)(c+0x360) == 0) return;
    func_ov002_020d71a0(c);
    {
        char* obj = *(char**)(c+0x360);
        int r = ((HeldActor*)obj)->OnYoshiTryEat();
        if (r == 0) return;
    }
    {
        char* obj = *(char**)(c+0x360);
        int b1 = (*(unsigned short*)(obj+0xc) == 0xbf);
        if (b1) {
            func_ov002_020d5cec(obj);   /* <-- the held object, ROM's r0 */
            (*(unsigned short *)(((int)c + 0x6ce))) &= ~2;
        } else {
            ((HeldActor*)obj)->OnTurnIntoEgg(c);
        }
    }
    *(int*)(c+0x360) = 0;
}
}

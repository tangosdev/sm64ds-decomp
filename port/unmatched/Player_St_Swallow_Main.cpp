/* HOST COPY of src/_ZN6Player15St_Swallow_MainEv.cpp -- Yoshi's swallow, the
 * state that lays the egg.
 *
 * WHY THIS FILE EXISTS. This is the Player_HeadBonk / Actor_OnAttacked2Dispatch
 * defect, on the one state the earlier passes never reached: the egg LAY.
 *
 * At swallow-animation frame 7 the state tells the thing in Yoshi's mouth it is
 * becoming an egg. The byte-locked src models the vtable as a plain array of
 * function pointers taking self explicitly:
 *
 *     (*(void (**)(void *, char *))(*(char **)p360 + 0x4c))(p360, this);   // 19
 *     (*(int  (**)(void *))        (*(char **)q    + 0x48))(q);            // 18
 *
 * which MSVC compiles __cdecl. Read off the port's own object file
 * (dumpbin /DISASM on the built .obj, offsets 0x80..0x97):
 *
 *     8b 01        mov  eax, [ecx]          ; ecx = p360
 *     56           push esi                 ; this      -- 2nd argument
 *     51           push ecx                 ; p360      -- the RECEIVER, pushed
 *     8b 40 4c     mov  eax, [eax+4Ch]      ; slot 19
 *     ff d0        call eax
 *     8b 8e 60 03  mov  ecx, [esi+360h]
 *     51           push ecx                 ; the RECEIVER, pushed again
 *     8b 01        mov  eax, [ecx]
 *     8b 40 48     mov  eax, [eax+48h]      ; slot 18
 *     ff d0        call eax
 *     83 c4 0c     add  esp, 0Ch            ; the CALLER cleans all three words
 *
 * Every seated slot-19 body in this port is the __fastcall three-parameter
 * shape -- `static int __fastcall X_egg(void *s, void *, void *p)`, which is
 * byte-for-byte the __thiscall ABI: receiver in ecx, the one argument pushed,
 * and the CALLEE pops it (`ret 4`). See hal/actor_classes.cpp:184 (ac_turn_egg),
 * hal/actor_classes_runtime_spawns.cpp (rs_egg) and the per-class overrides
 * (Goomba's gmb_egg at hal/actor_classes_bob_enemy.cpp, cap_egg at
 * hal/actor_classes_bob_world.cpp). Against the cdecl site above that is wrong
 * TWICE:
 *
 *   1. THE ARGUMENT. The face reads its `p` off the stack at [esp+4], which the
 *      cdecl caller filled with p360 -- the ENEMY -- not with `this`. So
 *      OnTurnIntoEgg(Player &player) is handed the eaten enemy as the player.
 *      Goomba's body (src/func_ov084_0212b344.c) then runs
 *      Actor::GivePlayerCoins, Player::IsCollectingCap and
 *      Player::RegisterEggCoinCount over a Goomba, reading Player fields
 *      hundreds of bytes past the end of it. That is the access violation.
 *   2. THE STACK. The face pops four on the way out while the caller's
 *      `add esp,0Ch` pops those same four again, so esp ends the block four
 *      bytes high and the function's own `pop esi; ret` takes the wrong word.
 *
 * The receiver itself survives by luck at both sites -- MSVC happens to load
 * p360 into ecx to reach its vptr and then pushes ecx -- so the fault is the
 * argument, not the `this`. Fixing the convention fixes both halves at once.
 *
 * WHAT IT LOOKED LIKE. Reported as "producing an egg as yoshi causes yoshi to
 * freeze which softlocks the game", and that is exactly what it is: the AV is
 * raised inside the Player's own state callback, so the actor-walk quarantine
 * net (unmatched/func_02043fdc_hostcopy.cpp) attributes it to the WALKER, and
 * the walker is Yoshi. The Player is frozen, so his state never advances off
 * St_Swallow, the no-control byte St_Swallow_Init set is never cleared, and the
 * level's teardown can never converge because a frozen actor is never
 * dispatched again. One egg ends the run.
 *
 * WHAT CHANGED. This copy is the matched source line for line with exactly ONE
 * thing changed: the two dispatches go through a C++ virtual so MSVC emits
 * __thiscall (receiver in ecx, one push for slot 19, no caller cleanup). Every
 * constant, every field offset, every branch, and the whole frame-3 / frame-7
 * structure are the byte-locked original's. On ARM both spellings are the same
 * three instructions -- r0 = receiver, r1 = argument, branch through the slot --
 * which is why the byte-locked src carries the raw one and why src/ does not
 * have to change. src/ is untouched; slice_gate10.txt carries this file in
 * place of the .cpp, the Player_HeadBonk pattern.
 *
 * St_Swallow_Init is NOT copied. Its slot-18 dispatch is the same cdecl
 * spelling, but slot 18 is the two-parameter __fastcall shape with NO stack
 * argument, so a one-argument cdecl call is stack-balanced against it and MSVC
 * loads the receiver into ecx there too (verified in the same disassembly).
 * It is correct today. So is func_ov002_020d674c, for the same reason. Neither
 * is on this fault and neither is touched here.
 */
#include "decl_Animation.h"
#include "decl_common.h"
#include "Player.h"

extern "C" {
extern unsigned int _ZNK6Player14GetBodyModelIDEjb(void* c, unsigned int a, int b);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, void* v);
extern int func_ov002_020ceaf4(char* c);
extern int _ZN6Player12FinishedAnimEv(char* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(char* c, void* st);
extern void Player_AdvanceAnims(char* c);

extern int data_0209ee90[];
extern int data_ov002_0211067c;
extern int data_ov002_0211013c;
}

/* The Actor vtable as the ROM lays it out, only as far as this file reaches:
   slot 18 OnYoshiTryEat() and slot 19 OnTurnIntoEgg(Player &). Declaring them
   virtual is the whole edit -- it is what makes MSVC put the receiver in ecx
   and let the callee pop the argument, which is what every seated face here
   expects. The eighteen slots before them are placeholders for the offsets. */
struct EggActor {
    virtual int v00(); virtual int v01(); virtual int v02(); virtual int v03();
    virtual int v04(); virtual int v05(); virtual int v06(); virtual int v07();
    virtual int v08(); virtual int v09(); virtual int v10(); virtual int v11();
    virtual int v12(); virtual int v13(); virtual int v14(); virtual int v15();
    virtual int v16(); virtual int v17();
    virtual int OnYoshiTryEat();               /* slot 18, +0x48 */
    virtual int OnTurnIntoEgg(char* player);   /* slot 19, +0x4c */
};

int Player::St_Swallow_Main()
{
    void* p360;

    if (_ZNK9Animation12WillHitFrameEi((char*)(*(void**)(((char*)this) + (_ZNK6Player14GetBodyModelIDEjb(((char*)this), param1 & 0xff, 0) << 2) + 0xdc)) + 0x50, 3)) {
        func_ov002_020d71a0(((char*)this));
        goto L65ec;
    }
    if (_ZNK9Animation12WillHitFrameEi((char*)(*(void**)(((char*)this) + (_ZNK6Player14GetBodyModelIDEjb(((char*)this), param1 & 0xff, 0) << 2) + 0xdc)) + 0x50, 7) == 0) {
        goto L65ec;
    }

    p360 = *(void**)((char*)&mObjInMouth);
    if (p360 == 0) goto L65e4;
    {
        int b = (*(unsigned short*)((char*)p360 + 0xc) == 0xbf);
        if (b == 0) goto L653c;
    }
    *(short*)((char*)p360 + 0x8c) = mAngleX;
    *(short*)((char*)p360 + 0x8e) = mAngleY;
    *(short*)((char*)p360 + 0x90) = mAngleZ;
    func_ov002_020d5ab4(*(void**)((char*)&mObjInMouth));
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(0, 0x100, ((char*)this) + 0x74);
    goto L65e4;

L653c:
    /* was: (*(void (**)(void*, char*))(*(char**)p360 + 0x4c))(p360, (char*)this); */
    ((EggActor*)p360)->OnTurnIntoEgg(((char*)this));
    {
        void* q = *(void**)((char*)&mObjInMouth);
        /* was: if ((*(int (**)(void*))(*(char**)q + 0x48))(q) == 1) */
        if (((EggActor*)q)->OnYoshiTryEat() == 1) goto L65e4;
    }
    if ((unsigned short)(mStateFlags & 0x1000)) goto L65e4;
    {
        unsigned char flag = 0;
        int b_c2 = (*(unsigned short*)((char*)*(void**)((char*)&mObjInMouth) + 0xc) == 0xc2);
        if (b_c2 || *(int*)((char*)data_0209ee90 + 0x238) != 0) {
            flag |= 1;
        }
        *(unsigned char*)(((long long)(int)((char*)&mEggParams))) |= (flag + 1);
    }
    func_ov002_020d6368(((char*)this));
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(0, 0x100, ((char*)this) + 0x74);

L65e4:
    *(void**)((char*)&mObjInMouth) = 0;
L65ec:
    if (mIsUnderwater != 0) {
        *(int*)(((long long)(int)((char*)&mVertSpeed))) += func_ov002_020ceaf4(((char*)this));
    }
    if (_ZN6Player12FinishedAnimEv(((char*)this))) {
        if (mIsUnderwater != 0) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), &data_ov002_0211067c);
        } else {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), &data_ov002_0211013c);
        }
    }
    Player_AdvanceAnims(((char*)this));
    return 1;
}

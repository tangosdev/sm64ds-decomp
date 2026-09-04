//cpp
// @symbol _ZN6Player15St_Swallow_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Animation.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
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

/* Actor's vtable as far as this file reaches: slot 18 OnYoshiTryEat() and slot
   19 OnTurnIntoEgg(Player &), spelled as VIRTUALS rather than as raw function
   pointers.

   On ARM the two spellings are the same three instructions -- r0 = receiver,
   r1 = argument, branch through the slot -- so this file still byte-matches
   the ROM (mwccarm 1.2 base/sp2/sp2p3, strict relocs, 0x020d6474 size 0x1f8).
   The same is true of every sibling on this path, which is why they already
   spell it this way: St_YoshiPower_Main's `Obj`, func_ov002_020d7430's `Obj`,
   func_ov002_020d6790's `Obj`, func_ov002_020ecd18's `VObj`.

   It is NOT the same on the host. A raw function-pointer call compiles cdecl
   -- receiver pushed, caller cleans -- while every seated slot-18/19 face in
   port/hal is the thiscall-shaped __fastcall: receiver in ecx, the argument
   pushed, and the CALLEE popping it. Against the cdecl spelling the slot-19
   face read its Player& off the stack, where the caller had left the eaten
   ENEMY, and popped four bytes the caller popped again -- so the frame-7 egg
   lay returned through a wild `ret`, took an access violation inside the
   Player's own state callback, and the actor-walk quarantine net froze the
   walker. The walker is Yoshi. That is the reported "producing an egg as
   yoshi causes yoshi to freeze". This spelling is the fix, and it costs the
   ROM nothing. */
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
    ((EggActor*)p360)->OnTurnIntoEgg(((char*)this));
    {
        void* q = *(void**)((char*)&mObjInMouth);
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

#ifndef DMGSTATE_C_H
#define DMGSTATE_C_H

#include "types.h"

/* The shared minigame state machine, embedded in dScMgBase_c at 0xcc. It
 * has no vtable and the ROM has no RTTI for it, so the class name and its
 * member names are coined. The matched methods fix the layout: SetState
 * takes mEnter from a 20-entry table and calls it, Behavior counts mTimer
 * toward 0 and calls mBehavior, Render calls mRender, and mState -1 (the
 * constructor's value) makes Behavior and Render do nothing. */
struct dMgState_c {
    typedef void (dMgState_c::*Callback)();

    Callback mEnter;
    Callback mBehavior;
    Callback mRender;
    s32 mState;
    s32 mTimer;
    s32 unk_020;
    s32 unk_024; /* dScMgBase_c steps and draws the machine only while this is 0 */

    dMgState_c();
    void Render();
    void Behavior();
    void SetState(s32 state);

    /* State handlers: pointer-to-member constants at 0x020bc8bc..0x020bca3c
       (the enter/behavior/render records) name these as targets. */
    void func_ov004_020b68e8();
    void func_ov004_020b6948();
    void func_ov004_020b6ad8();
    void func_ov004_020b6b40();
    void func_ov004_020b6c10();
    void func_ov004_020b6c9c();
    void func_ov004_020b6d6c();
    void func_ov004_020b6ddc();
    void func_ov004_020b6f14();
    void func_ov004_020b6f88();
    void func_ov004_020b7020();
    void func_ov004_020b70b4();
    void func_ov004_020b7124();
    void func_ov004_020b724c();
    void func_ov004_020b72d4();
    void func_ov004_020b743c();
    void func_ov004_020b7460();
    void func_ov004_020b746c();
    void func_ov004_020b7594();
    void func_ov004_020b75e4();
    void func_ov004_020b7744();
    void func_ov004_020b77b4();
    void func_ov004_020b7854();
    void func_ov004_020b78f4();
    void func_ov004_020b798c();
    void func_ov004_020b79b0();
    void func_ov004_020b7a18();
    void func_ov004_020b7b20();
    void func_ov004_020b7b90();
    void func_ov004_020b7c04();
    void func_ov004_020b7cd0();
    void func_ov004_020b7e38();
    void func_ov004_020b7eac();
    void func_ov004_020b7f5c();
    void func_ov004_020b7fec();
    void func_ov004_020b8098();
    void func_ov004_020b81f8();
    void func_ov004_020b8284();
    void func_ov004_020b83ac();
    void func_ov004_020b841c();
    void func_ov004_020b853c();
    void func_ov004_020b8560();
    void func_ov004_020b8688();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dMgState_c_size_must_be_0x28[
    sizeof(dMgState_c) == 0x28 ? 1 : -1];
#endif

#endif
